
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

const game_server_info & game_manager_handler::get_server_info() const
{
	return DGateServer.get_server_info();
}

void game_manager_handler::register_client()
{
	DGateServer.register_client(m_rpc_client);
}

void game_manager_handler::unregister_client()
{
	DGateServer.unregister_client(get_socket_index());
}