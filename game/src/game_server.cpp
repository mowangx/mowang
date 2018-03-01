
#include "game_server.h"
#include "log.h"
#include "socket.h"

#include "tbl_test.h"

CGameServer::CGameServer()
{
	
}

CGameServer::~CGameServer()
{
	
}

bool CGameServer::init()
{
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