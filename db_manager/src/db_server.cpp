
#include "db_server.h"

#include "mysql_conn.h"
#include "socket_manager.h"
#include "memory_pool.h"

CDbServer::CDbServer()
{
	m_db = NULL;
}

CDbServer::~CDbServer()
{
	if (NULL != m_db) {
		delete m_db;
		m_db = NULL;
	}
}

bool CDbServer::init()
{
	m_db = new CMysqlConn();
	if (NULL == m_db) {
		return false;
	}

	return m_db->init("127.0.0.1", 3306, "root", "123456", "test");
}

void CDbServer::run()
{
	while (true) {
		m_db->query("role", NULL, "id, name, sex");

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}