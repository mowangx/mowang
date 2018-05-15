
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
private:
	virtual void net_run(TProcessID_t process_id) override;
	virtual void do_loop(TGameTime_t diff) override;
	virtual bool connect_game_manager(const char* ip, TPort_t port) override;

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
	void db_remove(const char* table, const char* query, const std::function<void(bool)>& callback);
	void db_insert(const char* table, const char* fields, const std::function<void(bool)>& callback);
	void db_update(const char* table, const char* query, const char* fields, const std::function<void(bool)>& callback);
	void db_query(const char* table, const char* query, const char* fields, const std::function<void(bool, const dynamic_string_array&)>& callback);
private:
	void db_opt_with_status(uint8 opt_type, const char* table, const char* query, const char* fields, const std::function<void(bool)>& callback);
	void db_opt_with_result(uint8 opt_type, const char* table, const char* query, const char* fields, const std::function<void(bool, const dynamic_string_array&)>& callback);
	void db_opt(uint8 opt_type, const char* table, const char* query, const char* fields);

public:
	void login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, TPlatformID_t platform_id, TUserID_t user_id, TSocketIndex_t test_client_id);
	void on_register_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);
	void create_entity(TSocketIndex_t socket_index, const dynamic_string& stub_name);
	void on_opt_db_with_status(TSocketIndex_t socket_index, TDbOptID_t opt_id, bool status);
	void on_opt_db_with_result(TSocketIndex_t socket_index, TDbOptID_t opt_id, bool status, const dynamic_string_array& data);
private:
	virtual void on_connect(TSocketIndex_t socket_index) override;

public:
	void transfer_client(TSocketIndex_t client_id, packet_base* packet);
	void create_entity_globally(const dynamic_string& stub_name);
	void create_entity_locally(const dynamic_string& stub_name);

private:
	TRoleID_t get_role_id_by_client_id(TSocketIndex_t client_id) const;

private:
	TDbOptID_t m_db_opt_id;
	std::unordered_map<TDbOptID_t, std::function<void(bool, const dynamic_string_array&)>> m_db_result_callbacks;
	std::unordered_map<TDbOptID_t, std::function<void(bool)>> m_db_status_callbacks;
	obj_memory_pool<resource, 65536> m_resource_pool;
	obj_memory_pool<city, 1024> m_city_pool;
	obj_memory_pool<npc, 1024> m_npc_pool;
	obj_memory_pool<farmland, 1024> m_farmland_pool;
	std::unordered_map<TSocketIndex_t, TRoleID_t> m_client_id_2_role_id;
	std::vector<role*> m_roles;
};

#define DGameServer singleton<game_server>::get_instance()

#endif