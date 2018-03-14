#ifndef _MYSQL_CONN_H_
#define _MYSQL_CONN_H_

#include "db_conn.h"
#include "mysql_connection.h"

class CMysqlConn : public CDbConn
{
public:
	CMysqlConn();
	~CMysqlConn();

public:
	virtual bool init(const char* ip, uint16 port, const char* user, const char* pwd, const char* db_name) override;

	virtual void remove(const char* table, const char* query) override;
	virtual void insert(const char* table, const char* fields) override;
	virtual void update(const char* table, const char* query, const char* fields) override;
	virtual void query(const char* table, const char* query, const char* fields) override;

private:
	sql::Driver* m_driver;
	sql::Connection* m_conn;
};

#endif // !_MYSQL_CONN_H_
