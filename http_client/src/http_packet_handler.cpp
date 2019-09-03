
#include "http_packet_handler.h"
#include "log.h"
#include "http_client.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

http_packet_handler::http_packet_handler() : packet_handler<http_packet_handler>()
{

}

http_packet_handler::~http_packet_handler()
{

}

service_interface* http_packet_handler::get_service() const
{
	return singleton<http_client>::get_instance_ptr();
}