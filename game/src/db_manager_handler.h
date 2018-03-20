
#ifndef _DB_MANAGER_HANDLER_H_
#define _DB_MANAGER_HANDLER_H_

#include "packet_handler.h"

class CDbManagerHandler : public CPacketHandler<CDbManagerHandler>
{
public:
	CDbManagerHandler();
	~CDbManagerHandler();

public:
	static void Setup();

private:
	virtual TPacketInfo_t* create_packet_info() override;
	virtual char* create_packet(int n) override;
	virtual void write_packet(TPacketInfo_t* packet_info) override;

public:
	bool handle_test_1(CBasePacket* packet);
	bool handle_test_2(CBasePacket* packet);
};

#endif // !_DB_MANAGER_HANDLER_H_

