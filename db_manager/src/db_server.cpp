
#include "db_server.h"

#include "mysql_conn.h"
#include "time_manager.h"
#include "socket.h"
#include "socket_manager.h"
#include "game_server_handler.h"

static const uint32 PER_FRAME_TIME = 50;

CDbServer::CDbServer()
{
	m_db = NULL;
}

CDbServer::~CDbServer()
{
	if (NULL != m_db) {
		delete m_db;
		m_db = NULL;
	}
}

bool CDbServer::init()
{
	return true;
	//m_db = new CMysqlConn();
	//if (NULL == m_db) {
	//	return false;
	//}

	//return m_db->init("127.0.0.1", 3306, "root", "123456", "test");
}

void CDbServer::run()
{
	CGameServerHandler::Setup();
	TAppTime_t before_loop_time(0), after_loop_time(0);

	while (true) {
		before_loop_time = DTimeMgr.update();

		static bool first_flag = true;
		if (first_flag) {
			first_flag = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(60000));
			std::vector<CSocket*> sockets;
			DNetMgr.test_get_sockets(sockets);
			for (auto s : sockets) {
				CLoginRequest login_ret;
				login_ret.m_id = 12345678;
				login_ret.m_len = sizeof(CLoginRequest);
				login_ret.m_check = 987654321;
				login_ret.m_user = 89;
				s->getPacketHandler()->handle(&login_ret);
				s->getPacketHandler()->handle(&login_ret);
				s->getPacketHandler()->handle(&login_ret);
				s->getPacketHandler()->handle(&login_ret);
				s->getPacketHandler()->handle(&login_ret);
				s->getPacketHandler()->handle(&login_ret);
				s->getPacketHandler()->handle(&login_ret);
				s->getPacketHandler()->handle(&login_ret);
			}
		}

		// 
		std::vector<TPacketInfo_t*> packets;

		int read_packet_num(0), write_packet_num(0);

		DNetMgr.read_packets(packets);
		read_packet_num = packets.size();
		for (auto packet_info : packets) {
			packet_info->socket->getPacketHandler()->handle(packet_info->packet);
		}
		DNetMgr.finish_read_packets(packets);
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

TPacketInfo_t* CDbServer::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* CDbServer::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void CDbServer::push_write_packets(TPacketInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}