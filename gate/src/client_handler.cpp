
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

TPacketInfo_t* client_handler::create_packet_info()
{
	return DGateServer.allocate_packet_info();
}

char* client_handler::create_packet(int n)
{
	return DGateServer.allocate_memory(n);
}

void client_handler::write_packet(TPacketInfo_t* packet_info)
{
	DGateServer.push_write_packets(packet_info);
}

void client_handler::handle_init()
{
	log_info("'%"I64_FMT"u', handle init", get_socket_index());
	server_info_packet server_info;
	DGateServer.get_process_info(server_info.m_process_info);
	DGateServer.get_server_info(server_info.m_server_info);
	server_info.m_len = sizeof(server_info);
	send_packet(&server_info);

	game_process_info process_info;
	DGateServer.get_process_info(process_info);
	m_rpc_client->call_remote_func("query_servers", process_info, (TServerID_t)100, (TProcessType_t)PROCESS_DB);
}

void client_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}