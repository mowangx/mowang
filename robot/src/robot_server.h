
#ifndef _ROBOT_SERVER_H_
#define _ROBOT_SERVER_H_

#include <vector>
#include <unordered_map>

#include "singleton.h"
#include "base_packet.h"
#include "memory_pool.h"
#include "dynamic_array.h"

class rpc_client;

class robot_server : public singleton<robot_server>
{
public:
	robot_server();
	~robot_server();

public:
	bool init(TProcessID_t process_id);

public:
	TPacketSendInfo_t * allocate_packet_info();
	char* allocate_memory(int n);
	void push_write_packets(TPacketSendInfo_t* packet_info);

public:
	void run();

public:
	void register_client(rpc_client* rpc);

public:
	void robot_rpc_func_1(TSocketIndex_t client_id, const dynamic_string& p1, uint16 p2, const std::array<char, 127>& p3);
	void robot_rpc_func_2(TSocketIndex_t client_id, uint8 p1, const std::array<char, 33>& p2);

private:
	obj_memory_pool<TPacketSendInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TPacketSendInfo_t*> m_write_packets;
	std::unordered_map<TSocketIndex_t, rpc_client*> m_gates;
	rpc_client* m_rpc_client;
};

#define DRobotServer singleton<robot_server>::get_instance()

#endif // !_ROBOT_SERVER_H_

