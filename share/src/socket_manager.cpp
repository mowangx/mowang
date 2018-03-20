
#include "socket_manager.h"

#include "auto_lock.h"
#include "log.h"
#include "socket.h"
#include "socket_api.h"

CSocketManager::CSocketManager()
{
	m_socketSequenceIndex = 0;
	m_eventbase = NULL;
	m_sockets.clear();
	m_readPackets.clear();
	m_finishReadPackets.clear();
	m_writePackets.clear();
	m_finishWritePackets.clear();
}

CSocketManager::~CSocketManager()
{
	SOCKET_API::gx_lib_cleanup();
	cleanUp();
}

bool CSocketManager::init()
{
	if (!SOCKET_API::gx_lib_init()) {
		return false;
	}
	
	m_eventbase = event_base_new();
	if (NULL == m_eventbase) {
		return false;
	}

	return true;
}

void CSocketManager::update(uint32 diff)
{
	if (-1 == event_base_loop(m_eventbase, EVLOOP_ONCE | EVLOOP_NONBLOCK)) {
		log_info("update ret -1");
		return;
	}

	handleNewSocket();

	handleUnPacket();

	handleReleasePacket();

	handleWriteMsg();
}

uint32 CSocketManager::socket_num() const
{
	return (uint32)m_sockets.size();
}

void CSocketManager::read_packets(std::vector<TPacketInfo_t*>& packets)
{
	CLock lock(&m_mutex);
	packets.insert(packets.end(), m_readPackets.begin(), m_readPackets.end());
	m_readPackets.clear();
}

void CSocketManager::finish_read_packets(std::vector<TPacketInfo_t*>& packets)
{
	CLock lock(&m_mutex);
	m_finishReadPackets.insert(m_finishReadPackets.begin(), packets.begin(), packets.end());
}

void CSocketManager::write_packets(std::vector<TPacketInfo_t*>& packets)
{
	CLock lock(&m_mutex);
	m_writePackets.insert(m_writePackets.end(), packets.begin(), packets.end());
}

void CSocketManager::finish_write_packets(std::vector<TPacketInfo_t*>& packets)
{
	CLock lock(&m_mutex);
	packets.insert(packets.end(), m_finishWritePackets.begin(), m_finishWritePackets.end());
	m_finishWritePackets.clear();
}

void CSocketManager::test_get_sockets(std::vector<CSocket*>& sockets)
{
	for (auto itr : m_sockets) {
		sockets.push_back(itr.second);
	}
}

bool CSocketManager::onAccept(CSocketWrapper* listener)
{
	CSocket* socket = listener->accept();
	if (NULL == socket) {
		return false;
	}

	// @todo 检测添加的时候会不会影响性能
	TUniqueIndex_t index = genUniqueIndex();
	socket->setUniqueIndex(index);
	log_info("Accept socket! index = '%"I64_FMT"u'", index);

	socket->setPacketHandler(listener->create_handler());
	socket->getPacketHandler()->set_socket(socket);
	socket->getPacketHandler()->set_index(index);

	m_newSocketQueue.push(socket);

	return true;
}

void CSocketManager::onWrite(CSocket* socket)
{
	if (!socket->onWrite()) {
		handleCloseSocket(socket, true);
		return;
	}

	if (socket->isNeedWrite()) {
		TSocketEvent_t& writeEvent = socket->getWriteEvent();
		event_add(&writeEvent, NULL);
	}
}

void CSocketManager::onRead(CSocket* socket)
{
	if (!socket->isActive()) {
		return;
	}

	if (!socket->onRead()) {
		handleCloseSocket(socket, false);
		return;
	}
}

void CSocketManager::handleNewSocket()
{
	std::list<CSocket*> lst;
	m_newSocketQueue.pop(lst);
	for (auto socket : lst) {
		addSocket(socket);
	}
}

void CSocketManager::handleUnPacket()
{
	std::vector<CSocket*> delSockets;

	for (auto itr = m_sockets.begin(); itr != m_sockets.end(); ++itr) {
		CSocket* socket = itr->second;
		if (NULL == socket) {
			continue;
		}

		if (!socket->isActive()) {
			continue;
		}

		if (socket->getInputLen() > 0) {
			handleSocketUnPacket(socket);
		}

		if (!socket->isActive()) {
			delSockets.push_back(socket);
		}
	}

	for (uint32 i = 0; i < delSockets.size(); ++i) {
		handleCloseSocket(delSockets[i], false);
	}
}

void CSocketManager::handleSocketUnPacket(CSocket* socket)
{
	int len = socket->getInputLen();
	if (len > MAX_PACKET_READ_SIZE) {
		len = MAX_PACKET_READ_SIZE;
	}
	CSocketHandler* socket_handler = socket->getSocketHandler();
	int cur_len = socket->read(socket_handler->buffer(len), len);
	if (cur_len != len) {
		log_error("socket index = '%"I64_FMT"u', read len is not equal cache len, read len = %d, cache len = %d", socket->getUniqueIndex(), cur_len, len);
	}
	CBasePacket* packet = socket_handler->unpacket();
	while (NULL != packet) {
		char* packet_pool = m_memPool.allocate(packet->get_packet_len());
		memcpy(packet_pool, packet, packet->get_packet_len());
		TPacketInfo_t* packet_info = m_packetInfoPool.allocate();
		packet_info->socket = socket;
		packet_info->packet = (CBasePacket*)packet_pool;
		{
			CLock lock(&m_mutex);
			m_readPackets.push_back(packet_info);
		}
		packet = socket_handler->unpacket();
	}
}

void CSocketManager::handleWriteMsg()
{
	std::vector<TPacketInfo_t*> packets;
	{
		CLock lock(&m_mutex);
		packets.insert(packets.begin(), m_writePackets.begin(), m_writePackets.end());
		m_writePackets.clear();
	}
	for (auto packet_info : packets) {
		if (m_sockets.find(packet_info->index) != m_sockets.end()) {
			sendPacket(packet_info->socket, (char*)packet_info->packet, packet_info->packet->get_packet_len());
		}
		else {
			log_info("send packet failed for can't find socket index! socket index = '%"I64_FMT"u'", packet_info->index);
		}
		CLock lock(&m_mutex);
		m_finishWritePackets.push_back(packet_info);
	}
}

void CSocketManager::handleCloseSocket(CSocket* socket, bool writeFlag)
{
	if (writeFlag) {
		log_info("Write close socke!index = '%"I64_FMT"u'", socket->getUniqueIndex());
	} else {
		log_info("Read close socke!index = '%"I64_FMT"u'", socket->getUniqueIndex());
	}

	if (socket->isActive()) {
		socket->getOutputStream()->flush();
	}

	delSocket(socket);
}

void CSocketManager::handleReleasePacket()
{
	std::vector<TPacketInfo_t*> packets;
	{
		CLock lock(&m_mutex);
		packets.insert(packets.begin(), m_finishReadPackets.begin(), m_finishReadPackets.end());
		m_finishReadPackets.clear();
	}
	for (auto packet_info : packets) {
		m_memPool.deallocate((char*)packet_info->packet);
		m_packetInfoPool.deallocate(packet_info);
	}
}

void CSocketManager::addSocket(CSocket* socket)
{
	TSocketEvent_t& readEvent = socket->getReadEvent();
	TSocketEvent_t& writeEvent = socket->getWriteEvent();
	TSocketEventArg_t& eventArg = socket->getEventArg();
	eventArg.mgr = this;
	eventArg.s = socket;

	if (0 != event_assign(&readEvent, m_eventbase, socket->getSocketIndex(), EV_READ | EV_PERSIST, CSocketManager::OnReadEvent, &eventArg)) {

	}
	if (0 != event_assign(&writeEvent, m_eventbase, socket->getSocketIndex(), EV_WRITE, CSocketManager::OnWriteEvent, &eventArg)) {

	}

	if (0 != event_add(&readEvent, NULL)) {
	}

	if (0 != event_add(&writeEvent, NULL)) {
	}

	TUniqueIndex_t index = socket->getUniqueIndex();
	if (m_sockets.find(index) != m_sockets.end()) {
		log_error("The socket index is repeat! index = '%"I64_FMT"u'", index);
		return;
	}

	CSocketHandler* socket_handler = m_socketHandlerPool.allocate();
	socket_handler->setBuffer(m_packetBufferPool.allocate());
	socket->setSocketHandler(socket_handler);

	m_sockets[index] = socket;
}

void CSocketManager::delSocket(CSocket* socket)
{
	// 将数据全部写出
	socket->onWrite();

	TSocketEvent_t& readEvent = socket->getReadEvent();
	event_del(&readEvent);
	TSocketEvent_t& writeEvent = socket->getWriteEvent();
	event_del(&writeEvent);

	// 关闭链接
	socket->close();

	CSocketHandler* socket_handler = socket->getSocketHandler();
	m_packetBufferPool.deallocate(socket_handler->buffer());
	m_socketHandlerPool.deallocate(socket_handler);

	m_sockets.erase(socket->getUniqueIndex());

	delete socket->getPacketHandler();

	DSafeDelete(socket);
}

void CSocketManager::sendPacket(CSocket* socket, char* msg, uint32 len)
{
	socket->write(msg, len);
	TSocketEvent_t& writeEvent = socket->getWriteEvent();
	event_add(&writeEvent, NULL);
}

TUniqueIndex_t CSocketManager::genUniqueIndex()
{
	return ++m_socketSequenceIndex;
}

void CSocketManager::cleanUp()
{
	if (NULL != m_eventbase) {
		event_base_free(m_eventbase);
	}
}

void CSocketManager::OnAccept(TSocketIndex_t fd, short evt, void* arg)
{
	TSocketWrapperEventArg_t* event_arg = (TSocketWrapperEventArg_t*)arg;
	if (NULL == event_arg || NULL == event_arg->s || NULL == event_arg->mgr) {
		log_error("Cast socket arg failed");
		return;
	}
	for (uint32 i = 0; i < ACCEPT_ONCE_NUM; ++i) {
		if (!event_arg->mgr->onAccept(event_arg->s)) {
			break;
		}
	}
}

void CSocketManager::OnWriteEvent(TSocketIndex_t fd, short evt, void* arg)
{
	TSocketEventArg_t* eventArg = (TSocketEventArg_t*)arg;

	if (evt & EV_WRITE) {
		eventArg->mgr->onWrite(eventArg->s);
	} else {
		eventArg->mgr->handleCloseSocket(eventArg->s, true);
	}
}

void CSocketManager::OnReadEvent(TSocketIndex_t fd, short evt, void* arg)
{
	TSocketEventArg_t* eventArg = (TSocketEventArg_t*)arg;

	if (!eventArg->s->isActive()) {
		return;
	}

	if (evt & EV_READ) {
		eventArg->mgr->onRead(eventArg->s);
	} else {
		eventArg->mgr->handleCloseSocket(eventArg->s, false);
	}
}