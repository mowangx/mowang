
#ifndef _DB_CONN_H_
#define _DB_CONN_H_

#include "base_util.h"

class db_conn
{
public:
	virtual bool init(const char* ip, uint16 port, const char* user, const char* pwd, const char* db_name) = 0;

	virtual bool remove(const char* table, const char* query) = 0;
	virtual bool insert(const char* table, const char* fields, const char* values) = 0;
	virtual bool update(const char* table, const char* query, const char* fields) = 0;
	virtual bool query(const char* table, const char* query, const char* fields, char* result, int& len) = 0;
};

#endif
