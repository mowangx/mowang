
#include "etcd_manager.h"
#include "timer.h"
#include "service.h"

etcd_manager::etcd_manager()
{
	clean_up();
}

etcd_manager::~etcd_manager()
{
	clean_up();
}

bool etcd_manager::init(service* s)
{
	m_service = s;
	//DTimer.add_timer(5, true, NULL, [&](void* param, TTimerID_t timer_id) {
	//	refresh_ttl();
	//});
	DTimer.add_timer(10, false, NULL, [&](void* param, TTimerID_t timer_id) {
		init_entities();
	});

	std::string server_id = gx_to_string("%d", m_service->get_server_id());
	TNamespace_t name;
	memset(name.data(), 0, NAMESPACE_LEN);
	memcpy(name.data(), server_id.c_str(), server_id.length());
	etcd_listener* listener = new etcd_listener(name, m_service);
	m_listeners[name] = listener;
	listener->init();
	return true;
}

void etcd_manager::register_entity(const TNamespace_t& name, const TTagName_t& tag, TEntityID_t entity_id, const TEntityName_t& entity_name)
{
	etcd_listener* listener = nullptr;
	auto itr = m_listeners.find(name);
	if (itr == m_listeners.end()) {
		listener = new etcd_listener(name, m_service);
		m_listeners[name] = listener;
	}
	else {
		listener = itr->second;
	}
	listener->register_entity(entity_id, entity_name);
}

void etcd_manager::on_register_entities(const dynamic_string& name, TWaitIndex_t wait_index, const dynamic_array<etcd_process_packet_info>& processes, const dynamic_array<etcd_entity_packet_info>& entities)
{
	TNamespace_t cur_name;
	memset(cur_name.data(), 0, NAMESPACE_LEN);
	memcpy(cur_name.data(), name.data(), name.size());
	auto itr = m_listeners.find(cur_name);
	if (itr != m_listeners.end()) {
		itr->second->on_register_entities(wait_index, processes, entities);
		itr->second->watch();
	}
	else {
		log_error("on unregister process failed for not find namespace! namespace name: %s", name.data());
	}
}

void etcd_manager::on_unregister_process(const dynamic_string& name, TWaitIndex_t wait_index, const game_process_info& process_info)
{
	TNamespace_t cur_name;
	memset(cur_name.data(), 0, NAMESPACE_LEN);
	memcpy(cur_name.data(), name.data(), name.size());
	auto itr = m_listeners.find(cur_name);
	if (itr != m_listeners.end()) {
		itr->second->on_unregister_process(wait_index, process_info);
		itr->second->watch();
	}
	else {
		log_error("on unregister process failed for not find namespace! namespace name: %s", name.data());
	}
}

void etcd_manager::refresh_ttl()
{
	for (auto itr = m_listeners.begin(); itr != m_listeners.end(); ++itr) {
		itr->second->refresh_ttl();
	}
}

void etcd_manager::init_entities()
{
	for (auto itr = m_listeners.begin(); itr != m_listeners.end(); ++itr) {
		itr->second->init_entities();
	}
}

void etcd_manager::clean_up()
{
	m_service = nullptr;
	m_listeners.clear();
}
