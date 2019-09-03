
#include "db_packet_handler.h"
#include "log.h"
#include "db_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

db_packet_handler::db_packet_handler() : packet_handler<db_packet_handler>()
{
}

db_packet_handler::~db_packet_handler()
{
}

service_interface * db_packet_handler::get_service() const
{
	return singleton<db_server>::get_instance_ptr();
}
