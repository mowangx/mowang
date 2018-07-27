
#ifndef _TCP_MANAGER_H_
#define _TCP_MANAGER_H_

#include "singleton.h"
#include "socket_manager.hpp"
#include "socket_wrapper.h"

class tcp_manager : public socket_manager<socket_base, packet_recv_info>, public singleton<tcp_manager>
{
	typedef socket_manager<socket_base, packet_recv_info> TBaseType_t;
public:
	tcp_manager();
	virtual ~tcp_manager();

public:
	virtual bool init(TProcessType_t process_type, TProcessID_t process_id) override;
	virtual void update(uint32 diff) override;

public:
	template <class T>
	bool start_listen(TPort_t port);

	template <class T>
	bool start_connect(const char* host, TPort_t port);

private:
	bool on_accept(socket_wrapper* listener);

	void on_write(socket_base* socket);
	void on_read(socket_base* socket);

	virtual void unpack_packets(std::vector<packet_recv_info*>& packets, socket_base* socket) override;
	virtual void add_socket(socket_base* socket) override;
	virtual void del_socket(socket_base* socket) override;
	virtual void on_release_socket(socket_base* socket) override;
	virtual void on_send_packet(socket_base* socket) override;

private:
	static void OnAccept(TSocketFD_t fd, short evt, void* arg);
	static void OnWriteEvent(TSocketFD_t fd, short evt, void* arg);
	static void OnReadEvent(TSocketFD_t fd, short evt, void* arg);

private:
	SocketEventBase_t* m_eventbase;
};

template <class T>
bool tcp_manager::start_listen(TPort_t port)
{
	socket_wrapper * socket = new socket_listener<T>("any", port);
	if (NULL == socket) {
		return false;
	}

	if (!socket->create()) {
		DSafeDelete(socket);
		return false;
	}

	if (!socket->set_reuse_addr()) {
		DSafeDelete(socket);
		return false;
	}

	if (!socket->bind()) {
		DSafeDelete(socket);
		return false;
	}

	if (!socket->listen(5)) {
		DSafeDelete(socket);
		return false;
	}

	socket->set_reuse_addr(true);
	socket->set_linger(0);
	socket->set_non_blocking(true);

	TSocketEvent_t& listen_event = socket->get_read_event();
	socket_wrapper_event_arg_t& event_arg = socket->get_wrapper_event_arg();
	event_arg.s = socket;
	event_arg.mgr = this;
	if (0 != event_assign(&listen_event, m_eventbase, socket->get_socket_fd(), EV_READ | EV_PERSIST,
		tcp_manager::OnAccept, &event_arg)) {
		log_warning("can't event assign!");
		DSafeDelete(socket);
		return false;
	}

	if (0 != event_add(&listen_event, NULL)) {
		log_warning("can't event add!errno=%s", strerror(errno));
		DSafeDelete(socket);
		return false;
	}

	return true;
}


template <class T>
bool tcp_manager::start_connect(const char* host, TPort_t port)
{
	socket_wrapper* socket = new socket_connector<T>();
	if (NULL == socket) {
		return false;
	}

	if (!socket->create()) {
		DSafeDelete(socket);
		return false;
	}

	if (!socket->connect(host, port, 0)) {
		DSafeDelete(socket);
		return false;
	}

	socket->set_non_blocking(true);

	TSocketIndex_t index = gen_socket_index();
	socket->set_socket_index(index);
	log_info("connect socket success! index = '%"I64_FMT"u', host = %s, port = %u", index, host, port);

	socket->set_packet_handler(socket->create_handler());
	socket->get_packet_handler()->set_socket_index(index);

	add_socket(socket);

	return true;
}

#define DNetMgr		tcp_manager::get_instance()

#endif // !_TCP_MANAGER_H_

