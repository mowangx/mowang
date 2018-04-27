
#include "db_server.h"

#include <array>

#include "mysql_conn.h"
#include "time_manager.h"
#include "socket.h"
#include "socket_manager.h"
#include "game_server_handler.h"
#include "game_manager_handler.h"
#include "dynamic_array.h"
#include "game_enum.h"

#include "rpc_client.h"

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

bool db_server::init(TProcessID_t process_id)
{
	//m_db = new CMysqlConn();
	//if (NULL == m_db) {
	//	return false;
	//}

	//return m_db->init("127.0.0.1", 3306, "root", "123456", "test");

	m_server_info.process_info.server_id = 100;
	m_server_info.process_info.process_type = PROCESS_DB;
	m_server_info.process_info.process_id = process_id;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10100 + process_id;

	return true;
}

void db_server::run()
{
	game_server_handler::Setup();
	game_manager_handler::Setup();
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

const game_server_info& db_server::get_server_info() const
{
	return m_server_info;
}

TPacketSendInfo_t* db_server::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* db_server::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void db_server::push_write_packets(TPacketSendInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}

void db_server::register_client(rpc_client * client)
{
	m_clients[client->get_handler()->get_socket_index()] = client;
}
