
#include "mysql_conn.h"

#include <string>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "log.h"

mysql_conn::mysql_conn()
{
	m_driver = NULL;
	m_conn = NULL;
}

mysql_conn::~mysql_conn()
{
	if (NULL != m_conn) {
		delete m_conn;
		m_conn = NULL;
	}
}

bool mysql_conn::init(const char* ip, uint16 port, const char* user, const char* pwd, const char* db_name)
{
	m_driver = get_driver_instance();
	
	char db_host[1024];
	memset(db_host, 0, 1024);
	sprintf(db_host, "%s:%d", ip, port);
	m_conn = m_driver->connect(db_host, user, pwd);
	if (NULL == m_conn) {
		return false;
	}

	m_conn->setSchema(db_name);
	return true;
}

bool mysql_conn::remove(const char* table, const char* query)
{
	char sql[2048];
	memset(sql, 0, 2048);
	if (NULL == query || strlen(query) < 1) {
		sprintf(sql, "delete from %s", table);
	}
	else {
		sprintf(sql, "delete from %s where %s", table, query);
	}
	return true;
}

bool mysql_conn::insert(const char* table, const char* fields)
{
	if (NULL == fields || strlen(fields) < 1) {
		return false;
	}

	char sql[2048];
	memset(sql, 0, 2048);
	sprintf(sql, "insert into %s %s", table, fields);
	return true;
}

bool mysql_conn::update(const char* table, const char* query, const char* fields)
{
	char sql[2048];
	memset(sql, 0, 2048);
	if (NULL == query || strlen(query) < 1) {
		sprintf(sql, "update %s %s", table, fields);
	}
	else {
		sprintf(sql, "update %s %s where %s", table, fields, query);
	}
	return true;
}

bool mysql_conn::query(const char* table, const char* query, const char* fields)
{
	char sql[2048];
	memset(sql, 0, 2048);
	if (NULL == query || strlen(query) < 1) {
		if (NULL == fields || strlen(fields) < 1) {
			sprintf(sql, "select * from %s", table);
		}
		else {
			sprintf(sql, "select %s from %s", fields, table);
		}
	}
	else {
		if (NULL == fields || strlen(fields) < 1) {
			sprintf(sql, "select * from %s where %s", table, query);
		}
		else {
			sprintf(sql, "select %s from %s where %s", fields, table, query);
		}
	}

	try {
		sql::PreparedStatement* pstmt = m_conn->prepareStatement(sql);
		sql::ResultSet* res = pstmt->executeQuery();
		res->afterLast();
		while (res->previous()) {
			log_info("load from db, %d, %s, %d", res->getInt("id"), res->getString("name").c_str(), res->getInt("sex"));
		}
		delete res;
		delete pstmt;
		return true;
	}
	catch (sql::SQLException &e) {
		log_error("mysql query failed! error code = %d, error msg = %s, state = %s, sql = %s", e.getErrorCode(), e.what(), e.getSQLState().c_str(), sql);
	}
	return false;
}