
#include "db_server.h"

#include <array>

#include "mysql_conn.h"
#include "time_manager.h"
#include "socket.h"
#include "socket_manager.h"
#include "game_server_handler.h"

static const uint32 PER_FRAME_TIME = 50;

db_server::db_server()
{
	m_db = NULL;
}

db_server::~db_server()
{
	if (NULL != m_db) {
		delete m_db;
		m_db = NULL;
	}
}

bool db_server::init()
{
	return true;
	//m_db = new CMysqlConn();
	//if (NULL == m_db) {
	//	return false;
	//}

	//return m_db->init("127.0.0.1", 3306, "root", "123456", "test");
}

void db_server::run()
{
	game_server_handler::Setup();
	TAppTime_t before_loop_time(0), after_loop_time(0);

	while (true) {
		before_loop_time = DTimeMgr.update();

		std::vector<socket_base*> sockets;
		DNetMgr.test_get_sockets(sockets);
		for (auto s : sockets) {
			rpc_by_name_packet rpc_info;
			strcpy(rpc_info.rpc_name, "game_rpc_func_1");
			int index = 0;
			std::array<char, 22> p1;
			memcpy(p1.data(), "xiedi", 5);
			memcpy(rpc_info.buffer, p1.data(), sizeof(p1));
			index += sizeof(p1);
			uint16 p2 = 65500;
			memcpy((void*)(rpc_info.buffer + index), &p2, sizeof(p2));
			index += sizeof(p2);
			std::array<char, 127> p3;
			memcpy(p3.data(), "hello world", 11);
			memcpy((void*)(rpc_info.buffer + index), p3.data(), sizeof(p3));
			s->get_packet_handler()->handle(&rpc_info);
		}
		sockets.clear();

		// 
		std::vector<TPacketInfo_t*> packets;

		int read_packet_num(0), write_packet_num(0);

		DNetMgr.read_packets(packets, sockets);
		read_packet_num = packets.size();

		for (auto socket : sockets) {
			socket->get_packet_handler()->handle_close();
		}

		for (auto packet_info : packets) {
			packet_info->socket->get_packet_handler()->handle(packet_info->packet);
		}

		DNetMgr.finish_read_packets(packets, sockets);
		packets.clear();

		DNetMgr.finish_write_packets(packets);
		for (auto packet_info : packets) {
			m_mem_pool.deallocate((char*)packet_info->packet);
			m_packet_pool.deallocate(packet_info);
		}
		packets.clear();

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

TPacketInfo_t* db_server::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* db_server::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void db_server::push_write_packets(TPacketInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}