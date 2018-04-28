
#include "game_manager.h"
#include "log.h"
#include "server_handler.h"
#include "game_enum.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

game_manager::game_manager() : service(PROCESS_GAME_MANAGER)
{
}

game_manager::~game_manager()
{

}

bool game_manager::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	m_server_info.process_info.server_id = 100;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10000;

	server_handler::Setup();

	DRegisterServerRpc(this, game_manager, register_server, 2);
	DRegisterServerRpc(this, game_manager, query_servers, 3);
	
	return true;
}

void game_manager::register_server(TSocketIndex_t socket_index, const game_server_info& server_info)
{
	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		DRpcWrapper.register_handler_info(itr->second, server_info);
	}
	log_info("register handle info, server id = %d, process type = %d, process id = %d, listen ip = %s, port = %d",
		server_info.process_info.server_id, (TProcessType_t)server_info.process_info.process_type, server_info.process_info.process_id, 
		server_info.ip.data(), server_info.port);
}

void game_manager::query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type)
{
	log_info("query servers, server id = %d, process type = %d", server_id, process_type);
	dynamic_array<game_server_info> servers;
	DRpcWrapper.get_server_infos(server_id, process_type, servers);
	rpc_client* rpc = get_client(socket_index);
	if (NULL != rpc) {
		rpc->call_remote_func("on_query_servers", server_id, process_type, servers);
	}
}