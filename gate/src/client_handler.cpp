
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