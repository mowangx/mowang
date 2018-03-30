
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
	bool init();
	void run();

public:
	TPacketInfo_t * allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketInfo_t* packet_info);

public:
	void register_handle_info(rpc_client* client, server_info_packet* server_info);

public:
	void query_servers(const game_process_info& src_process_info, TServerID_t server_id, TProcessType_t process_type);

private:
	rpc_client* get_client(const game_process_info& process_info);
	uint64 get_client_key_id(const game_process_info& process_info);

private:
	obj_memory_pool<TPacketInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketInfo_t*> m_write_packets;
	std::map<uint64, rpc_client*> m_clients;
	server_manager m_gates;
	server_manager m_games;
	server_manager m_dbs;
};

#define DGameManager singleton<game_manager>::get_instance()

#endif // !_GAME_MANAGER_H_
