
#ifndef _HTTP_PACKET_HANDLER_H_
#define _HTTP_PACKET_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class http_packet_handler : public packet_handler<http_packet_handler>
{
	typedef packet_handler<http_packet_handler>	TBaseType_t;

public:
	http_packet_handler();
	virtual ~http_packet_handler() override;

private:
	virtual service_interface* get_service() const override;
};

#endif // !_GAME_SERVER_HANDLER_H_

