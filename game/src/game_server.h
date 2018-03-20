
#ifndef _GAME_SERVER_H_
#define _GAME_SERVER_H_

#include <unordered_map>

#include "types_def.h"
#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"

class CGameServer : public Singleton<CGameServer>
{
public:
	CGameServer();
	~CGameServer();

public:
	bool init();
	void run();

public:
	TPacketInfo_t* allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketInfo_t* packet_info);

private:
	CObjMemoryPool<TPacketInfo_t, 1000> m_packet_pool;
	CMemoryPool m_mem_pool;
	std::vector<TPacketInfo_t*> m_write_packets;
};

#define DGameSerger Singleton<CGameServer>::getInstance()

#endif