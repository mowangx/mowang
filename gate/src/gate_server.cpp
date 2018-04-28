
#include "gate_server.h"
#include "game_manager_handler.h"
#include "game_server_handler.h"
#include "client_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

gate_server::gate_server() : service(PROCESS_GATE)
{
	m_write_packets.clear();
}

gate_server::~gate_server()
{

}

bool gate_server::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	m_server_info.process_info.server_id = 100;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10300 + process_id;

	DRegisterServerRpc(this, gate_server, on_query_servers, 4);
	DRegisterServerRpc(this, gate_server, login_server, 4);
	DRegisterServerRpc(this, gate_server, register_server, 2);

	game_manager_handler::Setup();
	game_server_handler::Setup();
	client_handler::Setup();

	connect_server<game_manager_handler>("127.0.0.1", 10000);

	return true;
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
		on_game_connect(servers[i]);
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

void gate_server::on_game_lose(TServerID_t server_id, TProcessID_t game_id)
{
}

void gate_server::on_game_connect(const game_server_info & server_info)
{
	if (DNetMgr.start_connect<game_server_handler>(server_info.ip.data(), server_info.port)) {
		log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
	}
	else {
		log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
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
