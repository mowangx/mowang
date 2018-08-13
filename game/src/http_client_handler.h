
#ifndef _HTTP_CLIENT_HANDLER_H_
#define _HTTP_CLIENT_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class http_client_handler : public packet_handler<http_client_handler>
{
	typedef packet_handler<http_client_handler>	TBaseType_t;
public:
	http_client_handler();
	virtual ~http_client_handler() override;

private:
	virtual service_interface* get_service() const override;
};

#endif // !_HTTP_CLIENT_HANDLER_H_

