
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

service_interface* gate_handler::get_service() const
{
	return singleton<game_server>::get_instance_ptr();
}

bool gate_handler::handle_transfer_client(packet_base * packet)
{
	transfer_client_packet* transfer_info = (transfer_client_packet*)packet;
	DGameServer.transfer_client(transfer_info->m_gate_id, transfer_info->m_client_id, (packet_base*)transfer_info->m_buffer);
	return true;
}
