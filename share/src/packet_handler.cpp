
#include "packet_handler.h"

#include "socket.h"

game_handler::game_handler() : m_socket(NULL)
{

}

void game_handler::handle_close()
{
	m_socket = NULL;
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