
#include "gate_server.h"
#include "log.h"
#include "socket.h"

#include "time_manager.h"

#include "game_enum.h"
#include "socket_manager.h"
#include "game_manager_handler.h"
#include "game_server_handler.h"
#include "client_handler.h"

#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

#include "base_packet.h"

gate_server::gate_server()
{
	m_write_packets.clear();
}

gate_server::~gate_server()
{

}

bool gate_server::init(TProcessID_t process_id)
{
	m_server_info.process_info.server_id = 100;
	m_server_info.process_info.process_type = PROCESS_GATE;
	m_server_info.process_info.process_id = process_id;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10300 + process_id;
	return true;
}

void gate_server::run()
{
	DRegisterServerRpc(this, gate_server, on_query_servers, 4);
	DRegisterServerRpc(this, gate_server, login_server, 4);
	DRegisterServerRpc(this, gate_server, register_server, 2);

	game_manager_handler::Setup();
	game_server_handler::Setup();
	client_handler::Setup();
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

const game_server_info& gate_server::get_server_info() const
{
	return m_server_info;
}

TPacketSendInfo_t* gate_server::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* gate_server::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void gate_server::push_write_packets(TPacketSendInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
}

void gate_server::register_client(rpc_client * client)
{
	m_clients[client->get_handler()->get_socket_index()] = client;
}

void gate_server::register_server(TSocketIndex_t socket_index, const game_server_info& server_info)
{
	log_info("register_server, server id = %u, process type = %u, process id = %u, ip = %s, port = %u", server_info.process_info.server_id,
		server_info.process_info.process_type, server_info.process_info.process_id, server_info.ip.data(), server_info.port);
	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		DRpcWrapper.register_handler_info(itr->second, server_info);
	}
}

void gate_server::on_query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers)
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

void gate_server::login_server(TSocketIndex_t socket_index, TPlatformID_t platform_id, TServerID_t server_id, const TUserID_t& user_id)
{
	game_process_info process_info;
	process_info.server_id = server_id;
	process_info.process_type = PROCESS_GAME;
	process_info.process_id = DRpcWrapper.get_random_process_id(process_info.server_id, process_info.process_type);
	m_client_2_process[socket_index] = process_info;
	log_info("login server, client id = '%"I64_FMT"u', user id = %s, game id = %u", socket_index, user_id.data(), process_info.process_id);
	rpc_client* rpc = DRpcWrapper.get_client(process_info.server_id, process_info.process_id);
	if (NULL != rpc) {
		rpc->call_remote_func("login_server", socket_index, m_server_info.process_info.process_id, platform_id, user_id);
	}
}

void gate_server::transfer_role(TServerID_t server_id, TProcessID_t game_id, TRoleID_t role_id, packet_base* packet)
{
	TPacketSendInfo_t* packet_info = allocate_packet_info();
	packet_info->socket_index = DRpcWrapper.get_socket_index(server_id, game_id);
	role_rpc_by_name_packet* transfer_packet = (role_rpc_by_name_packet*)allocate_memory(packet->get_packet_len());
	packet_info->packet = transfer_packet;
	memcpy(transfer_packet, packet, packet->get_packet_len());
	push_write_packets(packet_info);
}

void gate_server::transfer_stub(TServerID_t server_id, TProcessID_t game_id, packet_base* packet)
{
	TPacketSendInfo_t* packet_info = allocate_packet_info();
	packet_info->socket_index = DRpcWrapper.get_socket_index(server_id, game_id);
	rpc_by_name_packet* transfer_packet = (rpc_by_name_packet*)allocate_memory(packet->get_packet_len());
	packet_info->packet = transfer_packet;
	memcpy(transfer_packet, packet, packet->get_packet_len());
	push_write_packets(packet_info);
}

void gate_server::transfer_client(TSocketIndex_t client_id, packet_base* packet)
{
	TPacketSendInfo_t* packet_info = allocate_packet_info();
	packet_info->socket_index = client_id;
	packet_base* transfer_packet = (packet_base*)allocate_memory(packet->get_packet_len());
	packet_info->packet = transfer_packet;
	memcpy(packet_info->packet, packet, packet->get_packet_len());
	push_write_packets(packet_info);
}

void gate_server::transfer_server(TSocketIndex_t client_id, packet_base * packet)
{
	TPacketSendInfo_t* packet_info = allocate_packet_info();
	packet_info->socket_index = get_server_socket_index(client_id);
	TPacketLen_t len = (TPacketLen_t)(sizeof(transfer_client_packet) - 65000 + packet->get_packet_len());
	transfer_client_packet* transfer_packet = (transfer_client_packet*)allocate_memory(len);
	packet_info->packet = transfer_packet;
	transfer_packet->m_len = len;
	transfer_packet->m_id = PACKET_ID_TRANSFER_CLIENT;
	transfer_packet->m_client_id = client_id;
	memcpy(transfer_packet->m_buffer, packet, packet->get_packet_len());
	push_write_packets(packet_info);
	log_info("transfer server! client id = '%"I64_FMT"u'", client_id);
}

TSocketIndex_t gate_server::get_server_socket_index(TSocketIndex_t client_id) const
{
	auto itr = m_client_2_process.find(client_id);
	if (itr == m_client_2_process.end()) {
		return INVALID_SOCKET_INDEX;
	}
	const game_process_info& process_info = itr->second;
	return DRpcWrapper.get_socket_index(process_info.server_id, process_info.process_id);
}
