
#ifndef _ETCD_LISTENER_H_
#define _ETCD_LISTENER_H_

#include <vector>
#include <unordered_map>

#include "etcd_struct.h"

class service;

class etcd_listener
{
public:
	etcd_listener(const TNamespace_t& name, service* s);
	~etcd_listener();

public:
	void init();

public:
	void register_entity(TEntityID_t entity_id, const TEntityName_t& entity_name);
private:
	void real_register_entity();
	void register_entity_core();

public:
	void on_register_entities(TWaitIndex_t wait_index, const dynamic_array<etcd_process_packet_info>& processes, const dynamic_array<etcd_entity_packet_info>& entities);
	void on_unregister_process(TWaitIndex_t wait_index, const game_process_info& process_info);

public:
	void refresh_ttl();
	void init_entities();
	void watch();

private:
	uint64 get_key_id_by_process_info(const game_process_info& process_info) const;

private:
	void clean_up();

private:
	service* m_service;
	TWaitIndex_t m_wait_index;
	TNamespace_t m_namespace;
	std::vector<etcd_entity_info> m_cache_enities;
	std::vector<etcd_entity_info> m_local_enities;
	std::unordered_map<uint64, etcd_process_info> m_global_entities;
};

#endif // !_ETCD_LISTENER_H_

