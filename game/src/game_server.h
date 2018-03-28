
#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include <unordered_map>

#include <array>

#include "types_def.h"
#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"

class game_server : public singleton<game_server>
{
public:
	game_server();
	~game_server();

public:
	bool init();
	void run();

public:
	TPacketInfo_t* allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketInfo_t* packet_info);

public:
	void game_rpc_func_1(const std::array<char, 22>& p1, uint16 p2, const std::array<char, 127>& p3);
	void game_rpc_func_2(uint8 p1, const std::array<char, 33>& p2);

private:
	obj_memory_pool<TPacketInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketInfo_t*> m_write_packets;
};

#define DGameSerger singleton<game_server>::get_instance()

#endif