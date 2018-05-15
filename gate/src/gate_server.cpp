
#include "gate_server.h"
#include "game_manager_handler.h"
#include "game_server_handler.h"
#include "client_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "socket_manager.h"

gate_server::gate_server() : service(PROCESS_GATE)
{
	m_write_packets.clear();
}

gate_server::~gate_server()
{

}

bool gate_server::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	m_server_info.process_info.server_id = 100;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10300 + process_id;

	DRegisterServerRpc(this, gate_server, register_server, 2);
	DRegisterServerRpc(this, gate_server, on_register_servers, 4);
	DRegisterServerRpc(this, gate_server, login_server, 5);

	game_manager_handler::Setup();
	game_server_handler::Setup();
	client_handler::Setup();

	connect_game_manager_loop("127.0.0.1", 10000);

	return true;
}

void gate_server::net_run(TProcessID_t process_id)
{
	TPort_t listen_port = 10300 + process_id;
	if (!DNetMgr.start_listen<client_handler>(listen_port)) {
		return;
	}

	log_info("init socket manager success");

	while (true) {
		DNetMgr.update(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

bool gate_server::connect_game_manager(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<game_manager_handler>(ip, port);
}

void gate_server::on_register_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers)
{
	log_info("on_register_servers, server id = %d, process type = %d, server size = %u", server_id, process_type, servers.size());
	for (int i = 0; i < servers.size(); ++i) {
		const game_server_info& server_info = servers[i];

		game_server_info tmp_server_info;
		if (DRpcWrapper.get_server_info(server_info.process_info, tmp_server_info)) {
			log_info("server has registerted, ip = %s, port = %d", server_info.ip.data(), server_info.port);
			continue;
		}

		if (DNetMgr.start_connect<game_server_handler>(server_info.ip.data(), server_info.port)) {
			log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
		else {
			log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
	}
}

void gate_server::login_server(TSocketIndex_t socket_index, TPlatformID_t platform_id, TServerID_t server_id, const TUserID_t& user_id, TSocketIndex_t test_client_id)
{
	game_process_info process_info;
	process_info.server_id = server_id;
	process_info.process_type = PROCESS_GAME;
	process_info.process_id = DRpcWrapper.get_random_process_id(process_info.server_id, process_info.process_type);
	m_client_2_process[socket_index] = process_info;
	log_info("login server, client id = '%"I64_FMT"u', user id = %s, game id = %u", socket_index, user_id.data(), process_info.process_id);
	rpc_client* rpc = DRpcWrapper.get_client(process_info);
	if (NULL != rpc) {
		rpc->call_remote_func("login_server", socket_index, platform_id, user_id, test_client_id);
	}
}

TSocketIndex_t gate_server::get_server_socket_index(TSocketIndex_t socket_index) const
{
	auto itr = m_client_2_process.find(socket_index);
	if (itr != m_client_2_process.end()) {
		return DRpcWrapper.get_socket_index(itr->second);
	}
	return INVALID_SOCKET_INDEX;
}
