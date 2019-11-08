
#ifndef _GATE_PACKET_HANDLER_H_
#define _GATE_PACKET_HANDLER_H_

#include "packet_handler.h"

class gate_packet_handler : public packet_handler<gate_packet_handler>
{
	typedef packet_handler<gate_packet_handler>	TBaseType_t;
public:
	gate_packet_handler();
	virtual ~gate_packet_handler() override;

private:
	virtual void setup_handlers() override;
	virtual service_interface* get_service() const override;

public:
	bool handle_transfer_packet(packet_base* packet);
	bool handle_transfer_ws_client(packet_base* packet);
};

#endif // !_GATE_PACKET_HANDLER_H_

