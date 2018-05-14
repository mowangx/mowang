
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

private:
	virtual service_interface* get_service() const override;
};

#endif // !_GAME_SERVER_HANDLER_H_

