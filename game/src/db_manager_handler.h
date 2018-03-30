
#ifndef _DB_MANAGER_HANDLER_H_
#define _DB_MANAGER_HANDLER_H_

#include "packet_handler.h"

class rpc_client;

class db_manager_handler : public packet_handler<db_manager_handler>
{
	typedef packet_handler<db_manager_handler>	TBaseType_t;
public:
	db_manager_handler();
	~db_manager_handler();

public:
	static void Setup();

private:
	virtual TPacketInfo_t* create_packet_info() override;
	virtual char* create_packet(int n) override;
	virtual void write_packet(TPacketInfo_t* packet_info) override;

public:
	virtual void handle_close() override;

private:
	rpc_client* m_rpc_client;
};

#endif // !_DB_MANAGER_HANDLER_H_

