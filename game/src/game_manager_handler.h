
#ifndef _GAME_MANAGER_HANDLER_H_
#define _GAME_MANAGER_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class game_manager_handler : public packet_handler<game_manager_handler>
{
	typedef packet_handler<game_manager_handler>	TBaseType_t;
public:
	game_manager_handler();
	~game_manager_handler();

private:
	virtual service_interface* get_service() const override;
};

#endif // !_GAME_MANAGER_HANDLER_H_
