
#include "game_manager.h"
#include "server_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

game_manager::game_manager() : service(PROCESS_GAME_MANAGER)
{
	for (int i = 0; i < MAX_PROCESS_TYPE_NUM; ++i) {
		m_process_num[i] = 0;
	}
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
	
	return true;
}

bool game_manager::check_all_process_start() const
{
	if (m_process_num[PROCESS_DB] < 2) {
		return false;
	}
	if (m_process_num[PROCESS_GAME] < 3) {
		return false;
	}
	if (m_process_num[PROCESS_GATE] < 3) {
		return false;
	}
	return true;
}

void game_manager::broadcast_dbs() const
{
	dynamic_array<game_server_info> db_servers;
	DRpcWrapper.get_server_infos(m_server_info.process_info.server_id, PROCESS_DB, db_servers);

	dynamic_array<game_server_info> game_servers;
	DRpcWrapper.get_server_infos(m_server_info.process_info.server_id, PROCESS_GAME, game_servers);
	for (int i = 0; i < game_servers.size(); ++i) {
		const game_server_info& server_info = game_servers[i];
		rpc_client* rpc = DRpcWrapper.get_client(server_info.process_info);
		if (NULL != rpc) {
			rpc->call_remote_func("on_query_servers", m_server_info.process_info.server_id, (TProcessType_t)PROCESS_DB, db_servers);
		}
	}
}

void game_manager::broadcast_db(const game_server_info & server_info) const
{
}

void game_manager::broadcast_games() const
{
	dynamic_array<game_server_info> game_servers;
	DRpcWrapper.get_server_infos(m_server_info.process_info.server_id, PROCESS_GAME, game_servers);

	dynamic_array<game_server_info> gate_servers;
	DRpcWrapper.get_server_infos(m_server_info.process_info.server_id, PROCESS_GATE, gate_servers);
	for (int i = 0; i < gate_servers.size(); ++i) {
		const game_server_info& server_info = gate_servers[i];
		rpc_client* rpc = DRpcWrapper.get_client(server_info.process_info);
		if (NULL != rpc) {
			rpc->call_remote_func("on_query_servers", m_server_info.process_info.server_id, (TProcessType_t)PROCESS_GAME, game_servers);
		}
	}
}

void game_manager::broadcast_game(const game_server_info & server_info) const
{
}

void game_manager::on_connect(TSocketIndex_t socket_index)
{
	game_process_info process_info;
	DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index);
	m_process_num[process_info.process_type] += 1;
	log_info("on connect, process type = %d, process num = %d", 
		process_info.process_type, m_process_num[process_info.process_type]);
	if (check_all_process_start()) {
		broadcast_dbs();
		broadcast_games();
	}
}

void game_manager::on_disconnect(TSocketIndex_t socket_index)
{
	game_process_info process_info;
	DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index);
	m_process_num[process_info.process_type] -= 1;
	log_info("on disconnect, process type = %d, process num = %d", 
		process_info.process_type, m_process_num[process_info.process_type]);
}
