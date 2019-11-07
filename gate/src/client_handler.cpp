
#include "client_handler.h"
#include "log.h"
#include "gate_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "server_enum.h"

client_handler::client_handler() : packet_handler<client_handler>()
{

}

client_handler::~client_handler()
{

}

void client_handler::setup_handlers()
{
	TBaseType_t::setup_handlers();
	register_handler((TPacketID_t)PACKET_ID_RPC_BY_INDEX, (packet_handler_func)&client_handler::handle_transfer_server_by_index);
	register_handler((TPacketID_t)PACKET_ID_RPC_BY_NAME, (packet_handler_func)&client_handler::handle_transfer_server_by_name);
}

service_interface * client_handler::get_service() const
{
	return singleton<gate_server>::get_instance_ptr();
}

void client_handler::handle_init()
{
	TBaseType_t::handle_init();
	DGateServer.on_client_connect(get_socket_index());
}

void client_handler::handle_close()
{
	DGateServer.on_client_disconnect(get_socket_index());
	TBaseType_t::handle_close();
}

bool client_handler::handle_transfer_server_by_index(packet_base* packet)
{
	TSocketIndex_t socket_index = get_socket_index();
	TProcessID_t process_id = DGateServer.get_process_id_by_client_id(socket_index);
	DRpcWrapper.transfer_server(process_id, socket_index, (rpc_by_index_packet*)packet);
	return true;
}

bool client_handler::handle_transfer_server_by_name(packet_base* packet)
{
	TSocketIndex_t socket_index = get_socket_index();
	TProcessID_t process_id = DGateServer.get_process_id_by_client_id(socket_index);
	DRpcWrapper.transfer_server(process_id, socket_index, (rpc_by_name_packet*)packet);
	return true;
}