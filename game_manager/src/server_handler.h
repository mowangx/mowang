
#ifndef _SERVER_HANDLER_H_
#define _SERVER_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class server_handler : public packet_handler<server_handler>
{
	typedef packet_handler<server_handler>	TBaseType_t;
public:
	server_handler();
	~server_handler();

public:
	static void Setup();

private:
	virtual TPacketSendInfo_t* create_packet_info() override;
	virtual char* create_packet(int n) override;
	virtual void write_packet(TPacketSendInfo_t* packet_info) override;

public:
	virtual void handle_close() override;

	bool handle_server_info(packet_base* packet);

private:
	rpc_client * m_rpc_client;
};

#endif // !_SERVER_HANDLER_H_

