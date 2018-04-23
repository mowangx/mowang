
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

public:
	static void Setup();

private:
	virtual TPacketSendInfo_t* create_packet_info() override;
	virtual char* create_packet(int n) override;
	virtual void write_packet(TPacketSendInfo_t* packet_info) override;

public:
	virtual void handle_init() override;
	virtual void handle_close() override;

private:
	rpc_client* m_rpc_client;
};

#endif // !_GAME_MANAGER_HANDLER_H_
