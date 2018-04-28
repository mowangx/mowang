
#include "gate_handler.h"
#include "log.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

gate_handler::gate_handler() : packet_handler<gate_handler>()
{
	m_rpc_client = new rpc_client(this);
}

gate_handler::~gate_handler()
{
	
}

void gate_handler::Setup()
{
	TBaseType_t::Setup();
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_CLIENT, (packet_handler_func)&gate_handler::handle_transfer_client);
}

TPacketSendInfo_t* gate_handler::create_packet_info()
{
	return DGameServer.allocate_packet_info();
}

char* gate_handler::create_packet(int n)
{
	return DGameServer.allocate_memory(n);
}

void gate_handler::write_packet(TPacketSendInfo_t* packet_info)
{
	DGameServer.push_write_packets(packet_info);
}

const game_server_info& gate_handler::get_server_info() const
{
	return DGameServer.get_server_info();
}

void gate_handler::register_client()
{
	DGameServer.register_client(m_rpc_client);
}

void gate_handler::unregister_client()
{
	DGameServer.unregister_client(get_socket_index());
}

bool gate_handler::handle_transfer_client(packet_base * packet)
{
	transfer_client_packet* transfer_info = (transfer_client_packet*)packet;
	DGameServer.transfer_client(transfer_info->m_client_id, (packet_base*)transfer_info->m_buffer);
	return true;
}
