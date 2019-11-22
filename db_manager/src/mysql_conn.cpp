
#include "mysql_conn.h"

#include <string>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/datatype.h>

#include "log.h"
#include "rpc_param.h"

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
	try {
		sql::PreparedStatement* pstmt = m_conn->prepareStatement(sql);
		pstmt->execute();
		delete pstmt;
	}
	catch (sql::SQLException &e) {
		log_error("mysql delete failed! error code = %d, error msg = %s, state = %s, sql = %s", e.getErrorCode(), e.what(), e.getSQLState().c_str(), sql);
	}
	return true;
}

bool mysql_conn::insert(const char* table, const char* fields, const char* values)
{
	if (NULL == fields || strlen(fields) < 1) {
		return false;
	}

	char sql[2048];
	memset(sql, 0, 2048);
	sprintf(sql, "insert into %s %s values %s", table, fields, values);
	try {
		sql::PreparedStatement* pstmt = m_conn->prepareStatement(sql);
		pstmt->execute();
		delete pstmt;
	}
	catch (sql::SQLException &e) {
		log_error("mysql insert failed! error code = %d, error msg = %s, state = %s, sql = %s", e.getErrorCode(), e.what(), e.getSQLState().c_str(), sql);
	}
	return true;
}

bool mysql_conn::update(const char* table, const char* fields, const char* query)
{
	char sql[2048];
	memset(sql, 0, 2048);
	if (NULL == query || strlen(query) < 1) {
		sprintf(sql, "update %s %s", table, fields);
	}
	else {
		sprintf(sql, "update %s %s where %s", table, fields, query);
	}
	try {
		sql::PreparedStatement* pstmt = m_conn->prepareStatement(sql);
		pstmt->execute();
	}
	catch (sql::SQLException &e) {
		log_error("mysql update failed! error code = %d, error msg = %s, state = %s, sql = %s", e.getErrorCode(), e.what(), e.getSQLState().c_str(), sql);
	}
	return false;
}

bool mysql_conn::query(const char* table, const char* fields, const char* query, char* result, int& len)
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
		if (res->rowsCount() == 0) {
			log_info("load from db success but empty! sql: %s", sql);
			return true;
		}
		rpc_param_fill<uint16, uint16>::fill_param(res->rowsCount(), result, len);
		res->beforeFirst();
		while (res->next()) {
			sql::ResultSetMetaData* res_meta = res->getMetaData();
			uint32 column_count = res_meta->getColumnCount();
			for (uint32 i = 1; i <= column_count; ++i) {
				int column_type = res_meta->getColumnType(i);
				const sql::SQLString& column_name = res_meta->getColumnTypeName(i);
				fill_result(res, i, column_type, result, len);
			}
			//log_info("load from db, %d, %s, %d", res->getInt("id"), res->getString("name").c_str(), res->getInt("sex"));
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

void mysql_conn::fill_result(sql::ResultSet * res, uint32 column_index, int column_type, char* result, int& len)
{
	if (column_type == sql::DataType::TINYINT) {
		rpc_param_fill<uint8, uint8>::fill_param(res->getInt(column_index), result, len);
	}
	else if (column_type == sql::DataType::SMALLINT) {
		rpc_param_fill<uint16, uint16>::fill_param(res->getInt(column_index), result, len);
	}
	else if (column_type == sql::DataType::INTEGER) {
		rpc_param_fill<uint32, uint32>::fill_param(res->getInt(column_index), result, len);
	}
	else if (column_type == sql::DataType::BIGINT) {
		rpc_param_fill<uint64, uint64>::fill_param(res->getInt64(column_index), result, len);
	}
	else if (column_type == sql::DataType::CHAR || column_type == sql::DataType::VARCHAR || column_type == sql::DataType::LONGVARCHAR) {
		dynamic_string s;
		const sql::SQLString& r = res->getString(column_index);
		for (int i = 0; i < r.length(); ++i) {
			s.push_back(r[i]);
		}
		rpc_param_fill<dynamic_string, dynamic_string>::fill_param(s, result, len);
	}
	else {
		log_error("load from db unknow column type %d", column_type);
	}
}
