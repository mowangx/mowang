
#ifndef _WS_MANAGER_H_
#define _WS_MANAGER_H_

#include <mutex>
#include <map>
#include <unordered_map>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "singleton.h"
#include "memory_pool.h"
#include "socket_util.h"
#include "base_packet.h"
#include "socket_handler.h"

typedef websocketpp::server<websocketpp::config::asio> WebsocketServer;
typedef WebsocketServer::message_ptr message_ptr;

struct ws_socket_info {
	TSocketIndex_t client_id;
	socket_handler* handler;
	ws_socket_info() {

	}

	ws_socket_info(TSocketIndex_t s, socket_handler* h) {
		client_id = s;
		handler = h;
	}
};

class WebSocketManager : public singleton<WebSocketManager>
{
public:
	WebSocketManager();
	~WebSocketManager();

public:
	void start_listen(TPort_t port);

public:
	bool init(TProcessType_t process_type, TProcessID_t process_id);
	void update(uint32 diff);

	void test_kick();

	uint32 socket_num() const;

	void	swap_net_2_logic(
		std::vector<ws_packet_recv_info*>& read_packets,
		std::vector<packet_send_info*>& finish_write_packets,
		std::vector<TSocketIndex_t>& new_sockets,
		std::vector<TSocketIndex_t>& del_sockets
	);

	void	swap_login_2_net(
		const std::vector<packet_send_info*>& write_packets,
		const std::vector<ws_packet_recv_info*>& finish_read_packets,
		const std::vector<TSocketIndex_t>& kick_sockets,
		const std::vector<TSocketIndex_t>& del_sockets
	);

private:
	void on_accept(websocketpp::connection_hdl hdl);
	void on_close(websocketpp::connection_hdl hdl);
	void on_message(websocketpp::connection_hdl hdl, message_ptr msg);

private:
	void handle_new_socket();
	void handle_write_msg();
	void handle_kick_socket();
	void handle_release_socket();
	void handle_release_packet();

private:
	void add_socket(websocketpp::connection_hdl hdl);
	void del_socket(websocketpp::connection_hdl hdl);

	void send_packet(TSocketIndex_t socket_index, char* msg, uint32 len);

private:
	TSocketIndex_t gen_socket_index();

private:
	void clean_up();

private:
	std::mutex m_mutex;
	WebsocketServer m_server;
	TSocketIndex_t m_socket_sequence_index;
	memory_pool	m_mem_pool;
	memory_allocator<MAX_PACKET_BUFFER_SIZE, 100> m_packet_buffer_pool;
	obj_memory_pool<socket_handler, 100> m_socket_handler_pool;
	obj_memory_pool<ws_packet_recv_info, 1000> m_packet_info_pool;
	std::vector<ws_packet_recv_info*> m_read_packets;
	std::vector<ws_packet_recv_info*> m_finish_read_packets;
	std::vector<packet_send_info*> m_write_packets;
	std::vector<packet_send_info*> m_finish_write_packets;
	std::vector<websocketpp::connection_hdl> m_new_sockets;
	std::vector<TSocketIndex_t> m_wait_init_sockets;
	std::vector<TSocketIndex_t> m_wait_kick_sockets;
	std::vector<TSocketIndex_t> m_wait_delete_sockets;
	std::vector<TSocketIndex_t> m_delete_sockets;
	std::map<TSocketIndex_t, websocketpp::connection_hdl> m_index_2_handle_map;
	std::map<websocketpp::connection_hdl, ws_socket_info, std::owner_less<websocketpp::connection_hdl>> m_handle_2_info_map;
};

#define DWSNetMgr WebSocketManager::get_instance()

#endif // !_WS_MANAGER_H_

