
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

}

void client_handler::Setup()
{
	TBaseType_t::Setup();
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_SERVER_BY_INDEX, (packet_handler_func)&client_handler::handle_transfer_server_by_index);
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_SERVER_BY_NAME, (packet_handler_func)&client_handler::handle_transfer_server_by_name);
}

service_interface * client_handler::get_service() const
{
	return singleton<gate_server>::get_instance_ptr();
}

void client_handler::handle_close()
{
	DGateServer.logout_server(get_socket_index());
	TBaseType_t::handle_close();
}

bool client_handler::handle_transfer_server_by_index(packet_base* packet)
{
	DGateServer.transfer_server(get_socket_index(), packet);
	return true;
}

bool client_handler::handle_transfer_server_by_name(packet_base* packet)
{
	DGateServer.transfer_server(get_socket_index(), packet);
	return true;
}