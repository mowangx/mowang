
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

}

void server_handler::Setup()
{
	TBaseType_t::Setup();
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

const game_server_info & server_handler::get_server_info() const
{
	return DGameManager.get_server_info();
}

void server_handler::register_client()
{
	DGameManager.register_client(m_rpc_client);
}

void server_handler::unregister_client()
{
	DGameManager.unregister_client(get_socket_index());
}
