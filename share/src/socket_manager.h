
#ifndef _SOCKET_MANAGER_H_
#define _SOCKET_MANAGER_H_

#include <mutex>
#include <unordered_map>

#include "base_util.h"
#include "socket_util.h"
#include "auto_lock.h"
#include "memory_pool.h"
#include "socket_handler.h"
#include "base_packet.h"
#include "packet_handler.h"

template <class socket_type, class packet_type>
class socket_manager
{
public:
	socket_manager();
	virtual ~socket_manager();

public:
	virtual bool init(TProcessType_t process_type, TProcessID_t process_id);
	virtual void update(uint32 diff);

	uint32 socket_num() const;

	void test_get_sockets(std::vector<socket_type*>& sockets);
	void test_kick();

	void swap_net_2_logic(
		std::vector<packet_type*>& read_packets,
		std::vector<packet_send_info*>& finish_write_packets,
		std::vector<socket_type*>& new_sockets,
		std::vector<socket_type*>& del_sockets
	);

	void swap_login_2_net(
		const std::vector<packet_send_info*>& write_packets,
		const std::vector<packet_type*>& finish_read_packets,
		const std::vector<TSocketIndex_t>& kick_sockets,
		const std::vector<socket_type*>& del_sockets
	);

protected:
	void handle_new_socket();
	void handle_unpacket();
	void handle_socket_unpacket(socket_type* socket);
	void handle_write_msg();
	void handle_kick_socket();
	void handle_close_socket(socket_type* socket, bool write_flag);
	void handle_release_socket();
	void handle_release_packet();

	void send_packet(TSocketIndex_t socket_index, char* msg, uint32 len);

	virtual void unpack_packets(std::vector<packet_type*>& packets, socket_type* socket) = 0;

	virtual void add_socket(socket_type* socket);
	virtual void del_socket(socket_type* socket);
	virtual void on_release_socket(socket_type* socket);
	virtual void on_send_packet(socket_type* socket);

	TSocketIndex_t gen_socket_index();

protected:
	std::mutex m_mutex;
	TSocketIndex_t m_socket_sequence_index;
	memory_allocator<MAX_PACKET_BUFFER_SIZE, 100> m_packet_buffer_pool;
	obj_memory_pool<socket_handler, 100> m_socket_handler_pool;
	obj_memory_pool<packet_type, 1000> m_packet_info_pool;
	memory_pool	m_mem_pool;
	std::vector<packet_type*> m_read_packets;
	std::vector<packet_type*> m_finish_read_packets;
	std::vector<packet_send_info*> m_write_packets;
	std::vector<packet_send_info*> m_finish_write_packets;
	std::vector<socket_type*> m_new_sockets;
	std::vector<socket_type*> m_wait_init_sockets;
	std::vector<TSocketIndex_t> m_wait_kick_sockets;
	std::vector<socket_type*> m_wait_delete_sockets;
	std::vector<socket_type*> m_delete_sockets;
	std::unordered_map<TSocketIndex_t, socket_type*> m_sockets;
};

#endif