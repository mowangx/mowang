

#include "robot_server.h"
#include "time_manager.h"
#include "socket_manager.h"
#include "gate_handler.h"
#include "rpc_client.h"

robot_server::robot_server()
{
}

robot_server::~robot_server()
{
}

bool robot_server::init(TProcessID_t process_id)
{
	return true;
}

TPacketSendInfo_t* robot_server::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* robot_server::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void robot_server::push_write_packets(TPacketSendInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}

void robot_server::run()
{
	gate_handler::Setup();
	TAppTime_t before_loop_time(0), after_loop_time(0);

	while (true) {
		before_loop_time = DTimeMgr.update();

		// 
		std::vector<TPacketRecvInfo_t*> read_packets;

		int read_packet_num(0), write_packet_num(0);

		std::vector<socket_base*> wait_init_sockets;
		std::vector<socket_base*> wait_del_sockets;
		DNetMgr.read_packets(read_packets, wait_init_sockets, wait_del_sockets);
		read_packet_num = read_packets.size();

		for (auto socket : wait_del_sockets) {
			socket->get_packet_handler()->handle_close();
		}

		for (auto socket : wait_init_sockets) {
			socket->get_packet_handler()->handle_init();
		}

		for (auto packet_info : read_packets) {
			packet_info->socket->get_packet_handler()->handle(packet_info->packet);
		}

		DNetMgr.finish_read_packets(read_packets, wait_del_sockets);
		read_packets.clear();

		std::vector<TPacketSendInfo_t*> write_packets;
		DNetMgr.finish_write_packets(write_packets);
		for (auto packet_info : write_packets) {
			m_mem_pool.deallocate((char*)packet_info->packet);
			m_packet_pool.deallocate(packet_info);
		}
		write_packets.clear();

		write_packet_num = m_write_packets.size();
		DNetMgr.write_packets(m_write_packets);
		m_write_packets.clear();

		after_loop_time = DTimeMgr.update();
		log_info("loop time: %d, read packet num = %d, write packet num = %d", (after_loop_time - before_loop_time), read_packet_num, write_packet_num);
		if ((after_loop_time - before_loop_time) < PER_FRAME_TIME) {
			std::this_thread::sleep_for(std::chrono::milliseconds(PER_FRAME_TIME + before_loop_time - after_loop_time));
		}
	}
}

void robot_server::register_gate(TSocketIndex_t client_id, rpc_client * rpc)
{
	m_gates[client_id] = rpc;
}

void robot_server::robot_rpc_func_1(TSocketIndex_t client_id, const dynamic_string & p1, uint16 p2, const std::array<char, 127>& p3)
{
	log_info("robot rpc func 1, p1 = %s, p2 = %d, p3 = %s", p1.data(), p2, p3.data());
	uint8 p2_1 = 99;
	std::array<char, 33> p2_2;
	memset(p2_2.data(), 0, 33);
	memcpy(p2_2.data(), "mowang", 6);
	auto itr = m_gates.find(client_id);
	if (itr != m_gates.end()) {
		rpc_client* rpc = itr->second;
		rpc->call_remote_func("game_rpc_func_2", p2_1, p2_2);
	}
}

void robot_server::robot_rpc_func_2(TSocketIndex_t client_id, uint8 p1, const std::array<char, 33>& p2)
{
	log_info("robot rpc func 2, p1 = %d, p2 = %s", p1, p2.data());
	dynamic_string p2_1("xiedi");
	uint16 p2_2 = 65500;
	std::array<char, 127> p2_3;
	memset(p2_3.data(), 0, 127);
	memcpy(p2_3.data(), "hello world", 11);
	auto itr = m_gates.find(client_id);
	if (itr != m_gates.end()) {
		rpc_client* rpc = itr->second;
		rpc->call_remote_func("game_rpc_func_1", p2_1, p2_2, p2_3);
	}
}
