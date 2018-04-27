
#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include <vector>
#include <map>

#include "socket_util.h"
#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"
#include "dynamic_array.h"
#include "resource.h"
#include "farmland.h"
#include "npc.h"
#include "city.h"
#include "role.h"
#include "server_manager.h"

class rpc_client;

class game_server : public singleton<game_server>
{
public:
	game_server();
	~game_server();

public:
	bool init(TProcessID_t process_id);
	void run();

public:
	const game_server_info& get_server_info() const;

public:
	TPacketSendInfo_t * allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketSendInfo_t* packet_info);

	resource * allocate_resource();
	void deallocate_resource(resource* res);

	city* allocate_city();
	void deallocate_city(city* c);

	npc* allocate_npc();
	void deallocate_npc(npc* p);

	farmland* allocate_farmland();
	void deallocate_farmland(farmland* f);

public:
	void register_client(rpc_client* client);

public:
	void register_server(TSocketIndex_t socket_index, const game_server_info& server_info);
	void login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, TProcessID_t gate_id, TPlatformID_t platform_id, TUserID_t user_id);
	void game_rpc_func(TSocketIndex_t socket_index, TServerID_t server_id);
	void game_rpc_func_1(TSocketIndex_t socket_index, const dynamic_string& p1, uint16 p2, const std::array<char, 127>& p3);
	void game_rpc_func_2(TSocketIndex_t socket_index, uint8 p1, const std::array<char, 33>& p2);
	void on_query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);

public:
	void transfer_client(TSocketIndex_t client_id, packet_base* packet);
	void create_entity(uint8 e_type);

private:
	TRoleID_t get_role_id_by_client_id(TSocketIndex_t client_id) const;

private:
	game_server_info m_server_info;
	obj_memory_pool<TPacketSendInfo_t, 1000> m_packet_pool;
	obj_memory_pool<resource, 65536> m_resource_pool;
	obj_memory_pool<city, 1024> m_city_pool;
	obj_memory_pool<npc, 1024> m_npc_pool;
	obj_memory_pool<farmland, 1024> m_farmland_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketSendInfo_t*> m_write_packets;
	std::unordered_map<TSocketIndex_t, TRoleID_t> m_client_id_2_role_id;
	std::map<TSocketIndex_t, rpc_client*> m_clients;
	std::vector<role*> m_roles;
};

#define DGameServer singleton<game_server>::get_instance()

#endif