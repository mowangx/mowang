
#ifndef _SOCKET_MANAGER_H_
#define _SOCKET_MANAGER_H_

#include <mutex>
#include <unordered_map>

#include "singleton.h"
#include "base_util.h"
#include "socket_util.h"
#include "msg_queue.h"
#include "memory_pool.h"
#include "socket_handler.h"
#include "base_packet.h"

class CSocket;

class CSocketManager : public Singleton<CSocketManager>
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

	void	read_packets(std::vector<TPacketInfo_t*>& packets);
	void	finish_read_packets(std::vector<TPacketInfo_t*>& packets);

	void	write_packets(std::vector<TPacketInfo_t*>& packets);
	void	finish_write_packets(std::vector<TPacketInfo_t*>& packets);

private:
	bool	onAccept(CSocket* listener);

	void	onWrite(CSocket* socket);
	void	onRead(CSocket* socket);

	void	handleNewSocket();
	void	handleUnPacket();
	void	handleSocketUnPacket(CSocket* socket);
	void	handleWriteMsg();
	void	handleCloseSocket(CSocket* socket, bool writeFlag);
	void	handleReleasePacket();

	void	addSocket(CSocket* socket);
	void	delSocket(CSocket* socket);

	void	sendPacket(TUniqueIndex_t index, char* msg, uint32 len);

	TUniqueIndex_t genUniqueIndex();

	void	cleanUp();

private:
	static void OnAccept(TSocketIndex_t fd, short evt, void* arg);
	static void OnWriteEvent(TSocketIndex_t fd, short evt, void* arg);
	static void OnReadEvent(TSocketIndex_t fd, short evt, void* arg);

private:
	std::mutex m_mutex;
	TUniqueIndex_t m_socketSequenceIndex;
	SocketEventBase_t* m_eventbase;
	CMemoryAllocator<MAX_PACKET_BUFFER_SIZE, 100> m_packetBufferPool;
	CObjMemoryPool<CSocketHandler, 100> m_socketHandlerPool;
	CObjMemoryPool<TPacketInfo_t, 1000> m_packetInfoPool;
	CMemoryPool	m_memPool;
	CMsgQueue<CSocket*> m_newSocketQueue;
	std::unordered_map<TUniqueIndex_t, CSocket*> m_sockets;
	std::vector<TPacketInfo_t*> m_readPackets;
	std::vector<TPacketInfo_t*> m_finishReadPackets;
	std::vector<TPacketInfo_t*> m_writePackets;
	std::vector<TPacketInfo_t*> m_finishWritePackets;
};

#define DNetMgr		CSocketManager::getInstance()

#endif