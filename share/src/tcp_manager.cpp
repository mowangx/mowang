
#include "tcp_manager.h"

#include "log.h"
#include "socket.h"
#include "socket_api.h"

tcp_manager::tcp_manager()
{
	m_eventbase = nullptr;
}

tcp_manager::~tcp_manager()
{
	SOCKET_API::gx_lib_cleanup();
	if (nullptr != m_eventbase) {
		event_base_free(m_eventbase);
	}
}

bool tcp_manager::init(TProcessType_t process_type, TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_type, process_id)) {
		return false;
	}

	if (!SOCKET_API::gx_lib_init()) {
		return false;
	}

	m_eventbase = event_base_new();
	if (nullptr == m_eventbase) {
		return false;
	}
	return true;
}

void tcp_manager::update(uint32 diff)
{
	if (-1 == event_base_loop(m_eventbase, EVLOOP_ONCE | EVLOOP_NONBLOCK)) {
		log_info("update ret -1");
		return;
	}

	TBaseType_t::update(diff);
}

bool tcp_manager::on_accept(socket_wrapper* listener)
{
	socket_base* socket = listener->accept();
	if (NULL == socket) {
		return false;
	}

	// @todo 检测添加的时候会不会影响性能
	TSocketIndex_t index = gen_socket_index();
	socket->set_socket_index(index);
	log_info("accept socket! index %" I64_FMT "u", index);

	socket->set_packet_handler(listener->create_handler());
	socket->get_packet_handler()->set_socket_index(index);

	m_new_sockets.push_back(socket);

	return true;
}

void tcp_manager::on_write(socket_base* socket)
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

void tcp_manager::on_read(socket_base* socket)
{
	if (!socket->is_active()) {
		return;
	}

	if (!socket->on_read()) {
		handle_close_socket(socket, false);
		return;
	}
}

void tcp_manager::unpack_packets(std::vector<packet_recv_info*>& packets, socket_base* socket)
{
	int len = socket->get_input_len();
	if (len > MAX_PACKET_READ_SIZE) {
		len = MAX_PACKET_READ_SIZE;
	}
	socket_handler* handler = socket->get_socket_handler();
	int cur_len = socket->read(handler->buffer(len), len);
	if (cur_len != len) {
		log_error("socket index %" I64_FMT "u, read len is not equal cache len, read len %d, cache le %d", socket->get_socket_index(), cur_len, len);
	}

	packet_base* packet = handler->unpack_packet();
	while (NULL != packet) {
		char* packet_pool = m_mem_pool.allocate(packet->get_packet_len());
		memcpy(packet_pool, packet, packet->get_packet_len());
		packet_recv_info* packet_info = m_packet_info_pool.allocate();
		packet_info->socket = socket;
		packet_info->packet = (packet_base*)packet_pool;
		packets.push_back(packet_info);
		packet = handler->unpack_packet();
	}
}

void tcp_manager::add_socket(socket_base* socket)
{
	TSocketEvent_t& readEvent = socket->get_read_event();
	TSocketEvent_t& writeEvent = socket->get_write_event();
	socket_event_arg_t& eventArg = socket->get_event_arg();
	eventArg.mgr = this;
	eventArg.s = socket;

	if (0 != event_assign(&readEvent, m_eventbase, socket->get_socket_fd(), EV_READ | EV_PERSIST, tcp_manager::OnReadEvent, &eventArg)) {

	}
	if (0 != event_assign(&writeEvent, m_eventbase, socket->get_socket_fd(), EV_WRITE, tcp_manager::OnWriteEvent, &eventArg)) {

	}

	if (0 != event_add(&readEvent, NULL)) {
	}

	if (0 != event_add(&writeEvent, NULL)) {
	}

	TBaseType_t::add_socket(socket);
}

void tcp_manager::del_socket(socket_base* socket)
{
	// 将数据全部写出
	socket->on_write();

	TSocketEvent_t& readEvent = socket->get_read_event();
	event_del(&readEvent);
	TSocketEvent_t& writeEvent = socket->get_write_event();
	event_del(&writeEvent);

	// 关闭链接
	socket->close();
	TBaseType_t::del_socket(socket);
}

void tcp_manager::on_release_socket(socket_base * socket)
{
	delete socket->get_packet_handler();
}

void tcp_manager::on_release_packets(std::vector<packet_recv_info*>& packets)
{
	for (auto packet_info : packets) {
		m_mem_pool.deallocate((char*)packet_info->packet);
		m_packet_info_pool.deallocate(packet_info);
	}
}

void tcp_manager::on_send_packet(socket_base * socket)
{
	TSocketEvent_t& writeEvent = socket->get_write_event();
	event_add(&writeEvent, NULL);
}

void tcp_manager::OnAccept(TSocketFD_t fd, short evt, void* arg)
{
	socket_wrapper_event_arg_t* event_arg = (socket_wrapper_event_arg_t*)arg;
	if (NULL == event_arg || NULL == event_arg->s || NULL == event_arg->mgr) {
		log_error("cast socket arg failed");
		return;
	}
	for (uint32 i = 0; i < ACCEPT_ONCE_NUM; ++i) {
		if (!event_arg->mgr->on_accept(event_arg->s)) {
			break;
		}
	}
}

void tcp_manager::OnWriteEvent(TSocketFD_t fd, short evt, void* arg)
{
	socket_event_arg_t* eventArg = (socket_event_arg_t*)arg;

	if (evt & EV_WRITE) {
		eventArg->mgr->on_write(eventArg->s);
	}
	else {
		eventArg->mgr->handle_close_socket(eventArg->s, true);
	}
}

void tcp_manager::OnReadEvent(TSocketFD_t fd, short evt, void* arg)
{
	socket_event_arg_t* eventArg = (socket_event_arg_t*)arg;

	if (!eventArg->s->is_active()) {
		return;
	}

	if (evt & EV_READ) {
		eventArg->mgr->on_read(eventArg->s);
	}
	else {
		eventArg->mgr->handle_close_socket(eventArg->s, false);
	}
}
