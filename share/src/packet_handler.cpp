
#include "packet_handler.h"

#include "socket.h"
#include "rpc_proxy.h"

game_handler::game_handler() : m_socket(NULL)
{

}

void game_handler::handle_init()
{
	
}

void game_handler::handle_close()
{
	m_socket = NULL;
}

bool game_handler::handle_rpc_by_index(packet_base* packet)
{
	rpc_by_index_packet* rpc_info = (rpc_by_index_packet*)packet;
	DRpcProxy.call(rpc_info->m_rpc_index, rpc_info->m_buffer);
	return true;
}

bool game_handler::handle_rpc_by_name(packet_base* packet)
{
	rpc_by_name_packet* rpc_info = (rpc_by_name_packet*)packet;
	DRpcProxy.call(rpc_info->m_rpc_name, rpc_info->m_buffer);
	return true;
}

void game_handler::set_socket(socket_base* socket)
{
	m_socket = socket;
}

socket_base* game_handler::get_socket() const
{
	return m_socket;
}

bool game_handler::is_valid() const
{
	return NULL != m_socket;
}

TSocketIndex_t game_handler::get_socket_index() const
{
	if (NULL != m_socket) {
		return m_socket->get_socket_index();
	}
	return 0;
}