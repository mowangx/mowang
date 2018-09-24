
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
};

struct game_server_info
{
	game_process_info process_info;
	TPort_t port;
	std::array<char, IP_SIZE> ip;
	game_server_info() {
		process_info.clean_up();
		port = 0;
		memset(ip.data(), 0, IP_SIZE);
	}

	game_server_info(const game_server_info& rhs) {
		process_info.server_id = rhs.process_info.server_id;
		process_info.process_type = rhs.process_info.process_type;
		process_info.process_id = rhs.process_info.process_id;
		port = rhs.port;
		memcpy(ip.data(), rhs.ip.data(), IP_SIZE);
	}

	game_server_info(TServerID_t _server_id, TProcessType_t _process_type, TProcessID_t _process_id, TPort_t _port, const std::array<char, IP_SIZE>& _ip) {
		process_info.server_id = _server_id;
		process_info.process_type = _process_type;
		process_info.process_id = _process_id;
		port = _port;
		memcpy(ip.data(), _ip.data(), IP_SIZE);
	}

	bool operator == (const game_server_info& rhs) const {
		return process_info.server_id == rhs.process_info.server_id 
			&& process_info.process_type == rhs.process_info.process_type 
			&& process_info.process_id == rhs.process_info.process_id;
	}
};

// Game ---> Gate ---> Client
struct proxy_info
{
	TServerID_t server_id;
	TProcessID_t gate_id;
	TSocketIndex_t client_id;
	proxy_info() {
		clean_up();
	}

	bool operator != (const proxy_info& rhs) const {
		return client_id != rhs.client_id || gate_id != rhs.gate_id || server_id != rhs.server_id;
	}

	void clean_up() {
		server_id = INVALID_SERVER_ID;
		gate_id = INVALID_PROCESS_ID;
		client_id = INVALID_SOCKET_INDEX;
	}
};

// Game ---> Gate ---> Game
struct mailbox_info
{
	TServerID_t server_id;
	TProcessID_t game_id;
	TEntityID_t entity_id;
	mailbox_info() {
		clean_up();
	}

	bool operator != (const mailbox_info& rhs) const {
		return entity_id != rhs.entity_id || game_id != rhs.game_id || server_id != rhs.server_id;
	}

	void clean_up() {
		server_id = INVALID_SERVER_ID;
		game_id = INVALID_PROCESS_ID;
		entity_id = INVALID_ENTITY_ID;
	}
};

struct game_stub_info
{
	TEntityName_t stub_name;
	game_process_info process_info;
	game_stub_info() {
		clean_up();
	}

	void clean_up() {
		memset(stub_name.data(), 0, ENTITY_NAME_LEN);
		process_info.clean_up();
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

struct game_pos
{
	TPosValue_t x;
	TPosValue_t y;
	game_pos() {
		clean_up();
	}

	bool operator == (const game_pos& rhs) const {
		return x == rhs.x && y == rhs.y;
	}

	bool operator != (const game_pos& rhs) const {
		return x != rhs.x || y != rhs.y;
	}

	void clean_up() {
		x = INVALID_POS_VALUE;
		y = INVALID_POS_VALUE;
	}
};

struct soldier_info
{
	TSoldierType_t soldier_type;
	TSoldierNum_t soldier_num;
	soldier_info() {
		clean_up();
	}

	void clean_up() {
		soldier_type = INVALID_SOLDIER_TYPE;
		soldier_num = INVALID_SOLDIER_NUM;
	}
};

struct account_info
{
	TTokenID_t token;
	TRoleName_t role_name;
	TSex_t sex;
	TSocketIndex_t test_client_id;
	account_info() {
		clean_up();
	}

	void clean_up() {
		memset(token.data(), 0, TOKEN_LEN);
		memset(role_name.data(), 0, ROLE_NAME_LEN);
		sex = INVALID_SEX;
		test_client_id = INVALID_SOCKET_INDEX;
	}
};

#pragma pack(pop)

#endif // !_PACKET_STRUCT_H_

