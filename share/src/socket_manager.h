
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
	void	update(uint32 diff);

	bool	start_listen(TPort_t port);
	bool	start_connect(const char* host, TPort_t port);

	uint32	socket_num() const;

private:
	bool	onAccept(CSocket* listener);

	void	onWrite(CSocket* socket);
	void	onRead(CSocket* socket);

	void	handleNewSocket();
	void	handleUnPacket();
	void	handleSocketUnPacket(CSocket* socket);
	void	handleWriteMsg(TUniqueIndex_t index, char* msg, uint32 len);
	void	handleCloseSocket(CSocket* socket, bool writeFlag);

	void	addSocket(CSocket* socket);
	void	delSocket(CSocket* socket);

	TUniqueIndex_t genUniqueIndex();

	void	cleanUp();

private:
	static void OnAccept(TSocketIndex_t fd, short evt, void* arg);
	static void OnWriteEvent(TSocketIndex_t fd, short evt, void* arg);
	static void OnReadEvent(TSocketIndex_t fd, short evt, void* arg);

private:
	TUniqueIndex_t m_socketSequenceIndex;
	SocketEventBase_t* m_eventbase;
	CMsgQueue<CSocket*> m_newSocketQueue;
	std::unordered_map<TUniqueIndex_t, CSocket*> m_sockets;
};

#endif