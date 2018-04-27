
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
	virtual TPacketSendInfo_t* create_packet_info() override;
	virtual char* create_packet(int n) override;
	virtual void write_packet(TPacketSendInfo_t* packet_info) override;
	virtual const game_server_info& get_server_info() const override;
	virtual void register_client() override;

public:
	bool handle_transfer_client(packet_base* packet);
};

#endif // !_GATE_HANDLER_H_

