
#ifndef _GATE_HANDLER_H_
#define _GATE_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class gate_handler : public packet_handler<gate_handler>
{
	typedef packet_handler<gate_handler>	TBaseType_t;
public:
	gate_handler();
	virtual ~gate_handler() override;

public:
	static void Setup();

private:
	virtual service_interface* get_service() const override;

public:
	bool handle_transfer_client(packet_base* packet);
};

#endif // !_GATE_HANDLER_H_

