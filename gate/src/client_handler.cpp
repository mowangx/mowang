
#include "client_handler.h"
#include "log.h"
#include "gate_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "game_enum.h"

client_handler::client_handler() : packet_handler<client_handler>()
{
	m_rpc_client = new rpc_client(this);
}

client_handler::~client_handler()
{
	if (NULL != m_rpc_client) {
		delete m_rpc_client;
		m_rpc_client = NULL;
	}
}

void client_handler::Setup()
{
	TBaseType_t::Setup();
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_SERVER_BY_INDEX, (packet_handler_func)&client_handler::handle_transfer_server_by_index);
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_SERVER_BY_NAME, (packet_handler_func)&client_handler::handle_transfer_server_by_name);
	register_handler((TPacketID_t)PACKET_ID_LOGIN, (packet_handler_func)&client_handler::handle_login);
}

TPacketSendInfo_t* client_handler::create_packet_info()
{
	return DGateServer.allocate_packet_info();
}

char* client_handler::create_packet(int n)
{
	return DGateServer.allocate_memory(n);
}

void client_handler::write_packet(TPacketSendInfo_t* packet_info)
{
	DGateServer.push_write_packets(packet_info);
}

void client_handler::handle_init()
{
	
}

void client_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}

bool client_handler::handle_login(packet_base * packet)
{
	login_packet* login_info = (login_packet*)packet;
	DGateServer.login_server(get_socket_index(), login_info->m_server_id, login_info->m_platform_id, login_info->m_user_id);
	return true;
}

bool client_handler::handle_transfer_server_by_index(packet_base * packet)
{
	DGateServer.transfer_server(get_socket_index(), packet);
	return true;
}

bool client_handler::handle_transfer_server_by_name(packet_base * packet)
{
	DGateServer.transfer_server(get_socket_index(), packet);
	return true;
}
