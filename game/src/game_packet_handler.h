
#ifndef _GAME_PACKET_HANDLER_H_
#define _GAME_PACKET_HANDLER_H_

#include "packet_handler.h"

class game_packet_handler : public packet_handler<game_packet_handler>
{
	typedef packet_handler<game_packet_handler>	TBaseType_t;
public:
	game_packet_handler();
	virtual ~game_packet_handler() override;

public:
	static void Setup();

private:
	virtual service_interface* get_service() const override;

public:
	// gate ---> game packet
	bool handle_transfer_client(packet_base* packet);
};


#endif // !_GAME_PACKET_HANDLER_H_

