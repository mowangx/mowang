
#include "gate_packet_handler.h"
#include "log.h"
#include "gate_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

gate_packet_handler::gate_packet_handler() : packet_handler<gate_packet_handler>()
{
}

gate_packet_handler::~gate_packet_handler()
{
}

void gate_packet_handler::setup_handlers()
{
	TBaseType_t::setup_handlers();
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_PAKCET, (packet_handler_func)&gate_packet_handler::handle_transfer_packet);
	register_handler((TPacketID_t)PACKET_ID_WS_CLIENT, (packet_handler_func)&gate_packet_handler::handle_transfer_ws_client);
}

service_interface * gate_packet_handler::get_service() const
{
	return singleton<gate_server>::get_instance_ptr();
}

bool gate_packet_handler::handle_transfer_packet(packet_base * packet)
{
	transfer_entity_packet* client_packet = (transfer_entity_packet*)packet;
	packet_base* rpc_packet = (packet_base*)client_packet->m_buffer;
	packet_send_info* packet_info = DGateServer.allocate_packet_info();
	packet_info->socket_index = client_packet->m_client_id;
	packet_base* transfer_packet = (packet_base*)DGateServer.allocate_memory(rpc_packet->get_packet_len());
	packet_info->buffer_info.len = rpc_packet->get_packet_len();
	packet_info->buffer_info.buffer = (char*)transfer_packet;
	memcpy(transfer_packet, rpc_packet, packet_info->buffer_info.len);
	DGateServer.push_write_packets(packet_info);
	log_info("transfer server packet to client! client id %" I64_FMT "u, socket index = %" I64_FMT "u", client_packet->m_client_id, get_socket_index());
	return true;
}

bool gate_packet_handler::handle_transfer_ws_client(packet_base * packet)
{
	ws_client_packet* client_packet = (ws_client_packet*)packet;
	int len = client_packet->get_packet_len() - sizeof(packet_base) - sizeof(client_packet->m_client_id);
	std::string msg(client_packet->m_buffer, len);
	DGateServer.push_ws_write_packets(client_packet->m_client_id, msg);
	log_info("transfer server packet to client! client id %" I64_FMT "u, socket index %" I64_FMT "u", client_packet->m_client_id, get_socket_index());
	return true;
}
