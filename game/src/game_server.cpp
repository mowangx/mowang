
#include "game_server.h"
#include "log.h"
#include "socket.h"

#include "tbl_test.h"
#include "time_manager.h"

#include "socket_manager.h"
#include "db_manager_handler.h"

static const uint32 PER_FRAME_TIME = 50;

CGameServer::CGameServer()
{
	m_write_packets.clear();
}

CGameServer::~CGameServer()
{
	
}

bool CGameServer::init()
{
	if (!DTblTestMgr.load("../config/server_test.xml")) {
		log_error("load config failed");
		return false;
	}
	log_info("load config success");

	return true;
}

void CGameServer::run()
{
	CDbManagerHandler::Setup();
	TAppTime_t before_loop_time(0), after_loop_time(0);

	while (true) {
		before_loop_time = DTimeMgr.update();

		// 
		std::vector<TPacketInfo_t*> packets;
		std::vector<CSocket*> sockets;

		DNetMgr.read_packets(packets, sockets);

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

		DNetMgr.write_packets(m_write_packets);
		m_write_packets.clear();

		after_loop_time = DTimeMgr.update();
		if ((after_loop_time - before_loop_time) < PER_FRAME_TIME) {
			std::this_thread::sleep_for(std::chrono::milliseconds(PER_FRAME_TIME + before_loop_time - after_loop_time));
		}
	}
}

TPacketInfo_t* CGameServer::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* CGameServer::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void CGameServer::push_write_packets(TPacketInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}