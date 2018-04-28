
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

const game_server_info & db_manager_handler::get_server_info() const
{
	return DGameServer.get_server_info();
}

void db_manager_handler::register_client()
{
	DGameServer.register_client(m_rpc_client);
}

void db_manager_handler::unregister_client()
{
	DGameServer.unregister_client(get_socket_index());
}
