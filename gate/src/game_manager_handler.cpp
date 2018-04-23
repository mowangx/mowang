
#include "game_manager_handler.h"
#include "log.h"
#include "gate_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "game_enum.h"

game_manager_handler::game_manager_handler() : packet_handler<game_manager_handler>()
{
	m_rpc_client = new rpc_client(this);
}

game_manager_handler::~game_manager_handler()
{
	if (NULL != m_rpc_client) {
		delete m_rpc_client;
		m_rpc_client = NULL;
	}
}

void game_manager_handler::Setup()
{
	TBaseType_t::Setup();
}

TPacketSendInfo_t* game_manager_handler::create_packet_info()
{
	return DGateServer.allocate_packet_info();
}

char* game_manager_handler::create_packet(int n)
{
	return DGateServer.allocate_memory(n);
}

void game_manager_handler::write_packet(TPacketSendInfo_t* packet_info)
{
	DGateServer.push_write_packets(packet_info);
}

void game_manager_handler::handle_init()
{
	log_info("'%"I64_FMT"u', handle init", get_socket_index());
	server_info_packet server_info;
	DGateServer.get_server_info(server_info.m_server_info);
	server_info.m_len = sizeof(server_info);
	send_packet(&server_info);

	game_process_info process_info;
	DGateServer.get_process_info(process_info);
	m_rpc_client->call_remote_func("query_servers", process_info, (TServerID_t)100, (TProcessType_t)PROCESS_GAME);
}

void game_manager_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}