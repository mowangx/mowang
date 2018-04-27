
#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include <vector>
#include <map>

#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"
#include "server_manager.h"

class rpc_client;

class game_manager : public singleton<game_manager>
{
public:
	game_manager();
	~game_manager();
	
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

public:
	void register_server(TSocketIndex_t socket_index, const game_server_info& server_info);
	void query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type);

private:
	rpc_client* get_client(TSocketIndex_t socket_index);

private:
	game_server_info m_server_info;
	obj_memory_pool<TPacketSendInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketSendInfo_t*> m_write_packets;
	std::map<uint64, rpc_client*> m_clients;
};

#define DGameManager singleton<game_manager>::get_instance()

#endif // !_GAME_MANAGER_H_
