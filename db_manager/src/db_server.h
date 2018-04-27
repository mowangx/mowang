
#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include <vector>
#include <map>

#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"

#include "dynamic_array.h"

class db_conn;
class rpc_client;

class db_server : public singleton<db_server>
{
public:
	db_server();
	~db_server();

public:
	bool init(TProcessID_t process_id);
	void run();

public:
	const game_server_info& get_server_info() const;

public:
	TPacketSendInfo_t * allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketSendInfo_t* packet_info);

public:
	void register_client(rpc_client* client);

private:
	game_server_info m_server_info;
	db_conn * m_db;
	obj_memory_pool<TPacketSendInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketSendInfo_t*> m_write_packets;
	std::map<TSocketIndex_t, rpc_client*> m_clients;
};

#define DDbServer singleton<db_server>::get_instance()


#endif // !_DB_SERVER_H_

