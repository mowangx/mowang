
#include "socket_manager.h"
#include "socket.h"
#include "socket_api.h"
#include "log.h"

CSocketManager::CSocketManager()
{
	
}

CSocketManager::~CSocketManager()
{
	SOCKET_API::gx_lib_cleanup();
}

bool CSocketManager::init()
{
	if (!SOCKET_API::gx_lib_init()) {
		return false;
	}
	return true;
}

bool CSocketManager::start_listen(TPort_t port)
{
	CSocket* socket = new CSocket("any", port);
	if (NULL == socket) {
		return false;
	}

	if (!socket->create()) {
		return false;
	}

	if (!socket->setReuseAddr()) {
		return false;
	}

	if (!socket->bind()) {
		return false;
	}

	if (!socket->listen(5)) {
		return false;
	}

	socket->setReuseAddr(true);
	socket->setLinger(0);
	socket->setNonBlocking(true);

	m_sockets[socket->getSocketIndex()] = socket;
	return true;
}