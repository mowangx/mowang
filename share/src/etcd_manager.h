#ifndef _ETCD_MANAGER_H_
#define _ETCD_MANAGER_H_

#include <map>
#include <vector>
#include <unordered_map>

#include "singleton.h"
#include "etcd_struct.h"
#include "dynamic_array.h"
#include "etcd_listener.h"

class service;

class etcd_manager : singleton<etcd_manager>
{
public:
	etcd_manager();
	~etcd_manager();

public:
	bool init(service* s);

public:
	void register_entity(const TNamespace_t& name, const TTagName_t& tag, TEntityID_t entity_id, const TEntityName_t& entity_name);

public:
	void on_register_entities(const dynamic_string& name, TWaitIndex_t wait_index, const dynamic_array<etcd_process_packet_info>& processes, const dynamic_array<etcd_entity_packet_info>& entities);
	void on_unregister_process(const dynamic_string& name, TWaitIndex_t wait_index, const game_process_info& process_info);

private:
	void refresh_ttl();
	void init_entities();

private:
	void clean_up();

private:
	service* m_service;
	std::map<TNamespace_t, etcd_listener*> m_listeners;
};

#define DEtcdMgr singleton<etcd_manager>::get_instance()

#endif // !_ETCD_MANAGER_H_
