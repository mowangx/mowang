
#ifndef _GAME_SERVER_HANDLER_H_
#define _GAME_SERVER_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class game_server_handler : public packet_handler<game_server_handler>
{
	typedef packet_handler<game_server_handler>	TBaseType_t;

public:
	game_server_handler();
	virtual ~game_server_handler() override;

public:
	static void Setup();

private:
	virtual service_interface* get_service() const override;

public:
	bool handle_transfer_role(packet_base* packet);
	bool handle_transfer_stub(packet_base* packet);
	bool handle_transfer_client(packet_base* packet);
	bool handle_transfer_ws_client(packet_base* packet);
};

#endif // !_GAME_SERVER_HANDLER_H_

