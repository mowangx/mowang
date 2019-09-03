
#ifndef _DB_PACKET_HANDLER_H_
#define _DB_PACKET_HANDLER_H_

#include "packet_handler.h"

class db_packet_handler : public packet_handler<db_packet_handler>
{
	typedef packet_handler<db_packet_handler>	TBaseType_t;

public:
	db_packet_handler();
	virtual ~db_packet_handler() override;

private:
	virtual service_interface* get_service() const override;
};

#endif // !_DB_PACKET_HANDLER_H_

