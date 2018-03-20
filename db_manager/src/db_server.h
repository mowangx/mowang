
#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include <vector>

#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"

class CDbConn;

class CDbServer : public CSingleton<CDbServer>
{
public:
	CDbServer();
	~CDbServer();

public:
	bool init();
	void run();

public:
	TPacketInfo_t * allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketInfo_t* packet_info);

private:
	CDbConn * m_db;
	CObjMemoryPool<TPacketInfo_t, 1000> m_packet_pool;
	CMemoryPool m_mem_pool;
	std::vector<TPacketInfo_t*> m_write_packets;
};

#define DDbServer CSingleton<CDbServer>::getInstance()


#endif // !_DB_SERVER_H_

