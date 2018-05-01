
#include "server_handler.h"
#include "log.h"
#include "game_manager.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

server_handler::server_handler() : packet_handler<server_handler>()
{
	m_rpc_client = new rpc_client(this);
}

server_handler::~server_handler()
{

}

service_interface * server_handler::get_service() const
{
	return singleton<game_manager>::get_instance_ptr();
}