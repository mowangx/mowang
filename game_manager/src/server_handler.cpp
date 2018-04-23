
#include "server_handler.h"
#include "log.h"
#include "game_manager.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

server_handler::server_handler() : packet_handler<server_handler>()
{
	m_rpc_client = new rpc_client(this);
}

server_handler::~server_handler()
{
	if (NULL != m_rpc_client) {
		delete m_rpc_client;
		m_rpc_client = NULL;
	}
}

void server_handler::Setup()
{
	TBaseType_t::Setup();
	register_handler((TPacketID_t)PACKET_ID_SERVER_INFO, (packet_handler_func)&server_handler::handle_server_info);
}

TPacketSendInfo_t* server_handler::create_packet_info()
{
	return DGameManager.allocate_packet_info();
}

char* server_handler::create_packet(int n)
{
	return DGameManager.allocate_memory(n);
}

void server_handler::write_packet(TPacketSendInfo_t* packet_info)
{
	DGameManager.push_write_packets(packet_info);
}

void server_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}

bool server_handler::handle_server_info(packet_base* packet)
{
	DGameManager.register_handle_info(m_rpc_client, (server_info_packet*)packet);
	return true;
}