
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
		CSocketManager::OnAccept, &event_arg))
	{
		log_warning("can't event assign!");
		return false;
	}

	if (0 != event_add(&listen_event, NULL))
	{
		log_warning("can't event add!errno=%s", strerror(errno));
		return false;
	}

	return true;
}

void CSocketManager::update(uint32 diff)
{
	if (-1 == event_base_loop(m_eventbase, EVLOOP_ONCE | EVLOOP_NONBLOCK))
	{
		return ;
	}
}

bool CSocketManager::onAccept(CSocket* listener)
{
	CSocket* socket = listener->accept();
	if (NULL == socket)
	{
		return false;
	}

	log_info("Accept socket!");

	// @todo 检测添加的时候会不会影响性能
	TUniqueIndex_t index = genUniqueIndex();
	socket->setUniqueIndex(index);

	m_sockets[index] = socket;

	m_newSocketQueue.push(socket);

	return true;
}

void CSocketManager::OnAccept(TSocketIndex_t fd, short evt, void* arg)
{
	TSocketEventArg* event_arg = (TSocketEventArg*)arg;
	if (NULL == event_arg || NULL == event_arg->s || NULL == event_arg->mgr) {
		log_error("Cast socket arg failed");
		return;
	}
	for (uint32 i = 0; i < ACCEPT_ONCE_NUM; ++i)
	{
		if (!event_arg->mgr->onAccept(event_arg->s))
		{
			break;
		}
	}
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