
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
	virtual ~game_server() override;

public:
	virtual bool init(TProcessID_t process_id) override;
private:
	virtual void work_run() override;
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

	soldier_training_info* allocate_soldier_training();
	void deallocate_soldier_training(soldier_training_info* s);

	resource_up_info* allocate_resource_up_info();
	void deallocate_resource_up_info(resource_up_info* r);

public:
	void db_remove(const char* table, const char* query, const std::function<void(bool)>& callback);
	void db_insert(const char* table, const char* fields, const std::function<void(bool)>& callback);
	void db_update(const char* table, const char* query, const char* fields, const std::function<void(bool)>& callback);
	void db_query(const char* table, const char* query, const char* fields, const std::function<void(bool, const binary_data&)>& callback);
private:
	void db_opt_with_status(uint8 opt_type, const char* table, const char* query, const char* fields, const std::function<void(bool)>& callback);
	void db_opt_with_result(uint8 opt_type, const char* table, const char* query, const char* fields, const std::function<void(bool, const binary_data&)>& callback);
	void db_opt(uint8 opt_type, const char* table, const char* query, const char* fields);

public:
	void http_request(const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl, const std::function<void(int, const dynamic_string&)>& callback);

public:
	void login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, TPlatformID_t platform_id, const account_info& account);
	void logout_server(TSocketIndex_t socket_index, TSocketIndex_t client_id);
	virtual void register_server(TSocketIndex_t socket_index, const game_server_info& server_info) override;
	void on_register_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);
	void create_entity(TSocketIndex_t socket_index, const TEntityName_t& entity_name);
	void remove_entity(TSocketIndex_t client_id);
	void on_opt_db_with_status(TSocketIndex_t socket_index, TOptID_t opt_id, bool status);
	void on_opt_db_with_result(TSocketIndex_t socket_index, TOptID_t opt_id, bool status, const binary_data& result);
	void on_http_response(TSocketIndex_t socket_index, TOptID_t opt_id, int status, const dynamic_string& result);
private:
	virtual void on_connect(TSocketIndex_t socket_index) override;
	virtual void on_disconnect(TSocketIndex_t socket_index) override;
	bool remove_entity_core(TSocketIndex_t client_id);

public:
	void transfer_client(TSocketIndex_t client_id, packet_base* packet);
	void create_entity_globally(const std::string& entity_name);
	entity* create_entity_locally(const std::string& entity_name);
	void destroy_entity(TEntityID_t entity_id);

public:
	role* get_role_by_client_id(TSocketIndex_t client_id) const;
	void update_role_proxy_info(const proxy_info& old_proxy_info, const proxy_info& new_proxy_info);

private:
	TRoleID_t get_role_id_by_client_id(TSocketIndex_t client_id) const;

private:
	TEntityID_t m_entity_id;
	TOptID_t m_opt_id;
	std::unordered_map<TOptID_t, std::function<void(bool, const binary_data&)>> m_db_result_callbacks;
	std::unordered_map<TOptID_t, std::function<void(bool)>> m_db_status_callbacks;
	std::unordered_map<TOptID_t, std::function<void(int, const dynamic_string&)>> m_http_response_callbacks;
	obj_memory_pool<resource, 65536> m_resource_pool;
	obj_memory_pool<city, 1024> m_city_pool;
	obj_memory_pool<npc, 1024> m_npc_pool;
	obj_memory_pool<farmland, 1024> m_farmland_pool;
	obj_memory_pool<resource_up_info, 128> m_resource_up_pool;
	std::unordered_map<TSocketIndex_t, role*> m_client_id_2_role;
};

#define DGameServer singleton<game_server>::get_instance()

#endif