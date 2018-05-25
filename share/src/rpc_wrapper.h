
#ifndef _RPC_WRAPPER_H_
#define _RPC_WRAPPER_H_

#include <map>

#include "singleton.h"
#include "packet_struct.h"
#include "server_manager.h"
#include "rpc_client.h"
#include "game_enum.h"

// rpc client wrapper for server
class rpc_wrapper : public singleton<rpc_wrapper>
{
	struct rpc_client_wrapper_info
	{
		TProcessID_t process_id;
		rpc_client* rpc;
		rpc_client_wrapper_info() {
			process_id = INVALID_PROCESS_ID;
			rpc = NULL;
		}

		rpc_client_wrapper_info(TProcessID_t _process_id, rpc_client* _rpc) {
			process_id = _process_id;
			rpc = _rpc;
		}
	};

public:
	rpc_wrapper();
	~rpc_wrapper();

public:
	void call_client(const proxy_info& proxy, const std::string& func_name) {
		rpc_client* rpc = get_client(game_process_info(proxy.server_id, PROCESS_GATE, proxy.gate_id));
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name);
		}
	}

	template <class... Args>
	void call_client(const proxy_info& proxy, const std::string& func_name, const Args&... args) {
		rpc_client* rpc = get_client(game_process_info(proxy.server_id, PROCESS_GATE, proxy.gate_id));
		if (NULL != rpc) {
			rpc->call_client(proxy.client_id, func_name, args...);
		}
	}

public:
	void call_role(const mailbox_info& mailbox, const std::string& func_name) {
		rpc_client* rpc = get_random_client(mailbox.server_id, PROCESS_GATE);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.entity_id, func_name);
		}
	}

	template <class... Args>
	void call_role(const mailbox_info& mailbox, const std::string& func_name, const Args&... args) {
		rpc_client* rpc = get_random_client(mailbox.server_id, PROCESS_GATE);
		if (NULL != rpc) {
			rpc->call_role(mailbox.server_id, mailbox.game_id, mailbox.entity_id, func_name, args...);
		}
	}

public:
	void call_stub(const std::string& class_name, const std::string& func_name) {
		game_process_info process_info;
		rpc_client* rpc = get_stub_client_and_prcoess_info(class_name, process_info);
		if (NULL != rpc) {
			rpc->call_stub(process_info.server_id, process_info.process_id, class_name, func_name);
		}
	}

	template <class... Args>
	void call_stub(const std::string& class_name, const std::string& func_name, const Args&... args) {
		game_process_info process_info;
		rpc_client* rpc = get_stub_client_and_prcoess_info(class_name, process_info);
		if (NULL != rpc) {
			rpc->call_stub(process_info.server_id, process_info.process_id, class_name, func_name, args...);
		}
	}

public:
	void register_handler_info(rpc_client* client, const game_server_info& server_info);
	void unregister_handler_info(TSocketIndex_t socket_index);
	void register_stub_info(const std::string& stub_name, const game_process_info& process_info);

public:
	bool get_server_info(const game_process_info& process_info, game_server_info& server_info) const;
	void get_server_infos(TServerID_t server_id, TProcessType_t process_type, dynamic_array<game_server_info>& servers) const;
	void get_stub_infos(dynamic_array<game_stub_info>& stub_infos) const;;
	bool get_server_simple_info_by_socket_index(game_process_info& process_info, TSocketIndex_t socket_index) const;
	TSocketIndex_t get_socket_index(const game_process_info& process_info) const;
	TProcessID_t get_random_process_id(TServerID_t server_id, TProcessType_t process_type) const;
	rpc_client* get_client(const game_process_info& process_info) const;
	rpc_client* get_random_client(TServerID_t server_id, TProcessType_t process_type) const;
	rpc_client* get_stub_client_and_prcoess_info(const std::string& stub_name, game_process_info& process_info);
	uint64 get_key_id_by_process_id(const game_process_info& process_info) const;
	uint64 get_key_id_by_process_type(TServerID_t server_id, TProcessType_t process_type) const;
	void parse_key_id_by_process_id(game_process_info& process_info, uint64 key_id) const;
	void parse_key_id_by_process_type(game_process_info& process_info, uint64 key_id) const;

private:
	std::map<uint64, rpc_client*> m_server_process_id_2_clients;
	std::map<uint64, std::vector<rpc_client_wrapper_info*>> m_server_process_type_2_clients;
	std::map<std::string, game_process_info> m_stub_name_2_process_infos;
	server_manager m_server_manager;
};

#define DRpcWrapper singleton<rpc_wrapper>::get_instance()

#endif // !_RPC_WRAPPER_H_
