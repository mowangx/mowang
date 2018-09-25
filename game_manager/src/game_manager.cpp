
#include "game_manager.h"
#include "server_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "game_random.h"
#include "tcp_manager.h"

game_manager::game_manager() : service(PROCESS_GAME_MANAGER)
{
	m_broadcast_flag = false;
	for (int i = 0; i < MAX_PROCESS_TYPE_NUM; ++i) {
		m_process_num[i] = 0;
	}
	m_stub_name_2_process_id.clear();
}

game_manager::~game_manager()
{

}

bool game_manager::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	server_handler::Setup();

	DRegisterServerRpc(this, game_manager, register_server, 2);
	DRegisterServerRpc(this, game_manager, create_entity, 5);
	DRegisterServerRpc(this, game_manager, register_entity, 3);

	if (!DNetMgr.start_listen<server_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	log_info("init socket manager success");
	
	return true;
}

bool game_manager::check_all_process_start() const
{
	for (int i = 0; i < MAX_PROCESS_TYPE_NUM; ++i) {
		if (m_process_num[i] < m_config.get_desire_process_num(i)) {
			return false;
		}
	}
	return true;
}

void game_manager::broadcast_dbs() const
{
	dynamic_array<game_server_info> servers;
	DRpcWrapper.get_server_infos(m_server_info.process_info.server_id, PROCESS_DB, servers);
	broadcast_core(servers, PROCESS_DB, PROCESS_GAME);
}

void game_manager::broadcast_db(const game_server_info& server_info) const
{
	dynamic_array<game_server_info> servers;
	servers.push_back(server_info);
	broadcast_core(servers, PROCESS_DB, PROCESS_GAME);
}

void game_manager::broadcast_games() const
{
	dynamic_array<game_server_info> servers;
	DRpcWrapper.get_server_infos(m_server_info.process_info.server_id, PROCESS_GAME, servers);
	broadcast_core(servers, PROCESS_GAME, PROCESS_GATE);
}

void game_manager::broadcast_game(const game_server_info& server_info) const
{
	dynamic_array<game_server_info> servers;
	servers.push_back(server_info);
	broadcast_core(servers, PROCESS_GAME, PROCESS_GATE);
}

void game_manager::broadcast_http_clients() const
{
	dynamic_array<game_server_info> servers;
	DRpcWrapper.get_server_infos(m_server_info.process_info.server_id, PROCESS_HTTP_CLIENT, servers);
	broadcast_core(servers, PROCESS_HTTP_CLIENT, PROCESS_GAME);
}

void game_manager::broadcast_http_client(const game_server_info & server_info) const
{
	dynamic_array<game_server_info> servers;
	servers.push_back(server_info);
	broadcast_core(servers, PROCESS_HTTP_CLIENT, PROCESS_GAME);
}

void game_manager::broadcast_core(const dynamic_array<game_server_info>& servers, game_process_type src_type, game_process_type dest_type) const
{
	dynamic_array<game_server_info> game_servers;
	DRpcWrapper.get_server_infos(m_server_info.process_info.server_id, dest_type, game_servers);
	for (int i = 0; i < game_servers.size(); ++i) {
		const game_server_info& server_info = game_servers[i];
		rpc_client* rpc = DRpcWrapper.get_client(server_info.process_info);
		if (NULL != rpc) {
			rpc->call_remote_func("on_register_servers", m_server_info.process_info.server_id, (TProcessType_t)src_type, servers);
		}
	}
}

void game_manager::unicast_to_game(const game_process_info& process_info) const
{
	rpc_client* rpc = DRpcWrapper.get_client(process_info);
	if (NULL != rpc) {
		std::vector<TProcessType_t> process_types;
		process_types.push_back(PROCESS_DB);
		process_types.push_back(PROCESS_HTTP_CLIENT);
		unicast_core(rpc, process_info.server_id, process_types);

		dynamic_array<game_stub_info> stub_infos;
		DRpcWrapper.get_stub_infos(stub_infos);
		rpc->call_remote_func("on_register_entity", stub_infos);
	}
}

void game_manager::unicast_to_gate(const game_process_info& process_info) const
{
	rpc_client* rpc = DRpcWrapper.get_client(process_info);
	if (NULL != rpc) {
		std::vector<TProcessType_t> process_types;
		process_types.push_back(PROCESS_GAME);
		unicast_core(rpc, process_info.server_id, process_types);
	}
}

void game_manager::unicast_core(rpc_client* rpc, TServerID_t server_id, const std::vector<TProcessType_t>& process_types) const
{
	for (TProcessType_t process_type : process_types) {
		dynamic_array<game_server_info> servers;
		DRpcWrapper.get_server_infos(server_id, process_type, servers);
		rpc->call_remote_func("on_register_servers", server_id, process_type, servers);
	}
}

void game_manager::create_entity(TSocketIndex_t socket_index, TServerID_t server_id, const TEntityName_t& entity_name, TProcessID_t process_id, bool check_repeat)
{
	dynamic_array<game_server_info> game_servers;
	DRpcWrapper.get_server_infos(server_id, PROCESS_GAME, game_servers);
	if (game_servers.empty()) {
		return;
	}

	TProcessID_t random_process_id = DGameRandom.get_rand<int>(0, (int)(game_servers.size() - 1));
	if (check_repeat) {
		std::string stub_name = entity_name.data();
		auto itr = m_stub_name_2_process_id.find(stub_name);
		if (itr != m_stub_name_2_process_id.end() && itr->second != process_id) {
			log_info("create entity repeat! stub name %s has create on game %d", stub_name.c_str(), itr->second);
			return;
		}
		m_stub_name_2_process_id[stub_name] = random_process_id;
	}
	rpc_client* rpc = DRpcWrapper.get_client(game_servers[random_process_id].process_info);
	rpc->call_remote_func("create_entity", entity_name);
}

void game_manager::register_entity(TSocketIndex_t socket_index, const TEntityName_t& entity_name, const game_process_info& process_info)
{
	dynamic_array<game_stub_info> stub_infos;
	game_stub_info stub_info;
	stub_info.stub_name = entity_name;
	stub_info.process_info = process_info;
	stub_infos.push_back(stub_info);
	TBaseType_t::on_register_entity(socket_index, stub_infos);
	dynamic_array<game_server_info> game_servers;
	DRpcWrapper.get_server_infos(process_info.server_id, PROCESS_GAME, game_servers);
	for (int i = 0; i < game_servers.size(); ++i) {
		const game_server_info& server_info = game_servers[i];
		rpc_client* rpc = DRpcWrapper.get_client(server_info.process_info);
		if (NULL != rpc) {
			rpc->call_remote_func("on_register_entity", stub_infos);
		}
	}
}

void game_manager::on_connect(TSocketIndex_t socket_index)
{
	game_process_info process_info;
	DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index);
	m_process_num[process_info.process_type] += 1;
	log_info("on connect, process type = %d, process num = %d", 
		process_info.process_type, m_process_num[process_info.process_type]);
	if (m_broadcast_flag) {
		game_server_info server_info;
		DRpcWrapper.get_server_info(process_info, server_info);
		if (process_info.process_type == PROCESS_GAME) {
			broadcast_game(server_info);
			unicast_to_game(process_info);
		}
		else if (process_info.process_type == PROCESS_DB) {
			broadcast_db(server_info);
		}
		else if (process_info.process_type == PROCESS_GATE) {
			unicast_to_gate(process_info);
		}
		else if (process_info.process_type == PROCESS_HTTP_CLIENT) {
			broadcast_http_client(server_info);
		}
	}
	else if (check_all_process_start()) {
		m_broadcast_flag = true;
		broadcast_dbs();
		broadcast_games();
		broadcast_http_clients();
		log_info("all process are start! broadcast server infos");
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
