
#include "entity_manager.h"
#include "roll_stub.h"
#include "lbs_stub.h"
#include "fight_stub.h"
#include "game_server.h"

entity_manager::entity_manager()
{
}

entity_manager::~entity_manager()
{
}

void entity_manager::init()
{
	m_create_entity_funcs["roll_stub"] = [&]() {
		return new roll_stub();
	};
	m_create_entity_funcs["lbs_stub"] = [&]() {
		return new lbs_stub();
	};
	m_create_entity_funcs["fight_stub"] = [&]() {
		return new fight_stub();
	};
	m_create_entity_funcs["role"] = [&]() {
		return m_role_pool.allocate();
	};
	m_create_entity_funcs["room"] = [&]() {
		return m_room_pool.allocate();
	};
}

entity * entity_manager::create_entity(const std::string& entity_name)
{
	auto itr = m_create_entity_funcs.find(entity_name);
	if (itr == m_create_entity_funcs.end()) {
		return nullptr;
	}
	const std::function<entity*()>& create_func = itr->second;
	entity* e = create_func();
	if (NULL != e) {
		++m_entity_id;
		e->init(DGameServer.get_server_id(), DGameServer.get_game_id(), m_entity_id);

		entity_info info;
		info.e = e;
		info.name = entity_name;
		m_entities[m_entity_id] = info;
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
	if (info.name == "role") {
		m_role_pool.deallocate((role*)info.e);
	}
	else if (info.name == "room") {
		m_room_pool.deallocate((room*)info.e);
	}
	else {
		delete info.e;
	}
	m_entities.erase(itr);
}

entity* entity_manager::get_entity(TEntityID_t entity_id)
{
	auto itr = m_entities.find(entity_id);
	if (itr == m_entities.end()) {
		return nullptr;
	}
	return itr->second.e;
}
