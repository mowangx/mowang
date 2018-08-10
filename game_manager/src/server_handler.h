
#ifndef _SERVER_HANDLER_H_
#define _SERVER_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class server_handler : public packet_handler<server_handler>
{
	typedef packet_handler<server_handler>	TBaseType_t;
public:
	server_handler();
	virtual ~server_handler() override;

private:
	virtual service_interface* get_service() const override;
};

#endif // !_SERVER_HANDLER_H_

