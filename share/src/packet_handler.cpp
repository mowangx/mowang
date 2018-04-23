
#include "packet_handler.h"

#include "socket.h"
#include "rpc_proxy.h"

game_handler::game_handler() : m_socket_index(INVALID_SOCKET_INDEX)
{

}

void game_handler::handle_init()
{
	
}

void game_handler::handle_close()
{
	m_socket_index = INVALID_SOCKET_INDEX;
}

bool game_handler::handle_rpc_by_index(packet_base* packet)
{
	rpc_by_index_packet* rpc_info = (rpc_by_index_packet*)packet;
	DRpcStub.call(rpc_info->m_rpc_index, rpc_info->m_buffer);
	return true;
}

bool game_handler::handle_rpc_by_name(packet_base* packet)
{
	rpc_by_name_packet* rpc_info = (rpc_by_name_packet*)packet;
	DRpcStub.call(rpc_info->m_rpc_name, rpc_info->m_buffer);
	return true;
}

bool game_handler::handle_role_rpc_by_index(packet_base * packet)
{
	role_rpc_by_index_packet* rpc_info = (role_rpc_by_index_packet*)packet;
	DRpcRole.call(rpc_info->m_role_id, rpc_info->m_rpc_index, rpc_info->m_buffer);
	return true;
}

bool game_handler::handle_role_rpc_by_name(packet_base * packet)
{
	role_rpc_by_name_packet* rpc_info = (role_rpc_by_name_packet*)packet;
	DRpcRole.call(rpc_info->m_role_id, rpc_info->m_rpc_name, rpc_info->m_buffer);
	return true;
}

void game_handler::set_socket_index(TSocketIndex_t socket_index)
{
	m_socket_index = socket_index;
}

TSocketIndex_t game_handler::get_socket_index() const
{
	return m_socket_index;
}

bool game_handler::is_valid() const
{
	return INVALID_SOCKET_INDEX != m_socket_index;
}