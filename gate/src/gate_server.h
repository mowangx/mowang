
#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include <vector>

#include "socket_util.h"
#include "singleton.h"
#include "memory_pool.h"
#include "base_packet.h"
#include "dynamic_array.h"

class gate_server : public singleton<gate_server>
{
public:
	gate_server();
	~gate_server();

public:
	bool init(TProcessID_t process_id);
	void run();

public:
	void get_process_info(game_process_info& process_info) const;
	void get_server_info(game_server_info& server_info) const;

public:
	TPacketInfo_t * allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketInfo_t* packet_info);

public:
	void on_query_servers(TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);

private:
	TServerID_t m_server_id;
	TProcessID_t m_process_id;
	std::array<char, IP_SIZE> m_listen_ip;
	TPort_t m_listen_port;
	obj_memory_pool<TPacketInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketInfo_t*> m_write_packets;
};

#define DGateServer singleton<gate_server>::get_instance()

#endif