
#include "game_server_handler.h"
#include "log.h"
#include "gate_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

game_server_handler::game_server_handler() : packet_handler<game_server_handler>()
{
	m_rpc_client = new rpc_client(this);
}

game_server_handler::~game_server_handler()
{
	
}

void game_server_handler::Setup()
{
	TBaseType_t::Setup();
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_ROLE, (packet_handler_func)&game_server_handler::handle_transfer_role);
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_STUB, (packet_handler_func)&game_server_handler::handle_transfer_stub);
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_CLIENT, (packet_handler_func)&game_server_handler::handle_transfer_client);
}

service_interface * game_server_handler::get_service() const
{
	return singleton<gate_server>::get_instance_ptr();
}

bool game_server_handler::handle_transfer_role(packet_base * packet)
{
	transfer_role_packet* rpc_info = (transfer_role_packet*)packet;
	DGateServer.transfer_role(rpc_info->m_server_id, rpc_info->m_game_id, rpc_info->m_role_id, (packet_base*)rpc_info->m_buffer);
	return true;
}

bool game_server_handler::handle_transfer_stub(packet_base * packet)
{
	transfer_stub_packet* rpc_info = (transfer_stub_packet*)packet;
	DGateServer.transfer_stub(rpc_info->m_server_id, rpc_info->m_game_id, (packet_base*)rpc_info->m_buffer);
	return true;
}

bool game_server_handler::handle_transfer_client(packet_base * packet)
{
	transfer_client_packet* rpc_info = (transfer_client_packet*)packet;
	DGateServer.transfer_client(rpc_info->m_client_id, (packet_base*)rpc_info->m_buffer);
	return true;
}
