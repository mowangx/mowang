
#include "socket_manager.h"

#include "auto_lock.h"
#include "log.h"
#include "socket.h"
#include "socket_api.h"
#include "game_random.h"

socket_manager::socket_manager()
{
	m_socket_sequence_index = 0;
	m_eventbase = NULL;
	m_read_packets.clear();
	m_finish_read_packets.clear();
	m_write_packets.clear();
	m_finish_write_packets.clear();
	m_new_sockets.clear();
	m_wait_init_sockets.clear();
	m_wait_delete_sockets.clear();
	m_delete_sockets.clear();
	m_sockets.clear();
}

socket_manager::~socket_manager()
{
	SOCKET_API::gx_lib_cleanup();
	clean_up();
}

bool socket_manager::init()
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

void socket_manager::update(uint32 diff)
{
	if (-1 == event_base_loop(m_eventbase, EVLOOP_ONCE | EVLOOP_NONBLOCK)) {
		log_info("update ret -1");
		return;
	}

	handle_new_socket();

	handle_unpacket();

	handle_release_packet();

	handle_write_msg();

	handle_release_socket();
}

void socket_manager::test_kick()
{
	if (DGameRandom.rand_odds(10, 1)) {
		if (m_sockets.empty()) {
			return;
		}
		for (auto itr : m_sockets) {
			log_info("test close socket! socket index = '%"I64_FMT"u'", itr.first);
			handle_close_socket(itr.second, false);
			return;
		}
	}
}

uint32 socket_manager::socket_num() const
{
	return (uint32)m_sockets.size();
}

void socket_manager::read_packets(std::vector<TPacketRecvInfo_t*>& packets, std::vector<socket_base*>& new_sockets, std::vector<socket_base*>& del_sockets)
{
	auto_lock lock(&m_mutex);

	packets.insert(packets.end(), m_read_packets.begin(), m_read_packets.end());
	m_read_packets.clear();

	new_sockets.insert(new_sockets.end(), m_wait_init_sockets.begin(), m_wait_init_sockets.end());
	m_wait_init_sockets.clear();

	del_sockets.insert(del_sockets.end(), m_wait_delete_sockets.begin(), m_wait_delete_sockets.end());
	m_wait_delete_sockets.clear();
}

void socket_manager::finish_read_packets(std::vector<TPacketRecvInfo_t*>& packets, std::vector<socket_base*>& sockets)
{
	auto_lock lock(&m_mutex);

	m_finish_read_packets.insert(m_finish_read_packets.end(), packets.begin(), packets.end());

	m_delete_sockets.insert(m_delete_sockets.end(), sockets.begin(), sockets.end());
}

void socket_manager::write_packets(std::vector<TPacketSendInfo_t*>& packets)
{
	auto_lock lock(&m_mutex);
	m_write_packets.insert(m_write_packets.end(), packets.begin(), packets.end());
}

void socket_manager::finish_write_packets(std::vector<TPacketSendInfo_t*>& packets)
{
	auto_lock lock(&m_mutex);
	packets.insert(packets.end(), m_finish_write_packets.begin(), m_finish_write_packets.end());
	m_finish_write_packets.clear();
}

void socket_manager::test_get_sockets(std::vector<socket_base*>& sockets)
{
	auto_lock lock(&m_mutex);
	for (auto itr : m_sockets) {
		sockets.push_back(itr.second);
	}
}

bool socket_manager::on_accept(socket_wrapper* listener)
{
	socket_base* socket = listener->accept();
	if (NULL == socket) {
		return false;
	}

	// @todo 检测添加的时候会不会影响性能
	TSocketIndex_t index = gen_socket_index();
	socket->set_socket_index(index);
	log_info("Accept socket! index = '%"I64_FMT"u'", index);

	socket->set_packet_handler(listener->create_handler());
	socket->get_packet_handler()->set_socket_index(index);

	m_new_sockets.push_back(socket);

	return true;
}

void socket_manager::on_write(socket_base* socket)
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

void socket_manager::on_read(socket_base* socket)
{
	if (!socket->is_active()) {
		return;
	}

	if (!socket->on_read()) {
		handle_close_socket(socket, false);
		return;
	}
}

void socket_manager::handle_new_socket()
{
	for (auto socket : m_new_sockets) {
		add_socket(socket);
	}
	m_new_sockets.clear();
}

void socket_manager::handle_unpacket()
{
	std::vector<socket_base*> del_sockets;

	for (auto itr = m_sockets.begin(); itr != m_sockets.end(); ++itr) {
		socket_base* socket = itr->second;
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
			del_sockets.push_back(socket);
		}
	}

	for (uint32 i = 0; i < del_sockets.size(); ++i) {
		handle_close_socket(del_sockets[i], false);
	}
}

void socket_manager::handle_socket_unpacket(socket_base* socket)
{
	int len = socket->get_input_len();
	if (len > MAX_PACKET_READ_SIZE) {
		len = MAX_PACKET_READ_SIZE;
	}
	socket_handler* socket_handler = socket->get_socket_handler();
	int cur_len = socket->read(socket_handler->buffer(len), len);
	if (cur_len != len) {
		log_error("socket index = '%"I64_FMT"u', read len is not equal cache len, read len = %d, cache len = %d", socket->get_socket_index(), cur_len, len);
	}
	packet_base* packet = socket_handler->unpacket();
	while (NULL != packet) {
		char* packet_pool = m_mem_pool.allocate(packet->get_packet_len());
		memcpy(packet_pool, packet, packet->get_packet_len());
		TPacketRecvInfo_t* packet_info = m_packet_info_pool.allocate();
		packet_info->socket = socket;
		packet_info->packet = (packet_base*)packet_pool;
		{
			auto_lock lock(&m_mutex);
			m_read_packets.push_back(packet_info);
		}
		packet = socket_handler->unpacket();
	}
}

void socket_manager::handle_write_msg()
{
	std::vector<TPacketSendInfo_t*> packets;
	{
		auto_lock lock(&m_mutex);
		packets.insert(packets.begin(), m_write_packets.begin(), m_write_packets.end());
		m_write_packets.clear();
	}
	for (auto packet_info : packets) {
		send_packet(packet_info->socket_index, (char*)packet_info->packet, packet_info->packet->get_packet_len());
		auto_lock lock(&m_mutex);
		m_finish_write_packets.push_back(packet_info);
	}
}

void socket_manager::handle_close_socket(socket_base* socket, bool write_flag)
{
	if (write_flag) {
		log_info("Write close socke!index = '%"I64_FMT"u'", socket->get_socket_index());
	} else {
		log_info("Read close socke!index = '%"I64_FMT"u'", socket->get_socket_index());
	}

	if (socket->is_active()) {
		socket->get_output_stream()->flush();
	}

	del_socket(socket);
}

void socket_manager::handle_release_socket()
{
	std::vector<socket_base*> sockets;
	{
		auto_lock lock(&m_mutex);
		sockets.insert(sockets.begin(), m_delete_sockets.begin(), m_delete_sockets.end());
		m_delete_sockets.clear();
	}

	for (auto socket : sockets) {
		socket_handler* socket_handler = socket->get_socket_handler();
		m_packet_buffer_pool.deallocate(socket_handler->buffer());
		m_socket_handler_pool.deallocate(socket_handler);

		delete socket->get_packet_handler();
		
		DSafeDelete(socket);
	}
}

void socket_manager::handle_release_packet()
{
	std::vector<TPacketRecvInfo_t*> packets;
	{
		auto_lock lock(&m_mutex);
		packets.insert(packets.begin(), m_finish_read_packets.begin(), m_finish_read_packets.end());
		m_finish_read_packets.clear();
	}
	for (auto packet_info : packets) {
		m_mem_pool.deallocate((char*)packet_info->packet);
		m_packet_info_pool.deallocate(packet_info);
	}
}

void socket_manager::add_socket(socket_base* socket)
{
	TSocketEvent_t& readEvent = socket->get_read_event();
	TSocketEvent_t& writeEvent = socket->get_write_event();
	socket_event_arg_t& eventArg = socket->get_event_arg();
	eventArg.mgr = this;
	eventArg.s = socket;

	if (0 != event_assign(&readEvent, m_eventbase, socket->get_socket_fd(), EV_READ | EV_PERSIST, socket_manager::OnReadEvent, &eventArg)) {

	}
	if (0 != event_assign(&writeEvent, m_eventbase, socket->get_socket_fd(), EV_WRITE, socket_manager::OnWriteEvent, &eventArg)) {

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

	socket_handler* socket_handler = m_socket_handler_pool.allocate();
	socket_handler->set_buffer(m_packet_buffer_pool.allocate());
	socket->set_socket_handler(socket_handler);

	m_sockets[index] = socket;

	{
		auto_lock lock(&m_mutex);
		m_wait_init_sockets.push_back(socket);
	}
}

void socket_manager::del_socket(socket_base* socket)
{
	// 将数据全部写出
	socket->on_write();

	TSocketEvent_t& readEvent = socket->get_read_event();
	event_del(&readEvent);
	TSocketEvent_t& writeEvent = socket->get_write_event();
	event_del(&writeEvent);

	// 关闭链接
	socket->close();

	m_sockets.erase(socket->get_socket_index());

	{
		auto_lock lock(&m_mutex);
		m_wait_delete_sockets.push_back(socket);
	}
}

void socket_manager::send_packet(TSocketIndex_t socket_index, char* msg, uint32 len)
{
	auto itr = m_sockets.find(socket_index);
	if (itr == m_sockets.end()) {
		log_error("send packet failed for socket is invalid, socket index = '%"I64_FMT"u'", socket_index);
		return;
	}
	socket_base* socket = itr->second;
	socket->write(msg, len);
	TSocketEvent_t& writeEvent = socket->get_write_event();
	event_add(&writeEvent, NULL);
}

TSocketIndex_t socket_manager::gen_socket_index()
{
	return ++m_socket_sequence_index;
}

void socket_manager::clean_up()
{
	if (NULL != m_eventbase) {
		event_base_free(m_eventbase);
	}
}

void socket_manager::OnAccept(TSocketFD_t fd, short evt, void* arg)
{
	socket_wrapper_event_arg_t* event_arg = (socket_wrapper_event_arg_t*)arg;
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

void socket_manager::OnWriteEvent(TSocketFD_t fd, short evt, void* arg)
{
	socket_event_arg_t* eventArg = (socket_event_arg_t*)arg;

	if (evt & EV_WRITE) {
		eventArg->mgr->on_write(eventArg->s);
	} else {
		eventArg->mgr->handle_close_socket(eventArg->s, true);
	}
}

void socket_manager::OnReadEvent(TSocketFD_t fd, short evt, void* arg)
{
	socket_event_arg_t* eventArg = (socket_event_arg_t*)arg;

	if (!eventArg->s->is_active()) {
		return;
	}

	if (evt & EV_READ) {
		eventArg->mgr->on_read(eventArg->s);
	} else {
		eventArg->mgr->handle_close_socket(eventArg->s, false);
	}
}