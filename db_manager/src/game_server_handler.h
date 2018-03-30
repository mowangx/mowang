
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
	virtual TPacketInfo_t* create_packet_info() override;
	virtual char* create_packet(int n) override;
	virtual void write_packet(TPacketInfo_t* packet_info) override;

public:
	virtual void handle_init() override;
	virtual void handle_close() override;

	bool handle_rpc_by_index(packet_base* packet);
	bool handle_rpc_by_name(packet_base* packet);

private:
	rpc_client * m_rpc_client;
};

#endif // !_GAME_SERVER_HANDLER_H_

