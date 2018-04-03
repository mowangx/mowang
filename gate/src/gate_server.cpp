
#include "gate_server.h"
#include "log.h"
#include "socket.h"

#include "time_manager.h"

#include "game_enum.h"
#include "socket_manager.h"
#include "game_manager_handler.h"
#include "game_server_handler.h"

#include "rpc_proxy.h"

gate_server::gate_server()
{
	m_write_packets.clear();
	m_server_id = INVALID_SERVER_ID;
	m_process_id = INVALID_PROCESS_ID;
	memset(m_listen_ip.data(), 0, IP_SIZE);
	m_listen_port = 0;
}

gate_server::~gate_server()
{

}

bool gate_server::init(TProcessID_t process_id)
{
	m_process_id = process_id;

	m_server_id = 100;

	char* ip = "127.0.0.1";
	memcpy(m_listen_ip.data(), ip, strlen(ip));

	m_listen_port = 10300 + process_id;

	return true;
}

void gate_server::run()
{
	DRegisterRpc(this, gate_server, on_query_servers, 3);

	game_manager_handler::Setup();
	game_server_handler::Setup();
	TAppTime_t before_loop_time(0), after_loop_time(0);

	while (true) {
		before_loop_time = DTimeMgr.update();

		// 
		std::vector<TPacketInfo_t*> packets;
		std::vector<socket_base*> wait_init_sockets;
		std::vector<socket_base*> wait_del_sockets;

		DNetMgr.read_packets(packets, wait_init_sockets, wait_del_sockets);

		for (auto socket : wait_del_sockets) {
			socket->get_packet_handler()->handle_close();
		}

		for (auto socket : wait_init_sockets) {
			socket->get_packet_handler()->handle_init();
		}

		for (auto packet_info : packets) {
			packet_info->socket->get_packet_handler()->handle(packet_info->packet);
		}

		DNetMgr.finish_read_packets(packets, wait_del_sockets);
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

void gate_server::get_process_info(game_process_info& process_info) const
{
	process_info.server_id = m_server_id;
	process_info.process_type = PROCESS_GATE;
	process_info.process_id = m_process_id;
}

void gate_server::get_server_info(game_server_info& server_info) const
{
	memcpy(server_info.ip.data(), m_listen_ip.data(), IP_SIZE);
	server_info.port = m_listen_port;
}

TPacketInfo_t* gate_server::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* gate_server::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void gate_server::push_write_packets(TPacketInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}

void gate_server::on_query_servers(TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers)
{
	log_info("on_query_servers, server id = %d, process type = %d, server size = %u", server_id, process_type, servers.size());
	for (int i = 0; i < servers.size(); ++i) {
		const game_server_info& server_info = servers[i];
		if (DNetMgr.start_connect<game_server_handler>(server_info.ip.data(), server_info.port)) {
			log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
		else {
			log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
	}
}