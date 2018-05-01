
#ifndef _DB_MANAGER_HANDLER_H_
#define _DB_MANAGER_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class db_manager_handler : public packet_handler<db_manager_handler>
{
	typedef packet_handler<db_manager_handler>	TBaseType_t;
public:
	db_manager_handler();
	~db_manager_handler();

private:
	virtual service_interface* get_service() const override;
};

#endif // !_DB_MANAGER_HANDLER_H_

