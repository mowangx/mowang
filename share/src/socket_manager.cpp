
#include "socket_manager.h"

#include "auto_lock.h"
#include "log.h"
#include "socket.h"
#include "socket_api.h"

CSocketManager::CSocketManager()
{
	m_socket_sequence_index = 0;
	m_eventbase = NULL;
	m_sockets.clear();
	m_read_packets.clear();
	m_finish_read_packets.clear();
	m_write_packets.clear();
	m_finish_write_packets.clear();
}

CSocketManager::~CSocketManager()
{
	SOCKET_API::gx_lib_cleanup();
	clean_up();
}

bool CSocketManager::init()
{
	if (!SOCKET_API::gx_lib_init()) {
		return false;
	}
	
	m_eventbase = event_base_new();
	if (NULL == m_eventbase) {
		return false;
	}

	return true;
}

void CSocketManager::update(uint32 diff)
{
	if (-1 == event_base_loop(m_eventbase, EVLOOP_ONCE | EVLOOP_NONBLOCK)) {
		log_info("update ret -1");
		return;
	}

	handle_new_socket();

	handle_unpacket();

	handle_release_packet();

	handle_write_msg();
}

uint32 CSocketManager::socket_num() const
{
	return (uint32)m_sockets.size();
}

void CSocketManager::read_packets(std::vector<TPacketInfo_t*>& packets)
{
	CLock lock(&m_mutex);
	packets.insert(packets.end(), m_read_packets.begin(), m_read_packets.end());
	m_read_packets.clear();
}

void CSocketManager::finish_read_packets(std::vector<TPacketInfo_t*>& packets)
{
	CLock lock(&m_mutex);
	m_finish_read_packets.insert(m_finish_read_packets.begin(), packets.begin(), packets.end());
}

void CSocketManager::write_packets(std::vector<TPacketInfo_t*>& packets)
{
	CLock lock(&m_mutex);
	m_write_packets.insert(m_write_packets.end(), packets.begin(), packets.end());
}

void CSocketManager::finish_write_packets(std::vector<TPacketInfo_t*>& packets)
{
	CLock lock(&m_mutex);
	packets.insert(packets.end(), m_finish_write_packets.begin(), m_finish_write_packets.end());
	m_finish_write_packets.clear();
}

void CSocketManager::test_get_sockets(std::vector<CSocket*>& sockets)
{
	for (auto itr : m_sockets) {
		sockets.push_back(itr.second);
	}
}

bool CSocketManager::on_accept(CSocketWrapper* listener)
{
	CSocket* socket = listener->accept();
	if (NULL == socket) {
		return false;
	}

	// @todo 检测添加的时候会不会影响性能
	TSocketIndex_t index = gen_socket_index();
	socket->set_socket_index(index);
	log_info("Accept socket! index = '%"I64_FMT"u'", index);

	socket->set_packet_handler(listener->create_handler());
	socket->get_packet_handler()->set_socket(socket);
	socket->get_packet_handler()->set_index(index);

	m_new_socket_queue.push(socket);

	return true;
}

void CSocketManager::on_write(CSocket* socket)
{
	if (!socket->on_write()) {
		handle_close_socket(socket, true);
		return;
	}

	if (socket->is_need_write()) {
		TSocketEvent_t& writeEvent = socket->get_write_event();
		event_add(&writeEvent, NULL);
	}
}

void CSocketManager::on_read(CSocket* socket)
{
	if (!socket->is_active()) {
		return;
	}

	if (!socket->on_read()) {
		handle_close_socket(socket, false);
		return;
	}
}

void CSocketManager::handle_new_socket()
{
	std::list<CSocket*> lst;
	m_new_socket_queue.pop(lst);
	for (auto socket : lst) {
		add_socket(socket);
	}
}

void CSocketManager::handle_unpacket()
{
	std::vector<CSocket*> delSockets;

	for (auto itr = m_sockets.begin(); itr != m_sockets.end(); ++itr) {
		CSocket* socket = itr->second;
		if (NULL == socket) {
			continue;
		}

		if (!socket->is_active()) {
			continue;
		}

		if (socket->get_input_len() > 0) {
			handle_socket_unpacket(socket);
		}

		if (!socket->is_active()) {
			delSockets.push_back(socket);
		}
	}

	for (uint32 i = 0; i < delSockets.size(); ++i) {
		handle_close_socket(delSockets[i], false);
	}
}

void CSocketManager::handle_socket_unpacket(CSocket* socket)
{
	int len = socket->get_input_len();
	if (len > MAX_PACKET_READ_SIZE) {
		len = MAX_PACKET_READ_SIZE;
	}
	CSocketHandler* socket_handler = socket->get_socket_handler();
	int cur_len = socket->read(socket_handler->buffer(len), len);
	if (cur_len != len) {
		log_error("socket index = '%"I64_FMT"u', read len is not equal cache len, read len = %d, cache len = %d", socket->get_socket_index(), cur_len, len);
	}
	CBasePacket* packet = socket_handler->unpacket();
	while (NULL != packet) {
		char* packet_pool = m_mem_pool.allocate(packet->get_packet_len());
		memcpy(packet_pool, packet, packet->get_packet_len());
		TPacketInfo_t* packet_info = m_packet_info_pool.allocate();
		packet_info->socket = socket;
		packet_info->packet = (CBasePacket*)packet_pool;
		{
			CLock lock(&m_mutex);
			m_read_packets.push_back(packet_info);
		}
		packet = socket_handler->unpacket();
	}
}

void CSocketManager::handle_write_msg()
{
	std::vector<TPacketInfo_t*> packets;
	{
		CLock lock(&m_mutex);
		packets.insert(packets.begin(), m_write_packets.begin(), m_write_packets.end());
		m_write_packets.clear();
	}
	for (auto packet_info : packets) {
		if (m_sockets.find(packet_info->index) != m_sockets.end()) {
			send_packet(packet_info->socket, (char*)packet_info->packet, packet_info->packet->get_packet_len());
		}
		else {
			log_info("send packet failed for can't find socket index! socket index = '%"I64_FMT"u'", packet_info->index);
		}
		CLock lock(&m_mutex);
		m_finish_write_packets.push_back(packet_info);
	}
}

void CSocketManager::handle_close_socket(CSocket* socket, bool writeFlag)
{
	if (writeFlag) {
		log_info("Write close socke!index = '%"I64_FMT"u'", socket->get_socket_index());
	} else {
		log_info("Read close socke!index = '%"I64_FMT"u'", socket->get_socket_index());
	}

	if (socket->is_active()) {
		socket->get_output_stream()->flush();
	}

	del_socket(socket);
}

void CSocketManager::handle_release_packet()
{
	std::vector<TPacketInfo_t*> packets;
	{
		CLock lock(&m_mutex);
		packets.insert(packets.begin(), m_finish_read_packets.begin(), m_finish_read_packets.end());
		m_finish_read_packets.clear();
	}
	for (auto packet_info : packets) {
		m_mem_pool.deallocate((char*)packet_info->packet);
		m_packet_info_pool.deallocate(packet_info);
	}
}

void CSocketManager::add_socket(CSocket* socket)
{
	TSocketEvent_t& readEvent = socket->get_read_event();
	TSocketEvent_t& writeEvent = socket->get_write_event();
	TSocketEventArg_t& eventArg = socket->get_event_arg();
	eventArg.mgr = this;
	eventArg.s = socket;

	if (0 != event_assign(&readEvent, m_eventbase, socket->get_socket_fd(), EV_READ | EV_PERSIST, CSocketManager::OnReadEvent, &eventArg)) {

	}
	if (0 != event_assign(&writeEvent, m_eventbase, socket->get_socket_fd(), EV_WRITE, CSocketManager::OnWriteEvent, &eventArg)) {

	}

	if (0 != event_add(&readEvent, NULL)) {
	}

	if (0 != event_add(&writeEvent, NULL)) {
	}

	TSocketIndex_t index = socket->get_socket_index();
	if (m_sockets.find(index) != m_sockets.end()) {
		log_error("The socket index is repeat! index = '%"I64_FMT"u'", index);
		return;
	}

	CSocketHandler* socket_handler = m_socket_handler_pool.allocate();
	socket_handler->set_buffer(m_packet_buffer_pool.allocate());
	socket->set_socket_handler(socket_handler);

	m_sockets[index] = socket;
}

void CSocketManager::del_socket(CSocket* socket)
{
	// 将数据全部写出
	socket->on_write();

	TSocketEvent_t& readEvent = socket->get_read_event();
	event_del(&readEvent);
	TSocketEvent_t& writeEvent = socket->get_write_event();
	event_del(&writeEvent);

	// 关闭链接
	socket->close();

	CSocketHandler* socket_handler = socket->get_socket_handler();
	m_packet_buffer_pool.deallocate(socket_handler->buffer());
	m_socket_handler_pool.deallocate(socket_handler);

	m_sockets.erase(socket->get_socket_index());

	delete socket->get_packet_handler();

	DSafeDelete(socket);
}

void CSocketManager::send_packet(CSocket* socket, char* msg, uint32 len)
{
	socket->write(msg, len);
	TSocketEvent_t& writeEvent = socket->get_write_event();
	event_add(&writeEvent, NULL);
}

TSocketIndex_t CSocketManager::gen_socket_index()
{
	return ++m_socket_sequence_index;
}

void CSocketManager::clean_up()
{
	if (NULL != m_eventbase) {
		event_base_free(m_eventbase);
	}
}

void CSocketManager::OnAccept(TSocketFD_t fd, short evt, void* arg)
{
	TSocketWrapperEventArg_t* event_arg = (TSocketWrapperEventArg_t*)arg;
	if (NULL == event_arg || NULL == event_arg->s || NULL == event_arg->mgr) {
		log_error("Cast socket arg failed");
		return;
	}
	for (uint32 i = 0; i < ACCEPT_ONCE_NUM; ++i) {
		if (!event_arg->mgr->on_accept(event_arg->s)) {
			break;
		}
	}
}

void CSocketManager::OnWriteEvent(TSocketFD_t fd, short evt, void* arg)
{
	TSocketEventArg_t* eventArg = (TSocketEventArg_t*)arg;

	if (evt & EV_WRITE) {
		eventArg->mgr->on_write(eventArg->s);
	} else {
		eventArg->mgr->handle_close_socket(eventArg->s, true);
	}
}

void CSocketManager::OnReadEvent(TSocketFD_t fd, short evt, void* arg)
{
	TSocketEventArg_t* eventArg = (TSocketEventArg_t*)arg;

	if (!eventArg->s->is_active()) {
		return;
	}

	if (evt & EV_READ) {
		eventArg->mgr->on_read(eventArg->s);
	} else {
		eventArg->mgr->handle_close_socket(eventArg->s, false);
	}
}