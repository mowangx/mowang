
#include "db_server.h"
#include "mysql_conn.h"
#include "game_manager_handler.h"
#include "game_server_handler.h"
#include "socket_manager.h"
#include "rpc_proxy.h"

db_server::db_server() : service(PROCESS_DB)
{
	m_db = NULL;
}

db_server::~db_server()
{
	if (NULL != m_db) {
		delete m_db;
		m_db = NULL;
	}
}

bool db_server::init(TProcessID_t process_id)
{
	//m_db = new CMysqlConn();
	//if (NULL == m_db) {
	//	return false;
	//}

	//return m_db->init("127.0.0.1", 3306, "root", "123456", "test");

	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	m_server_info.process_info.server_id = 100;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10100 + process_id;

	game_server_handler::Setup();
	game_manager_handler::Setup();

	DRegisterServerRpc(this, db_server, register_server, 2);

	connect_game_manager_loop("127.0.0.1", 10000);

	return true;
}

bool db_server::connect_game_manager(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<game_manager_handler>(ip, port);
}
