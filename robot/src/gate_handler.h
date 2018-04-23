
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

public:
	static void Setup();

private:
	virtual TPacketInfo_t* create_packet_info() override;
	virtual char* create_packet(int n) override;
	virtual void write_packet(TPacketInfo_t* packet_info) override;

public:
	virtual void handle_init() override;
	virtual void handle_close() override;

private:
	rpc_client * m_rpc_client;
};


#endif // !_GATE_HANDLER_H_

