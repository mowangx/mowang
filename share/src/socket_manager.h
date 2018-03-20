
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

class CSocketManager : public CSingleton<CSocketManager>
{
public:
	CSocketManager();
	~CSocketManager();

public:
	bool	init();
	void	update(uint32 diff);

	template <class T>
	bool	start_listen(TPort_t port);

	template <class T>
	bool	start_connect(const char* host, TPort_t port);

	uint32	socket_num() const;

	void	read_packets(std::vector<TPacketInfo_t*>& packets);
	void	finish_read_packets(std::vector<TPacketInfo_t*>& packets);

	void	write_packets(std::vector<TPacketInfo_t*>& packets);
	void	finish_write_packets(std::vector<TPacketInfo_t*>& packets);

	void	test_get_sockets(std::vector<CSocket*>& sockets);

private:
	bool	on_accept(CSocketWrapper* listener);

	void	on_write(CSocket* socket);
	void	on_read(CSocket* socket);

	void	handle_new_socket();
	void	handle_unpacket();
	void	handle_socket_unpacket(CSocket* socket);
	void	handle_write_msg();
	void	handle_close_socket(CSocket* socket, bool writeFlag);
	void	handle_release_packet();

	void	add_socket(CSocket* socket);
	void	del_socket(CSocket* socket);

	void	send_packet(CSocket* socket, char* msg, uint32 len);

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
	CMemoryAllocator<MAX_PACKET_BUFFER_SIZE, 100> m_packet_buffer_pool;
	CObjMemoryPool<CSocketHandler, 100> m_socket_handler_pool;
	CObjMemoryPool<TPacketInfo_t, 1000> m_packet_info_pool;
	CMemoryPool	m_mem_pool;
	CMsgQueue<CSocket*> m_new_socket_queue;
	std::unordered_map<TSocketIndex_t, CSocket*> m_sockets;
	std::vector<TPacketInfo_t*> m_read_packets;
	std::vector<TPacketInfo_t*> m_finish_read_packets;
	std::vector<TPacketInfo_t*> m_write_packets;
	std::vector<TPacketInfo_t*> m_finish_write_packets;
};

template <class T>
bool CSocketManager::start_listen(TPort_t port)
{
	CSocketWrapper * socket = new CSocketListener<T>("any", port);
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
	TSocketWrapperEventArg_t& event_arg = socket->get_wrapper_event_arg();
	event_arg.s = socket;
	event_arg.mgr = this;
	if (0 != event_assign(&listen_event, m_eventbase, socket->get_socket_fd(), EV_READ | EV_PERSIST,
		CSocketManager::OnAccept, &event_arg)) {
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
bool CSocketManager::start_connect(const char* host, TPort_t port)
{
	CSocketWrapper* socket = new CSocketConnector<T>();
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
	socket->get_packet_handler()->set_socket(socket);
	socket->get_packet_handler()->set_index(index);

	add_socket(socket);

	return true;
}

#define DNetMgr		CSocketManager::getInstance()

#endif