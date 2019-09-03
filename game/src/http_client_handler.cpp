
#include "http_client_handler.h"

#include "log.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

http_client_handler::http_client_handler() : packet_handler<http_client_handler>()
{

}

http_client_handler::~http_client_handler()
{

}

service_interface * http_client_handler::get_service() const
{
	return singleton<game_server>::get_instance_ptr();
}
