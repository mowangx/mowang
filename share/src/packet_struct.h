
#ifndef _PACKET_STRUCT_H_
#define _PACKET_STRUCT_H_

#include "socket_util.h"
#include "base_util.h"

#pragma pack(push, 1)

struct game_process_info
{
	TServerID_t server_id;
	TProcessType_t process_type;
	TProcessID_t process_id;
	game_process_info() {
		clean_up();
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
			&& process_info.process_id == rhs.process_info.process_id 
			&& port == rhs.port;
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
	TRoleID_t role_id;
	mailbox_info() {
		clean_up();
	}

	void clean_up() {
		server_id = INVALID_SERVER_ID;
		game_id = INVALID_PROCESS_ID;
		role_id = INVALID_ROLE_ID;
	}
};

#pragma pack(pop)

#endif // !_PACKET_STRUCT_H_

