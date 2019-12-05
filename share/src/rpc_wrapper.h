
#ifndef _RPC_WRAPPER_H_
#define _RPC_WRAPPER_H_

#include <map>

#include "singleton.h"
#include "packet_struct.h"
#include "server_manager.h"
#include "rpc_client.h"
#include "server_enum.h"
#include "mailbox_manager.h"

class service;

// rpc client wrapper for server
class rpc_wrapper : public singleton<rpc_wrapper>
{
	struct rpc_client_wrapper_info
	{
		TGameTime_t send_time;
		rpc_client* rpc;
		rpc_client_wrapper_info() {
			send_time = INVALID_GAME_TIME;
			rpc = NULL;
		}

		rpc_client_wrapper_info(TGameTime_t _send_time, rpc_client* _rpc) {
			send_time = _send_time;
			rpc = _rpc;
		}
	};

public:
	rpc_wrapper();
	~rpc_wrapper();

public:
	void call_client(const proxy_info& proxy, const std::string& func_name) {
		rpc_client* rpc = get_client_by_process_id(PROCESS_GATE, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_transfer_func(proxy.client_id, func_name);
		}
	}

	template <class... Args>
	void call_client(const proxy_info& proxy, const std::string& func_name, const Args&... args) {
		rpc_client* rpc = get_client_by_process_id(PROCESS_GATE, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_transfer_func(proxy.client_id, func_name, args...);
		}
	}

	void call_ws_client(const proxy_info& proxy, const std::string& msg) {
		rpc_client* rpc = get_client_by_process_id(PROCESS_GATE, proxy.gate_id);
		if (NULL != rpc) {
			rpc->call_ws_client(proxy.client_id, msg);
		}
	}

public:
	// gate transfer client websocket packet to game, gate will parse websocket json packet to byte stream
	void call_server(TSocketIndex_t client_id, const std::string& func_name) {
		rpc_client* rpc = get_client_by_client_id(client_id);
		if (NULL != rpc) {
			rpc->call_transfer_func(client_id, func_name);
		}
	}

	template <class... Args>
	void call_server(TSocketIndex_t client_id, const std::string& func_name, const Args&... args) {
		rpc_client* rpc = get_client_by_client_id(client_id);
		if (NULL != rpc) {
			rpc->call_transfer_func(client_id, func_name, args...);
		}
	}

	// gate transfer client tcp packet to game
	template <class T>
	void transfer_server(TSocketIndex_t client_id, T* packet) {
		rpc_client* rpc = get_client_by_client_id(client_id);
		if (NULL != rpc) {
			rpc->call_transfer_packet(client_id, packet);
		}
	}

public:
	void call_entity(const mailbox_info& mailbox, const std::string& func_name) {
		rpc_client* rpc = get_client_by_address(mailbox.ip, mailbox.port);
		if (NULL != rpc) {
			rpc->call_entity(mailbox.entity_id, func_name);
		}
	}

	template <class... Args>
	void call_entity(const mailbox_info& mailbox, const std::string& func_name, const Args&... args) {
		rpc_client* rpc = get_client_by_address(mailbox.ip, mailbox.port);
		if (NULL != rpc) {
			rpc->call_entity(mailbox.entity_id, func_name, args...);
		}
	}

public:
	void call_stub(const std::string& stub_name, const std::string& func_name) {
		mailbox_info mailbox;
		if (DMailboxMgr.get_mailbox_by_entity(mailbox, stub_name)) {
			call_entity(mailbox, func_name);
		}
	}

	template <class... Args>
	void call_stub(const std::string& stub_name, const std::string& func_name, const Args&... args) {
		mailbox_info mailbox;
		if (DMailboxMgr.get_mailbox_by_entity(mailbox, stub_name)) {
			call_entity(mailbox, func_name, args...);
		}
	}

public:
	void init(service* s);

public:
	void register_handler_info(game_handler* handler);
	void update_handler_info(TSocketIndex_t socket_index, const game_server_info& server_info);
	void unregister_handler_info(TSocketIndex_t socket_index);

public:
	TSocketIndex_t get_socket_index(const game_process_info& process_info) const;
	rpc_client* get_client_by_socket_index(TSocketIndex_t socket_index) const;
	rpc_client* get_client_by_process_type(TProcessType_t process_type) const;
	rpc_client* get_client_by_process_id(TProcessType_t process_type, TProcessID_t process_id) const;
	rpc_client* get_client_by_client_id(TSocketIndex_t client_id) const;
	rpc_client* get_client_by_address(const TIP_t& ip, TPort_t port);
	TProcessID_t get_random_process_id(TProcessType_t process_type) const;
	uint32 get_key_id_by_process_id(TProcessType_t process_type, TProcessID_t process_id) const;
	void parse_key_id_by_process_id(game_process_info& process_info, uint64 key_id) const;
	void parse_key_id_by_process_type(game_process_info& process_info, uint64 key_id) const;

private:
	service* m_service;
	std::map<uint32, rpc_client*> m_process_id_2_client;
	std::map<std::string, rpc_client_wrapper_info*> m_directly_games;	// key: ip + port
	std::map<TSocketIndex_t, rpc_client*> m_socket_index_2_client;
};

#define DRpcWrapper singleton<rpc_wrapper>::get_instance()

#endif // !_RPC_WRAPPER_H_
