
#ifndef _GAME_SERVER_HANDLER_H_
#define _GAME_SERVER_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class game_server_handler : public packet_handler<game_server_handler>
{
	typedef packet_handler<game_server_handler>	TBaseType_t;

public:
	game_server_handler();
	~game_server_handler();

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
	virtual void handle_init() override;
};

#endif // !_GAME_SERVER_HANDLER_H_

