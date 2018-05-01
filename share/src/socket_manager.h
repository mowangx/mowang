
#ifndef _SOCKET_MANAGER_H_
#define _SOCKET_MANAGER_H_

#include <mutex>
#include <unordered_map>

#include "singleton.h"
#include "base_util.h"
#include "socket_util.h"
#include "msg_queue.h"
#include "memory_pool.h"
#include "socket_handler.h"
#include "base_packet.h"
#include "socket_wrapper.h"
#include "packet_handler.h"

class socket_manager : public singleton<socket_manager>
{
public:
	socket_manager();
	~socket_manager();

public:
	bool	init();
	void	update(uint32 diff);

	void	test_kick();

	template <class T>
	bool	start_listen(TPort_t port);

	template <class T>
	bool	start_connect(const char* host, TPort_t port);

	uint32	socket_num() const;


	void	swap_net_2_logic(
		std::vector<TPacketRecvInfo_t*>& read_packets, 
		std::vector<TPacketSendInfo_t*>& finish_write_packets, 
		std::vector<socket_base*>& new_sockets, 
		std::vector<socket_base*>& del_sockets
	);

	void	swap_login_2_net(
		const std::vector<TPacketSendInfo_t*>& write_packets, 
		const std::vector<TPacketRecvInfo_t*>& finish_read_packets, 
		const std::vector<TSocketIndex_t>& kick_sockets,
		const std::vector<socket_base*>& del_sockets
	);

	void	test_get_sockets(std::vector<socket_base*>& sockets);

private:
	bool	on_accept(socket_wrapper* listener);

	void	on_write(socket_base* socket);
	void	on_read(socket_base* socket);

	void	handle_new_socket();
	void	handle_unpacket();
	void	handle_socket_unpacket(socket_base* socket);
	void	handle_write_msg();
	void	handle_kick_socket();
	void	handle_close_socket(socket_base* socket, bool write_flag);
	void	handle_release_socket();
	void	handle_release_packet();

	void	add_socket(socket_base* socket);
	void	del_socket(socket_base* socket);

	void	send_packet(TSocketIndex_t socket_index, char* msg, uint32 len);

	TSocketIndex_t gen_socket_index();

	void	clean_up();

private:
	static void OnAccept(TSocketFD_t fd, short evt, void* arg);
	static void OnWriteEvent(TSocketFD_t fd, short evt, void* arg);
	static void OnReadEvent(TSocketFD_t fd, short evt, void* arg);

private:
	std::mutex m_mutex;
	TSocketIndex_t m_socket_sequence_index;
	SocketEventBase_t* m_eventbase;
	memory_allocator<MAX_PACKET_BUFFER_SIZE, 100> m_packet_buffer_pool;
	obj_memory_pool<socket_handler, 100> m_socket_handler_pool;
	obj_memory_pool<TPacketRecvInfo_t, 1000> m_packet_info_pool;
	memory_pool	m_mem_pool;
	std::vector<TPacketRecvInfo_t*> m_read_packets;
	std::vector<TPacketRecvInfo_t*> m_finish_read_packets;
	std::vector<TPacketSendInfo_t*> m_write_packets;
	std::vector<TPacketSendInfo_t*> m_finish_write_packets;
	std::vector<socket_base*> m_new_sockets;
	std::vector<socket_base*> m_wait_init_sockets;
	std::vector<TSocketIndex_t> m_wait_kick_sockets;
	std::vector<socket_base*> m_wait_delete_sockets;
	std::vector<socket_base*> m_delete_sockets;
	std::unordered_map<TSocketIndex_t, socket_base*> m_sockets;
};

template <class T>
bool socket_manager::start_listen(TPort_t port)
{
	socket_wrapper * socket = new socket_listener<T>("any", port);
	if (NULL == socket) {
		return false;
	}

	if (!socket->create()) {
		return false;
	}

	if (!socket->set_reuse_addr()) {
		return false;
	}

	if (!socket->bind()) {
		return false;
	}

	if (!socket->listen(5)) {
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
		socket_manager::OnAccept, &event_arg)) {
		log_warning("can't event assign!");
		return false;
	}

	if (0 != event_add(&listen_event, NULL)) {
		log_warning("can't event add!errno=%s", strerror(errno));
		return false;
	}

	return true;
}


template <class T>
bool socket_manager::start_connect(const char* host, TPort_t port)
{
	socket_wrapper* socket = new socket_connector<T>();
	if (NULL == socket) {
		return false;
	}

	if (!socket->create()) {
		return false;
	}

	if (!socket->connect(host, port, 0)) {
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

#define DNetMgr		socket_manager::get_instance()

#endif