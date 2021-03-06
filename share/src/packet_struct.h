
#ifndef _PACKET_STRUCT_H_
#define _PACKET_STRUCT_H_

#include "socket_util.h"
#include "base_util.h"
#include "dynamic_array.h"

#pragma pack(push, 1)

struct game_process_info
{
	TServerID_t server_id;
	TProcessType_t process_type;
	TProcessID_t process_id;
	game_process_info() {
		clean_up();
	}

	game_process_info(TServerID_t _server_id, TProcessType_t _process_type, TProcessID_t _process_id) {
		server_id = _server_id;
		process_type = _process_type;
		process_id = _process_id;
	}

	void clean_up() {
		server_id = INVALID_SERVER_ID;
		process_type = INVALID_PROCESS_TYPE;
		process_id = INVALID_PROCESS_ID;
	}

	bool operator == (const game_process_info& rhs) const {
		return server_id == rhs.server_id && process_type == rhs.process_type && process_id == rhs.process_id;
	}
};

struct game_server_info
{
	game_process_info process_info;
	TPort_t port;
	TIP_t ip;
	game_server_info() {
		clean_up();
	}

	game_server_info(const game_server_info& rhs) {
		process_info.server_id = rhs.process_info.server_id;
		process_info.process_type = rhs.process_info.process_type;
		process_info.process_id = rhs.process_info.process_id;
		port = rhs.port;
		memcpy(ip.data(), rhs.ip.data(), IP_LEN);
	}

	game_server_info(TServerID_t _server_id, TProcessType_t _process_type, TProcessID_t _process_id, TPort_t _port, const TIP_t& _ip) {
		process_info.server_id = _server_id;
		process_info.process_type = _process_type;
		process_info.process_id = _process_id;
		port = _port;
		memcpy(ip.data(), _ip.data(), IP_LEN);
	}

	bool operator == (const game_server_info& rhs) const {
		return process_info.server_id == rhs.process_info.server_id 
			&& process_info.process_type == rhs.process_info.process_type 
			&& process_info.process_id == rhs.process_info.process_id;
	}

	void clean_up() {
		process_info.clean_up();
		port = 0;
		memset(ip.data(), 0, IP_LEN);
	}
};

// Game ---> Gate ---> Client
struct proxy_info
{
	TProcessID_t gate_id;
	TSocketIndex_t client_id;
	proxy_info() {
		clean_up();
	}

	bool operator != (const proxy_info& rhs) const {
		return client_id != rhs.client_id || gate_id != rhs.gate_id;
	}

	void clean_up() {
		gate_id = INVALID_PROCESS_ID;
		client_id = INVALID_SOCKET_INDEX;
	}
};

// Game ---> Game
struct mailbox_info
{
	TIP_t ip;
	TPort_t port;
	TEntityID_t entity_id;
	mailbox_info() {
		clean_up();
	}

	bool operator != (const mailbox_info& rhs) const {
		return entity_id != rhs.entity_id || port != rhs.port || ip != rhs.ip;
	}

	void clean_up() {
		memset(ip.data(), 0, IP_LEN);
		port = 0;
		entity_id = INVALID_ENTITY_ID;
	}
};

struct game_stub_info
{
	TEntityName_t stub_name;
	mailbox_info mailbox;
	game_stub_info() {
		clean_up();
	}

	void clean_up() {
		memset(stub_name.data(), 0, ENTITY_NAME_LEN);
		mailbox.clean_up();
	}
};

struct stub_role_info
{
	proxy_info proxy;
	mailbox_info mailbox;
	stub_role_info() {
		clean_up();
	}

	void clean_up() {
		proxy.clean_up();
		mailbox.clean_up();
	}
};

#pragma pack(pop)

#endif // !_PACKET_STRUCT_H_

