
#include "db_manager_handler.h"
#include "log.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

db_manager_handler::db_manager_handler() : packet_handler<db_manager_handler>()
{

}

db_manager_handler::~db_manager_handler()
{
	
}

service_interface * db_manager_handler::get_service() const
{
	return singleton<game_server>::get_instance_ptr();
}