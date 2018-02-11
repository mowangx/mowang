
#ifndef _SOCKET_MANAGER_H_
#define _SOCKET_MANAGER_H_

#include <unordered_map>

#include "socket_util.h"

class CSocket;

class CSocketManager
{
public:
	CSocketManager();
	~CSocketManager();

public:
	bool	init();
	bool	start_listen(TPort_t port);

private:
	std::unordered_map<TSocketIndex_t, CSocket*> m_sockets;
};

#endif