
#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include <vector>
#include <map>

#include "socket_util.h"
#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"
#include "dynamic_array.h"
#include "server_manager.h"

class rpc_client;

class gate_server : public singleton<gate_server>
{
public:
	gate_server();
	~gate_server();

public:
	bool init(TProcessID_t process_id);
	void run();

public:
	const game_server_info& get_server_info() const;

public:
	TPacketSendInfo_t * allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketSendInfo_t* packet_info);

public:
	void register_client(rpc_client* client);

public:
	void register_server(TSocketIndex_t socket_index, const game_server_info& server_info);
	void on_query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);
	void login_server(TSocketIndex_t socket_index, TPlatformID_t platform_id, TServerID_t server_id, const TUserID_t& user_id);

public:
	void transfer_role(TServerID_t server_id, TProcessID_t game_id, TRoleID_t role_id, packet_base* packet);
	void transfer_stub(TServerID_t server_id, TProcessID_t game_id, packet_base* packet);
	void transfer_client(TSocketIndex_t client_id, packet_base* packet);
	void transfer_server(TSocketIndex_t client_id, packet_base* packet);

private:
	TSocketIndex_t get_server_socket_index(TSocketIndex_t client_id) const;

private:
	game_server_info m_server_info;
	obj_memory_pool<TPacketSendInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketSendInfo_t*> m_write_packets;
	std::unordered_map<TSocketIndex_t, game_process_info> m_client_2_process;
	std::map<TSocketIndex_t, rpc_client*> m_clients;
};

#define DGateServer singleton<gate_server>::get_instance()

#endif