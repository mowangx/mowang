
#include "db_manager_handler.h"
#include "log.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

db_manager_handler::db_manager_handler() : packet_handler<db_manager_handler>()
{
	m_rpc_client = new rpc_client(this);
}

db_manager_handler::~db_manager_handler()
{
	if (NULL != m_rpc_client) {
		delete m_rpc_client;
		m_rpc_client = NULL;
	}
}

void db_manager_handler::Setup()
{
	TBaseType_t::Setup();
}

TPacketSendInfo_t* db_manager_handler::create_packet_info()
{
	return DGameServer.allocate_packet_info();
}

char* db_manager_handler::create_packet(int n)
{
	return DGameServer.allocate_memory(n);
}

void db_manager_handler::write_packet(TPacketSendInfo_t* packet_info)
{
	DGameServer.push_write_packets(packet_info);
}

void db_manager_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}