
#include "etcd_listener.h"
#include <algorithm>
#include "timer.h"
#include "rpc_wrapper.h"
#include "mailbox_manager.h"
#include "service.h"

etcd_listener::etcd_listener(const TNamespace_t& name, service* s)
{
	clean_up();
	m_namespace = name;
	m_service = s;
}

etcd_listener::~etcd_listener()
{
	clean_up();
}

void etcd_listener::init()
{
	register_entity_core();
}

void etcd_listener::register_entity(TEntityID_t entity_id, const TEntityName_t & entity_name)
{
	etcd_entity_info entity_info;
	entity_info.entity_id = entity_id;
	entity_info.entity_name = entity_name;
	m_local_enities.push_back(entity_info);
	m_cache_enities.push_back(entity_info);
	DTimer.add_timer(1, false, NULL, [&](void* param, TTimerID_t timer_id) {
		real_register_entity();
	});
}

void etcd_listener::real_register_entity()
{
	if (m_cache_enities.empty()) {
		return;
	}
	m_cache_enities.clear();
	register_entity_core();
}

void etcd_listener::register_entity_core()
{
	rpc_client* rpc = DRpcWrapper.get_client_by_process_type(PROCESS_HTTP_CLIENT);
	if (nullptr == rpc) {
		log_error("not find http client proxy!");
		return;
	}
	dynamic_string name(m_namespace.data());
	dynamic_array<etcd_entity_info> entities;
	for (auto entity_info : m_local_enities) {
		entities.push_back(entity_info);
	}
	rpc->call_remote_func("register_process_entities", name, m_service->get_server_info(), entities);
	log_info("call register process entities success! %s", name.data());
}

void etcd_listener::on_register_entities(TWaitIndex_t wait_index, const dynamic_array<etcd_process_packet_info>& processes, const dynamic_array<etcd_entity_packet_info>& entities)
{
	std::unordered_map<TProcessID_t, game_server_info> process_id_2_server_info;
	for (int i = 0; i < processes.size(); ++i) {
		process_id_2_server_info[processes[i].process_id] = processes[i].server_info;
		m_service->add_process(processes[i].server_info);
	}
	for (int i = 0; i < entities.size(); ++i) {
		auto it = process_id_2_server_info.find(entities[i].process_id);
		if (it == process_id_2_server_info.end()) {
			continue;
		}
		etcd_entity_detail_info entity_info;
		entity_info.server_info = it->second;
		entity_info.entity_info = entities[i].entity_info;
		uint64 key_id = get_key_id_by_process_info(entity_info.server_info.process_info);
		auto itr = m_global_entities.find(key_id);
		if (itr == m_global_entities.end()) {
			etcd_process_info process_info;
			process_info.server_info = entity_info.server_info;
			process_info.entities.push_back(entity_info.entity_info);
			m_global_entities[key_id] = process_info;
		}
		else {
			std::vector<etcd_entity_info>& cur_entities = itr->second.entities;
			auto entity_itr = std::find(cur_entities.begin(), cur_entities.end(), entity_info.entity_info);
			if (entity_itr == cur_entities.end()) {
				cur_entities.push_back(entity_info.entity_info);
			}
		}
		DMailboxMgr.add_entity(entity_info);
		log_info("on register entity success! entity id: %" I64_FMT "u, entity name: %s, tag: %s, server id: %u, process type: %d, process id: %u, ip: %s, port: %u",
			entity_info.entity_info.entity_id, entity_info.entity_info.entity_name.data(), entity_info.entity_info.tag.data(),
			entity_info.server_info.process_info.server_id, entity_info.server_info.process_info.process_type, entity_info.server_info.process_info.process_id,
			entity_info.server_info.ip.data(), entity_info.server_info.port);
	}
	if (wait_index > 0) {
		m_wait_index = wait_index;
	}
}

void etcd_listener::on_unregister_process(TWaitIndex_t wait_index, const game_process_info& process_info)
{
	log_info("on unregister process! server id: %u, process type: %d, process id: %u", process_info.server_id, process_info.process_type, process_info.process_id);
	DMailboxMgr.del_entity(process_info);
	if (wait_index > 0) {
		m_wait_index = wait_index;
	}
	uint64 key_id = get_key_id_by_process_info(process_info);
	auto itr = m_global_entities.find(key_id);
	if (itr == m_global_entities.end()) {
		return;
	}
	etcd_process_info& cur_process_info = itr->second;
	log_info("on unregister process! server id: %u, process type: %d, process id: %u, ip: %s, port: %u",
		cur_process_info.server_info.process_info.server_id, cur_process_info.server_info.process_info.process_type, cur_process_info.server_info.process_info.process_id,
		cur_process_info.server_info.ip, cur_process_info.server_info.port);
	m_global_entities.erase(itr);
}

void etcd_listener::refresh_ttl()
{
	rpc_client* rpc = DRpcWrapper.get_client_by_process_type(PROCESS_HTTP_CLIENT);
	if (nullptr == rpc) {
		return;
	}
	dynamic_string name(m_namespace.data());
	rpc->call_remote_func("refresh_ttl", name, m_service->get_server_info().process_info);
}

void etcd_listener::init_entities()
{
	rpc_client* rpc = DRpcWrapper.get_client_by_process_type(PROCESS_HTTP_CLIENT);
	if (nullptr == rpc) {
		return;
	}
	dynamic_string name(m_namespace.data());
	rpc->call_remote_func("request_all", name);
}

void etcd_listener::watch()
{
	rpc_client* rpc = DRpcWrapper.get_client_by_process_type(PROCESS_HTTP_CLIENT);
	if (nullptr == rpc) {
		return;
	}
	dynamic_string name(m_namespace.data());
	rpc->call_remote_func("request_one", name, m_wait_index);
}

uint64 etcd_listener::get_key_id_by_process_info(const game_process_info & process_info) const
{
	uint64 key_id = process_info.server_id;
	key_id = (key_id << sizeof(TProcessType_t) * 8) + process_info.process_type;
	return (key_id << sizeof(TProcessID_t) * 8) + process_info.process_id;
}

void etcd_listener::clean_up()
{
	m_service = nullptr;
	m_wait_index = INVALID_WAIT_INDEX;
	memset(m_namespace.data(), 0, NAMESPACE_LEN);
	m_local_enities.clear();
	m_cache_enities.clear();
	m_global_entities.clear();
}
