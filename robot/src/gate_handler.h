
#ifndef _GATE_HANDLER_H_
#define _GATE_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class gate_handler : public packet_handler<gate_handler>
{
	typedef packet_handler<gate_handler>	TBaseType_t;

public:
	gate_handler();
	~gate_handler();

private:
	virtual service_interface* get_service() const override;
	virtual bool need_register_server() const override {
		return false; 
	}

public:
	virtual void handle_init() override;
	virtual void handle_close() override;

	virtual bool handle_rpc_by_index(packet_base* packet) const override;
	virtual bool handle_rpc_by_name(packet_base* packet) const override;
};


#endif // !_GATE_HANDLER_H_

