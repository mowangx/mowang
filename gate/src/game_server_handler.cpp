
#include "game_server_handler.h"
#include "log.h"
#include "gate_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

game_server_handler::game_server_handler() : packet_handler<game_server_handler>()
{
	m_rpc_client = new rpc_client(this);
}

game_server_handler::~game_server_handler()
{
	if (NULL != m_rpc_client) {
		delete m_rpc_client;
		m_rpc_client = NULL;
	}
}

void game_server_handler::Setup()
{
	TBaseType_t::Setup();
	register_handler((TPacketID_t)PACKET_ID_SERVER_INFO, (packet_handler_func)&game_server_handler::handle_server_info);
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_ROLE, (packet_handler_func)&game_server_handler::handle_transfer_role);
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_STUB, (packet_handler_func)&game_server_handler::handle_transfer_stub);
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_CLIENT, (packet_handler_func)&game_server_handler::handle_transfer_client);
}

TPacketSendInfo_t* game_server_handler::create_packet_info()
{
	return DGateServer.allocate_packet_info();
}

char* game_server_handler::create_packet(int n)
{
	return DGateServer.allocate_memory(n);
}

void game_server_handler::write_packet(TPacketSendInfo_t* packet_info)
{
	DGateServer.push_write_packets(packet_info);
}

void game_server_handler::handle_init()
{
	log_info("'%"I64_FMT"u', handle init", get_socket_index());
	server_info_packet server_info;
	DGateServer.get_server_info(server_info.m_server_info);
	server_info.m_len = sizeof(server_info);
	send_packet(&server_info);
}

void game_server_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}

bool game_server_handler::handle_server_info(packet_base * packet)
{
	DRpcWrapper.register_handle_info(m_rpc_client, (server_info_packet*)packet);
	return true;
}

bool game_server_handler::handle_transfer_role(packet_base * packet)
{
	transfer_role_packet* rpc_info = (transfer_role_packet*)packet;
	DGateServer.transfer_role(rpc_info->m_server_id, rpc_info->m_game_id, rpc_info->m_role_id, (packet_base*)rpc_info->m_buffer);
	return true;
}

bool game_server_handler::handle_transfer_stub(packet_base * packet)
{
	transfer_stub_packet* rpc_info = (transfer_stub_packet*)packet;
	DGateServer.transfer_stub(rpc_info->m_server_id, rpc_info->m_game_id, (packet_base*)rpc_info->m_buffer);
	return true;
}

bool game_server_handler::handle_transfer_client(packet_base * packet)
{
	transfer_client_packet* rpc_info = (transfer_client_packet*)packet;
	DGateServer.transfer_client(rpc_info->m_client_id, (packet_base*)rpc_info->m_buffer);
	return true;
}
