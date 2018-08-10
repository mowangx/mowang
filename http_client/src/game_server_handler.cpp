
#include "game_server_handler.h"
#include "log.h"
#include "http_client.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

game_server_handler::game_server_handler() : packet_handler<game_server_handler>()
{
	m_rpc_client = new rpc_client(this);
}

game_server_handler::~game_server_handler()
{

}

service_interface* game_server_handler::get_service() const
{
	return singleton<http_client>::get_instance_ptr();
}