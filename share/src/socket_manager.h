
#ifndef _SOCKET_MANAGER_H_
#define _SOCKET_MANAGER_H_

#include <unordered_map>

#include "base_util.h"
#include "socket_util.h"
#include "msg_queue.h"

class CSocket;

class CSocketManager
{
public:
	CSocketManager();
	~CSocketManager();

public:
	bool	init();
	bool	start_listen(TPort_t port);

	void	update(uint32 diff);

private:
	bool	onAccept(CSocket* listener);
	static void OnAccept(TSocketIndex_t fd, short evt, void* arg);

	TUniqueIndex_t genUniqueIndex();

	void	cleanUp();

private:
	TUniqueIndex_t m_socketSequenceIndex;
	SocketEventBase_t* m_eventbase;
	CMsgQueue<CSocket*> m_newSocketQueue;
	std::unordered_map<TUniqueIndex_t, CSocket*> m_sockets;
};

#endif