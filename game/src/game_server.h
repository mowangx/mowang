
#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include "singleton.h"
#include "service.h"
#include "dynamic_array.h"
#include "account.h"
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
public:
	virtual bool connect_server(const char* ip, TPort_t port) override;
private:
	virtual void do_loop(TGameTime_t diff) override;

public:
	void db_remove(const char* table, const char* query, const std::function<void(bool)>& callback);
	void db_insert(const char* table, const char* fields, const char* query, const std::function<void(bool)>& callback);
	void db_update(const char* table, const char* fields, const char* query, const std::function<void(bool)>& callback);
	void db_query(const char* table, const char* fields, const char* query, const std::function<void(bool, const binary_data&)>& callback);
private:
	void db_opt_with_status(uint8 opt_type, const char* table, const char* fields, const char* query, const std::function<void(bool)>& callback);
	void db_opt_with_result(uint8 opt_type, const char* table, const char* fields, const char* query, const std::function<void(bool, const binary_data&)>& callback);
	void db_opt(uint8 opt_type, const char* table, const char* fields, const char* query);

public:
	virtual void add_process(const game_server_info& server_info) override;
	virtual void register_server(TSocketIndex_t socket_index, const game_server_info& server_info) override;
	void on_opt_db_with_status(TSocketIndex_t socket_index, TOptID_t opt_id, bool status);
	void on_opt_db_with_result(TSocketIndex_t socket_index, TOptID_t opt_id, bool status, const binary_data& result);
private:
	void on_game_start();

public:
	TEntityID_t get_entity_id_by_client_id(TSocketIndex_t client_id) const;
	void update_client_entity_id(TSocketIndex_t client_id, TEntityID_t entity_id);
	void remove_client_id(TSocketIndex_t client_id);

public:
	void create_entity_globally(const std::string& entity_name, bool check_repeat = false);
	entity* create_entity_locally(const std::string& tag, const std::string& entity_name);

private:
	std::unordered_map<TOptID_t, std::function<void(bool, const binary_data&)>> m_db_result_callbacks;
	std::unordered_map<TOptID_t, std::function<void(bool)>> m_db_status_callbacks;
	TProcessNum_t m_process_num[MAX_PROCESS_TYPE_NUM];
	std::unordered_map<TSocketIndex_t, TEntityID_t> m_client_2_entity;
};

#define DGameServer singleton<game_server>::get_instance()

#endif