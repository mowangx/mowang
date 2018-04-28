
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
	virtual const game_server_info& get_server_info() const override;
	virtual void register_client() override;
	virtual void unregister_client() override;
};

#endif // !_SERVER_HANDLER_H_

