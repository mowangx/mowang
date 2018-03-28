
#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include <vector>

#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"

class db_conn;

class db_server : public singleton<db_server>
{
public:
	db_server();
	~db_server();

public:
	bool init();
	void run();


public:
	TPacketInfo_t * allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketInfo_t* packet_info);

private:
	db_conn * m_db;
	obj_memory_pool<TPacketInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketInfo_t*> m_write_packets;
};

#define DDbServer singleton<db_server>::get_instance()


#endif // !_DB_SERVER_H_

