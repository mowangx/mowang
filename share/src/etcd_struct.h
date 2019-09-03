
#ifndef _ETCD_STRUCT_H_
#define _ETCD_STRUCT_H_

#include "base_util.h"
#include "packet_struct.h"

#pragma pack(push, 1)

struct etcd_entity_info
{
	TEntityID_t entity_id;
	TEntityName_t entity_name;
	TTagName_t tag;
	etcd_entity_info() {
		clean_up();
	}

	bool operator == (const etcd_entity_info& rhs) const {
		return entity_id == rhs.entity_id;
	}

	void clean_up() {
		entity_id = INVALID_ENTITY_ID;
		memset(entity_name.data(), 0, ENTITY_NAME_LEN);
		memset(tag.data(), 0, TAG_NAME_LEN);
	}
};

struct etcd_process_info
{
	game_server_info server_info;
	std::vector<etcd_entity_info> entities;
	etcd_process_info() {
		clean_up();
	}

	void clean_up() {
		server_info.clean_up();
		entities.clear();
	}
};

struct etcd_entity_detail_info
{
	game_server_info server_info;
	etcd_entity_info entity_info;
	etcd_entity_detail_info() {
		clean_up();
	}

	void clean_up() {
		server_info.clean_up();
		entity_info.clean_up();
	}
};

struct etcd_entity_packet_info
{
	TProcessID_t process_id;
	etcd_entity_info entity_info;
	etcd_entity_packet_info() {
		clean_up();
	}

	void clean_up() {
		process_id = INVALID_PROCESS_ID;
		entity_info.clean_up();
	}
};

struct etcd_process_packet_info
{
	TProcessID_t process_id;
	game_server_info server_info;
	etcd_process_packet_info() {
		clean_up();
	}

	void clean_up() {
		process_id = INVALID_PROCESS_ID;
		server_info.clean_up();
	}
};

#pragma pack(pop)

#endif // !_ECTD_STRUCT_H_