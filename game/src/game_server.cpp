
#include "game_server.h"
#include "log.h"
#include "socket_manager.h"
#include "socket.h"

#include "tbl_test.h"

CGameServer::CGameServer()
{
	m_socketMgr = NULL;
}

CGameServer::~CGameServer()
{
	if (NULL != m_socketMgr) {
		delete m_socketMgr;
		m_socketMgr = NULL;
	}
}

bool CGameServer::init()
{
	m_socketMgr = new CSocketManager();
	if (NULL == m_socketMgr) {
		return false;
	}

	if (!m_socketMgr->init()) {
		return false;
	}
	
	if (!m_socketMgr->start_listen(10000)) {
		return false;
	}
	log_info("init socket manager success");

	if (!DTblTestMgr.load("../config/server_test.xml")) {
		log_error("load config failed");
		return false;
	}
	log_info("load config success");

	return true;
}

void CGameServer::run()
{
	while (true) {

	}
}