#ifndef _MYSQL_CONN_H_
#define _MYSQL_CONN_H_

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
	virtual bool insert(const char* table, const char* fields) override;
	virtual bool update(const char* table, const char* query, const char* fields) override;
	virtual bool query(const char* table, const char* query, const char* fields) override;

private:
	sql::Driver* m_driver;
	sql::Connection* m_conn;
};

#endif // !_MYSQL_CONN_H_
