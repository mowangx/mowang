
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
	std::array<char, IP_SIZE> ip;
	TPort_t port;
	game_server_info() {
		memset(ip.data(), 0, IP_SIZE);
		port = 0;
	}

	game_server_info(const std::array<char, IP_SIZE>& _ip, TPort_t _port) {
		memcpy(ip.data(), _ip.data(), IP_SIZE);
		port = _port;
	}

	bool operator == (const game_server_info& rhs) const {
		return port == rhs.port && 0 == strcmp(ip.data(), rhs.ip.data());
	}
};

#pragma pack(pop)

#endif // !_PACKET_STRUCT_H_

