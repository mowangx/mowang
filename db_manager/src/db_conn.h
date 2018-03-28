
#ifndef _DB_CONN_H_
#define _DB_CONN_H_

#include "base_util.h"

class db_conn
{
public:
	virtual bool init(const char* ip, uint16 port, const char* user, const char* pwd, const char* db_name) = 0;

	virtual void remove(const char* table, const char* query) = 0;
	virtual void insert(const char* table, const char* fields) = 0;
	virtual void update(const char* table, const char* query, const char* fields) = 0;
	virtual void query(const char* table, const char* query, const char* fields) = 0;
};

#endif
