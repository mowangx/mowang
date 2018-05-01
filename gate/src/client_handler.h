
#ifndef _CLIENT_HANDLER_H_
#define _CLIENT_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class client_handler : public packet_handler<client_handler>
{
	typedef packet_handler<client_handler>	TBaseType_t;

public:
	client_handler();
	~client_handler();

public:
	static void Setup();

private:
	virtual service_interface* get_service() const override;

public:
	bool handle_transfer_server_by_index(packet_base* packet);
	bool handle_transfer_server_by_name(packet_base* packet);
};

#endif // !_CLIENT_HANDLER_H_

