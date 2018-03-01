
#include "socket_manager.h"
#include "socket.h"
#include "socket_api.h"
#include "log.h"

CSocketManager::CSocketManager()
{
	m_socketSequenceIndex = 0;
	m_eventbase = NULL;
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

	TSocketEvent_t& listen_event = socket->getReadEvent();
	TSocketEventArg& event_arg = socket->getEventArg();
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

bool CSocketManager::start_connect(const char* host, TPort_t port)
{
	CSocket* socket = new CSocket();
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

	addSocket(socket);

	return true;
}

uint32 CSocketManager::socket_num() const
{
	return m_sockets.size();
}

bool CSocketManager::onAccept(CSocket* listener)
{
	CSocket* socket = listener->accept();
	if (NULL == socket) {
		return false;
	}

	// @todo 检测添加的时候会不会影响性能
	TUniqueIndex_t index = genUniqueIndex();
	socket->setUniqueIndex(index);
	log_info("Accept socket! index = '%"I64_FMT"u'", index);

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
	char buffer[120];
	int len = socket->read(buffer, 119);
	buffer[len] = '\0';
	if (len > 0) {
		log_info("socket index = %"I64_FMT"u, recv msg: %s", socket->getUniqueIndex(), buffer);
	}
}

void CSocketManager::handleWriteMsg(TUniqueIndex_t index, char* msg, uint32 len)
{
	auto itr = m_sockets.find(index);
	if (itr == m_sockets.end()) {
		log_error("can't find socket index, index = '%"I64_FMT"u'", index);
		return;
	}
	CSocket* socket = itr->second;
	socket->write(msg, len);
	TSocketEvent_t& writeEvent = socket->getWriteEvent();
	event_add(&writeEvent, NULL);
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

void CSocketManager::addSocket(CSocket* socket)
{
	TSocketEvent_t& readEvent = socket->getReadEvent();
	TSocketEvent_t& writeEvent = socket->getWriteEvent();
	TSocketEventArg& eventArg = socket->getEventArg();
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
	m_sockets.erase(socket->getUniqueIndex());

	DSafeDelete(socket);
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
	TSocketEventArg* event_arg = (TSocketEventArg*)arg;
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
	TSocketEventArg* eventArg = (TSocketEventArg*)arg;

	if (evt & EV_WRITE) {
		eventArg->mgr->onWrite(eventArg->s);
	} else {
		eventArg->mgr->handleCloseSocket(eventArg->s, true);
	}
}

void CSocketManager::OnReadEvent(TSocketIndex_t fd, short evt, void* arg)
{
	TSocketEventArg* eventArg = (TSocketEventArg*)arg;

	if (!eventArg->s->isActive()) {
		return;
	}

	if (evt & EV_READ) {
		eventArg->mgr->onRead(eventArg->s);
	} else {
		eventArg->mgr->handleCloseSocket(eventArg->s, false);
	}
}