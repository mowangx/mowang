
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
	virtual TPacketSendInfo_t* create_packet_info() override;
	virtual char* create_packet(int n) override;
	virtual void write_packet(TPacketSendInfo_t* packet_info) override;
	virtual const game_server_info& get_server_info() const override;
	virtual void register_client() override;
	virtual void unregister_client() override;

public:
	bool handle_transfer_server_by_index(packet_base* packet);
	bool handle_transfer_server_by_name(packet_base* packet);
};

#endif // !_CLIENT_HANDLER_H_

