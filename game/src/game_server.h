
#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include "singleton.h"
#include "service.h"
#include "dynamic_array.h"
#include "resource.h"
#include "farmland.h"
#include "npc.h"
#include "city.h"
#include "role.h"

class rpc_client;

class game_server : public service, public singleton<game_server>
{
	typedef service TBaseType_t;

public:
	game_server();
	~game_server();

public:
	virtual bool init(TProcessID_t process_id) override;

public:
	resource * allocate_resource();
	void deallocate_resource(resource* res);

	city* allocate_city();
	void deallocate_city(city* c);

	npc* allocate_npc();
	void deallocate_npc(npc* p);

	farmland* allocate_farmland();
	void deallocate_farmland(farmland* f);

public:
	void register_server(TSocketIndex_t socket_index, const game_server_info& server_info);
	void login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, TProcessID_t gate_id, TPlatformID_t platform_id, TUserID_t user_id);
	void game_rpc_func(TSocketIndex_t socket_index, TServerID_t server_id);
	void game_rpc_func_1(TSocketIndex_t socket_index, const dynamic_string& p1, uint16 p2, const std::array<char, 127>& p3);
	void game_rpc_func_2(TSocketIndex_t socket_index, uint8 p1, const std::array<char, 33>& p2);
	void on_register_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);

public:
	void transfer_client(TSocketIndex_t client_id, packet_base* packet);
	void create_entity(uint8 e_type);

private:
	TRoleID_t get_role_id_by_client_id(TSocketIndex_t client_id) const;

private:
	obj_memory_pool<resource, 65536> m_resource_pool;
	obj_memory_pool<city, 1024> m_city_pool;
	obj_memory_pool<npc, 1024> m_npc_pool;
	obj_memory_pool<farmland, 1024> m_farmland_pool;
	std::unordered_map<TSocketIndex_t, TRoleID_t> m_client_id_2_role_id;
	std::vector<role*> m_roles;
};

#define DGameServer singleton<game_server>::get_instance()

#endif