
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
	transfer_role_packet* role_packet = (transfer_role_packet*)packet;
	packet_base* rpc_packet = (packet_base*)role_packet->m_buffer;
	packet_send_info* packet_info = DGateServer.allocate_packet_info();
	packet_info->socket_index = DRpcWrapper.get_socket_index(game_process_info(role_packet->m_server_id, PROCESS_GAME, role_packet->m_game_id));
	role_rpc_by_name_packet* transfer_packet = (role_rpc_by_name_packet*)DGateServer.allocate_memory(rpc_packet->get_packet_len());
	packet_info->buffer_info.len = rpc_packet->get_packet_len();
	packet_info->buffer_info.buffer = (char*)transfer_packet;
	memcpy(transfer_packet, rpc_packet, packet_info->buffer_info.len);
	DGateServer.push_write_packets(packet_info);
	return true;
}

bool game_server_handler::handle_transfer_stub(packet_base * packet)
{
	transfer_stub_packet* stub_packet = (transfer_stub_packet*)packet;
	packet_base* rpc_packet = (packet_base*)stub_packet->m_buffer;
	packet_send_info* packet_info = DGateServer.allocate_packet_info();
	packet_info->socket_index = DRpcWrapper.get_socket_index(game_process_info(stub_packet->m_server_id, PROCESS_GAME, stub_packet->m_game_id));
	rpc_by_name_packet* transfer_packet = (rpc_by_name_packet*)DGateServer.allocate_memory(rpc_packet->get_packet_len());
	packet_info->buffer_info.len = rpc_packet->get_packet_len();
	packet_info->buffer_info.buffer = (char*)transfer_packet;
	memcpy(transfer_packet, rpc_packet, packet_info->buffer_info.len);
	DGateServer.push_write_packets(packet_info);
	return true;
}

bool game_server_handler::handle_transfer_client(packet_base * packet)
{
	transfer_client_packet* client_packet = (transfer_client_packet*)packet;
	packet_base* rpc_packet = (packet_base*)client_packet->m_buffer;
	packet_send_info* packet_info = DGateServer.allocate_packet_info();
	packet_info->socket_index = client_packet->m_client_id;
	packet_base* transfer_packet = (packet_base*)DGateServer.allocate_memory(rpc_packet->get_packet_len());
	packet_info->buffer_info.len = rpc_packet->get_packet_len();
	packet_info->buffer_info.buffer = (char*)transfer_packet;
	memcpy(transfer_packet, rpc_packet, packet_info->buffer_info.len);
	DGateServer.push_write_packets(packet_info);
	log_info("transfer server packet to client! client id = '%"I64_FMT"u', socket index = '%"I64_FMT"u'", client_packet->m_client_id, get_socket_index());
	return true;
}
