
#include "game_manager_handler.h"
#include "log.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "game_enum.h"

game_manager_handler::game_manager_handler() : packet_handler<game_manager_handler>()
{
	m_rpc_client = new rpc_client(this);
}

game_manager_handler::~game_manager_handler()
{

}

service_interface* game_manager_handler::get_service() const
{
	return singleton<game_server>::get_instance_ptr();
}
