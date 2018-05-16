
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
	transfer_server(packet);
	return true;
}

bool client_handler::handle_transfer_server_by_name(packet_base* packet)
{
	transfer_server(packet);
	return true;
}

void client_handler::transfer_server(packet_base* packet) const
{
	TPacketSendInfo_t* packet_info = DGateServer.allocate_packet_info();
	packet_info->socket_index = DGateServer.get_server_socket_index(get_socket_index());
	TPacketLen_t len = (TPacketLen_t)(sizeof(transfer_client_packet) - 65000 + packet->get_packet_len());
	transfer_client_packet* transfer_packet = (transfer_client_packet*)DGateServer.allocate_memory(len);
	packet_info->packet = transfer_packet;
	transfer_packet->m_len = len;
	transfer_packet->m_id = PACKET_ID_TRANSFER_CLIENT;
	transfer_packet->m_client_id = get_socket_index();
	memcpy(transfer_packet->m_buffer, packet, packet->get_packet_len());
	DGateServer.push_write_packets(packet_info);
	log_info("transfer client packet to server! client id = '%"I64_FMT"u', socket index = '%"I64_FMT"u'", packet_info->socket_index, get_socket_index());
}