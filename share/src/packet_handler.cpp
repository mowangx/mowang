
#include "packet_handler.h"

#include "socket.h"

CGameHandler::CGameHandler() : m_socket(NULL)
{

}

void CGameHandler::handle_close()
{
	m_socket = NULL;
}

void CGameHandler::set_socket(CSocket* socket)
{
	m_socket = socket;
}

CSocket* CGameHandler::get_socket() const
{
	return m_socket;
}

bool CGameHandler::is_valid() const
{
	return NULL != m_socket;
}

TSocketIndex_t CGameHandler::get_socket_index() const
{
	if (NULL != m_socket) {
		return m_socket->get_socket_index();
	}
	return 0;
}