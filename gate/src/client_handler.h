
#ifndef _CLIENT_HANDLER_H_
#define _CLIENT_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class client_handler : public packet_handler<client_handler>
{
	typedef packet_handler<client_handler>	TBaseType_t;

public:
	client_handler();
	virtual ~client_handler() override;

private:
	virtual void setup_handlers() override;
	virtual service_interface* get_service() const override;
	virtual bool need_register_server() const override {
		return false;
	}

public:
	virtual void handle_init() override;
	virtual void handle_close() override;

	bool handle_transfer_server_by_index(packet_base* packet);
	bool handle_transfer_server_by_name(packet_base* packet);
};

#endif // !_CLIENT_HANDLER_H_

