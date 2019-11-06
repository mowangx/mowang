#ifndef _MYSQL_CONN_H_
#define _MYSQL_CONN_H_

#include <cppconn/resultset.h>

#include "db_conn.h"
#include "mysql_connection.h"

class mysql_conn : public db_conn
{
public:
	mysql_conn();
	~mysql_conn();

public:
	virtual bool init(const char* ip, uint16 port, const char* user, const char* pwd, const char* db_name) override;

	virtual bool remove(const char* table, const char* query) override;
	virtual bool insert(const char* table, const char* fields, const char* values) override;
	virtual bool update(const char* table, const char* query, const char* fields) override;
	virtual bool query(const char* table, const char* query, const char* fields, char* result, int& len) override;

private:
	void fill_result(sql::ResultSet* res, uint32 column_index, int column_type, char* result, int& len);

private:
	sql::Driver* m_driver;
	sql::Connection* m_conn;
};

#endif // !_MYSQL_CONN_H_
