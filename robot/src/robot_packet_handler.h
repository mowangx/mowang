
#ifndef _GATE_HANDLER_H_
#define _GATE_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class robot_packet_handler : public packet_handler<robot_packet_handler>
{
	typedef packet_handler<robot_packet_handler>	TBaseType_t;

public:
	robot_packet_handler();
	virtual ~robot_packet_handler() override;

private:
	virtual service_interface* get_service() const override;
	virtual bool need_register_server() const override {
		return false; 
	}

public:
	virtual void handle_init() override;
};


#endif // !_GATE_HANDLER_H_

