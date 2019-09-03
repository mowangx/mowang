
#include "gate_handler.h"
#include "log.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

gate_handler::gate_handler() : packet_handler<gate_handler>()
{

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
	return true;
}
