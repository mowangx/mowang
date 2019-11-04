
#include "entity_manager.h"
#include "log.h"
#include "game_server.h"
#include "roll_stub.h"

entity_manager::entity_manager()
{
	m_entity_id = INVALID_ENTITY_ID;
}

entity_manager::~entity_manager()
{
}

void entity_manager::init()
{
	//m_entity_id = DGameServer.get_server_id();
	//m_entity_id <<= 8;
	//m_entity_id += DGameServer.get_game_id();
	//m_entity_id <<= 40;
	m_entity_id = DGameServer.get_game_id();
	m_entity_id <<= 8;

	m_create_entity_funcs["account"] = [&]() {
		return m_account_pool.allocate();
	};
	m_create_entity_funcs["role"] = [&]() {
		return m_role_pool.allocate();
	};
	m_create_entity_funcs["roll_stub"] = [&]() {
		return new roll_stub();
	};
}

server_entity* entity_manager::create_entity(const std::string& entity_name, TProcessID_t gate_id, TSocketIndex_t client_id)
{
	auto itr = m_create_entity_funcs.find(entity_name);
	if (itr == m_create_entity_funcs.end()) {
		log_error("create entity failed for not find entity name: %s", entity_name.c_str());
		return nullptr;
	}
	const game_server_info& server_info = DGameServer.get_server_info();
	const std::function<server_entity*()>& create_func = itr->second;
	server_entity* e = create_func();
	if (NULL != e) {
		++m_entity_id;
		e->init(m_entity_id, gate_id, client_id);

		entity_info info;
		info.e = e;
		info.name = entity_name;
		m_entities[m_entity_id] = info;
	}
	else {
		log_error("create entity failed for new failed! entity name: %s", entity_name.c_str());
	}
	return e;
}

void entity_manager::destroy_entity(TEntityID_t entity_id)
{
	auto itr = m_entities.find(entity_id);
	if (itr == m_entities.end()) {
		return;
	}

	entity_info& info = itr->second;
	if (info.name == "account") {
		m_account_pool.deallocate((account*)info.e);
	}
	else if (info.name == "role") {
		m_role_pool.deallocate((role*)info.e);
	}
	else {
		delete info.e;
	}
	m_entities.erase(itr);
}

server_entity* entity_manager::get_entity(TEntityID_t entity_id)
{
	auto itr = m_entities.find(entity_id);
	if (itr == m_entities.end()) {
		return nullptr;
	}
	return itr->second.e;
}
