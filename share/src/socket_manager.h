
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
#include "socket_wrapper.h"
#include "packet_handler.h"

class CSocketManager : public Singleton<CSocketManager>
{
public:
	CSocketManager();
	~CSocketManager();

public:
	bool	init();
	void	update(uint32 diff);

	template <class T>
	bool	start_listen(TPort_t port);

	template <class T>
	bool	start_connect(const char* host, TPort_t port);

	uint32	socket_num() const;

	void	read_packets(std::vector<TPacketInfo_t*>& packets);
	void	finish_read_packets(std::vector<TPacketInfo_t*>& packets);

	void	write_packets(std::vector<TPacketInfo_t*>& packets);
	void	finish_write_packets(std::vector<TPacketInfo_t*>& packets);

	void	test_get_sockets(std::vector<CSocket*>& sockets);

private:
	bool	onAccept(CSocketWrapper* listener);

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

	void	sendPacket(CSocket* socket, char* msg, uint32 len);

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

template <class T>
bool CSocketManager::start_listen(TPort_t port)
{
	CSocketWrapper * socket = new CSocketListener<T>("any", port);
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

	TSocketEvent_t& listen_event = socket->getReadEvent();
	TSocketWrapperEventArg_t& event_arg = socket->get_wrapper_event_arg();
	event_arg.s = socket;
	event_arg.mgr = this;
	if (0 != event_assign(&listen_event, m_eventbase, socket->getSocketIndex(), EV_READ | EV_PERSIST,
		CSocketManager::OnAccept, &event_arg)) {
		log_warning("can't event assign!");
		return false;
	}

	if (0 != event_add(&listen_event, NULL)) {
		log_warning("can't event add!errno=%s", strerror(errno));
		return false;
	}

	return true;
}


template <class T>
bool CSocketManager::start_connect(const char* host, TPort_t port)
{
	CSocketWrapper* socket = new CSocketConnector<T>();
	if (NULL == socket) {
		return false;
	}

	if (!socket->create()) {
		return false;
	}

	if (!socket->connect(host, port, 0)) {
		return false;
	}

	socket->setNonBlocking(true);

	TUniqueIndex_t index = genUniqueIndex();
	socket->setUniqueIndex(index);
	log_info("connect socket success! index = '%"I64_FMT"u', host = %s, port = %u", index, host, port);

	socket->setPacketHandler(socket->create_handler());
	socket->getPacketHandler()->set_socket(socket);
	socket->getPacketHandler()->set_index(index);

	addSocket(socket);

	return true;
}

#define DNetMgr		CSocketManager::getInstance()

#endif