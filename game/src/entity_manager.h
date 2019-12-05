
#ifndef _ENTITY_MANAGER_H_
#define _ENTITY_MANAGER_H_

#include <map>
#include <unordered_map>

#include "singleton.h"
#include "memory_pool.h"
#include "server_struct.h"
#include "account.h"
#include "role.h"


class server_entity;

class entity_manager : public singleton<entity_manager>
{
	struct entity_info {
		std::string name;
		server_entity* e;
		entity_info() {
			clean_up();
		}

		void clean_up() {
			name = "";
			e = nullptr;
		}
	};
public:
	entity_manager();
	~entity_manager();

public:
	void init();
	void update();

public:
	server_entity* create_entity(const std::string& entity_name, TProcessID_t gate_id = INVALID_PROCESS_ID, TSocketIndex_t client_id = INVALID_SOCKET_INDEX);
	void destroy_entity(TEntityID_t entity_id);
private:
	void destroy_entity_core(TEntityID_t entity_id);

public:
	server_entity* get_entity(TEntityID_t entity_id);

private:
	TGameTime_t m_last_time;
	TEntityID_t m_entity_id;
	obj_memory_pool<account, 1024> m_account_pool;
	obj_memory_pool<role, 1024> m_role_pool;
	std::vector<TEntityID_t> m_destroy_ids;
	std::vector<TEntityID_t> m_delay_destroy_ids;
	std::map<std::string, std::function<server_entity*()>> m_create_entity_funcs;
	std::unordered_map<TEntityID_t, entity_info> m_entities;
};

#define DEntityMgr singleton<entity_manager>::get_instance()

#endif // !_ENTITY_MANAGER_H_
