
#include "game_manager.h"
#include "log.h"
#include "server_handler.h"
#include "time_manager.h"
#include "socket_manager.h"
#include "game_enum.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

game_manager::game_manager()
{
	m_write_packets.clear();
}

game_manager::~game_manager()
{

}

bool game_manager::init(TProcessID_t process_id)
{
	m_server_info.process_info.server_id = 100;
	m_server_info.process_info.process_type = PROCESS_GAME_MANAGER;
	m_server_info.process_info.process_id = process_id;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10000;
	return true;
}

void game_manager::run()
{
	DRegisterServerRpc(this, game_manager, register_server, 2)
	DRegisterServerRpc(this, game_manager, query_servers, 3);

	server_handler::Setup();
	TAppTime_t before_loop_time(0), after_loop_time(0);

	while (true) {
		before_loop_time = DTimeMgr.update();

		// 
		std::vector<TPacketRecvInfo_t*> read_packets;
		std::vector<socket_base*> wait_init_sockets;
		std::vector<socket_base*> wait_del_sockets;

		DNetMgr.read_packets(read_packets, wait_init_sockets, wait_del_sockets);

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

		DNetMgr.write_packets(m_write_packets);
		m_write_packets.clear();

		after_loop_time = DTimeMgr.update();
		if ((after_loop_time - before_loop_time) < PER_FRAME_TIME) {
			std::this_thread::sleep_for(std::chrono::milliseconds(PER_FRAME_TIME + before_loop_time - after_loop_time));
		}
	}
}

const game_server_info & game_manager::get_server_info() const
{
	return m_server_info;
}

TPacketSendInfo_t* game_manager::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* game_manager::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void game_manager::push_write_packets(TPacketSendInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}

void game_manager::register_client(rpc_client* client)
{
	m_clients[client->get_handler()->get_socket_index()] = client;
}

void game_manager::register_server(TSocketIndex_t socket_index, const game_server_info& server_info)
{
	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		DRpcWrapper.register_handler_info(itr->second, server_info);
	}
	log_info("register handle info, server id = %d, process type = %d, process id = %d, listen ip = %s, port = %d",
		server_info.process_info.server_id, (TProcessType_t)server_info.process_info.process_type, server_info.process_info.process_id, 
		server_info.ip.data(), server_info.port);
}

void game_manager::query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type)
{
	log_info("query servers, server id = %d, process type = %d", server_id, process_type);
	dynamic_array<game_server_info> servers;
	DRpcWrapper.get_server_infos(server_id, process_type, servers);
	rpc_client* rpc = get_client(socket_index);
	if (NULL != rpc) {
		rpc->call_remote_func("on_query_servers", server_id, process_type, servers);
	}
}

rpc_client* game_manager::get_client(TSocketIndex_t socket_index)
{
	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		return itr->second;
	}
	return NULL;
}