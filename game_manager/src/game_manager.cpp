
#include "game_manager.h"
#include "log.h"
#include "server_handler.h"
#include "time_manager.h"
#include "socket_manager.h"
#include "game_enum.h"
#include "rpc_client.h"
#include "rpc_proxy.h"

game_manager::game_manager()
{
	m_write_packets.clear();
}

game_manager::~game_manager()
{

}

bool game_manager::init()
{
	return true;
}

void game_manager::run()
{
	DRegisterRpc(this, game_manager, query_servers, 3);

	server_handler::Setup();
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

TPacketInfo_t* game_manager::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* game_manager::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void game_manager::push_write_packets(TPacketInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}

void game_manager::register_handle_info(rpc_client* client, server_info_packet* server_info)
{
	m_clients[get_client_key_id(server_info->m_process_info)] = client;
	TProcessType_t process_type = server_info->m_process_info.process_type;
	if (process_type == PROCESS_GATE) {
		m_gates.register_server(server_info->m_process_info.server_id, server_info->m_server_info.ip, server_info->m_server_info.port);
	}
	else if (process_type == PROCESS_GAME) {
		m_games.register_server(server_info->m_process_info.server_id, server_info->m_server_info.ip, server_info->m_server_info.port);
	}
	else if (process_type == PROCESS_DB) {
		m_dbs.register_server(server_info->m_process_info.server_id, server_info->m_server_info.ip, server_info->m_server_info.port);
	}
	log_info("register handle info, server id = %d, process type = %d, process id = %d, listen ip = %s, port = %d", 
		server_info->m_process_info.server_id, (TProcessType_t)server_info->m_process_info.process_type, server_info->m_process_info.process_id, 
		server_info->m_server_info.ip.data(), server_info->m_server_info.port);
}

void game_manager::query_servers(const game_process_info& process_info, TServerID_t server_id, TProcessType_t process_type)
{
	dynamic_array<game_server_info> servers;
	if (process_type == PROCESS_GATE) {
		m_gates.get_servers(server_id, servers);
	}
	else if (process_type == PROCESS_GAME) {
		m_games.get_servers(server_id, servers);
	}
	else if (process_type == PROCESS_DB) {
		m_dbs.get_servers(server_id, servers);
	}
	rpc_client* rpc = get_client(process_info);
	if (NULL != rpc) {
		rpc->call_remote_func("on_query_servers", server_id, process_type, servers);
	}
}

rpc_client* game_manager::get_client(const game_process_info& process_info)
{
	uint64 key_id = get_client_key_id(process_info);
	auto itr = m_clients.find(key_id);
	if (itr != m_clients.end()) {
		return itr->second;
	}
	return NULL;
}

uint64 game_manager::get_client_key_id(const game_process_info& process_info)
{
	uint64 key_id = process_info.server_id;
	key_id = (key_id << (sizeof(process_info.process_type) * 8)) + process_info.process_type;
	key_id = (key_id << (sizeof(process_info.process_id) * 8)) + process_info.process_id;
	return key_id;
}