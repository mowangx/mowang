
#include "gate_server.h"
#include "gate_packet_handler.h"
#include "client_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "tcp_manager.h"
#include "ws_manager.h"
#include "time_manager.h"
#include "timer.h"

gate_server::gate_server() : ws_service(PROCESS_GATE)
{
	m_ws_port = 0;
	m_delay_kick_sockets.clear();
	m_client_2_process.clear();
}

gate_server::~gate_server()
{

}

bool gate_server::load_config(ini_file& ini, const std::string & module_name)
{
	if (!ini.read_type_if_exist(module_name.c_str(), "ws_port", m_ws_port)) {
		log_error("load config failed for not find ws port in module %s!", module_name.c_str());
		return false;
	}

	return true;
}

bool gate_server::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	DRegisterServerRpc(this, gate_server, register_server, 2);
	DRegisterServerRpc(this, gate_server, kick_socket_delay, 2);
	DRegisterServerRpc(this, gate_server, on_register_entities, 5);
	DRegisterServerRpc(this, gate_server, on_unregister_process, 4);

	if (!DNetMgr.start_listen<client_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	log_info("init socket manager success");

	DWSNetMgr.start_listen(m_ws_port);

	log_info("init websocket manager success");

	return true;
}

void gate_server::init_ws_process_func()
{
	m_cmd_2_parse_func["login"] = std::bind(&gate_server::process_login, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["create_role"] = std::bind(&gate_server::process_create_role, this, std::placeholders::_1, std::placeholders::_2);
}

void gate_server::do_loop(TGameTime_t diff)
{
	TBaseType_t::do_loop(diff);

	if (m_delay_kick_sockets.empty()) {
		return;
	}

	TGameTime_t cur_time = DTimeMgr.now_sys_time();
	std::vector<socket_kick_info> del_kick_infos;

	for (auto itr = m_delay_kick_sockets.begin(); itr != m_delay_kick_sockets.end(); ++itr) {
		const socket_kick_info& kick_info = *itr;
		if (kick_info.kick_time < cur_time) {
			kick_socket(kick_info.socket_index);
			del_kick_infos.push_back(kick_info);
		}
	}

	if (del_kick_infos.empty()) {
		return;
	}

	for (auto del_itr = del_kick_infos.begin(); del_itr != del_kick_infos.end(); ++del_itr) {
		auto itr = std::find(m_delay_kick_sockets.begin(), m_delay_kick_sockets.end(), *del_itr);
		if (itr != m_delay_kick_sockets.end()) {
			m_delay_kick_sockets.erase(itr);
		}
	}
}

void gate_server::on_disconnect(TSocketIndex_t socket_index)
{
	TBaseType_t::on_disconnect(socket_index);
	//game_process_info process_info;
	//if (!DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index) ||
	//	process_info.process_type != PROCESS_GAME) {
	//	return;
	//}

	//for (auto itr = m_client_2_process.begin(); itr != m_client_2_process.end(); ++itr) {
	//	const game_process_info& tmp_process_info = itr->second;
	//	if (tmp_process_info.process_id == process_info.process_id &&
	//		tmp_process_info.server_id == process_info.server_id) {
	//		kick_socket(itr->first);
	//	}
	//}
}

bool gate_server::connect_server(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<gate_packet_handler>(ip, port);
}

void gate_server::add_process(const game_server_info& server_info)
{
	log_info("on_register_servers, server id = %d, process type = %d, listen ip = %s, listen port = %d",
		server_info.process_info.server_id, server_info.process_info.process_type, server_info.ip.data(), server_info.port);
	if (server_info.process_info.process_type == PROCESS_GAME) {
		if (DNetMgr.start_connect<gate_packet_handler>(server_info.ip.data(), server_info.port)) {
			log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
		else {
			log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
	}
}

void gate_server::on_client_connect(TSocketIndex_t socket_index)
{
	game_process_info process_info;
	process_info.server_id = get_server_id();
	process_info.process_type = PROCESS_GAME;
	process_info.process_id = DRpcWrapper.get_random_process_id(PROCESS_GAME);
	m_client_2_process[socket_index] = process_info;
	log_info("on client connect, client id %" I64_FMT "u, game id %u", socket_index, process_info.process_id);
}

void gate_server::on_client_disconnect(TSocketIndex_t socket_index)
{
	auto itr = m_client_2_process.find(socket_index);
	if (itr != m_client_2_process.end()) {
		DRpcWrapper.call_server(itr->second.process_id, socket_index, "disconnect_client");
		m_client_2_process.erase(itr);
		log_info("on client disconnect remove success, client id %" I64_FMT "u", socket_index);
	}
	else {
		log_info("on client disconnect remove failed for not find client id %" I64_FMT "u", socket_index);
	}
}

void gate_server::kick_socket_delay(TSocketIndex_t socket_index, TSocketIndex_t client_id)
{
	socket_kick_info kick_info;
	kick_info.socket_index = client_id;
	kick_info.kick_time = DTimeMgr.now_sys_time() + DELAY_KICK_SOCKET_TIME;
	m_delay_kick_sockets.push_back(kick_info);
}

void gate_server::process_ws_close_sockets(std::vector<web_socket_wrapper_base*>& sockets)
{
	for (auto socket : sockets) {
		on_client_disconnect(socket->get_socket_index());
	}
}

void gate_server::process_login(TSocketIndex_t socket_index, boost::property_tree::ptree * json)
{
	TServerID_t server_id = json->get<TServerID_t>("server_id", m_server_info.process_info.server_id);
	std::string cur_token = json->get<std::string>("token", "");
	dynamic_string token(cur_token.c_str());
	std::string cur_user_id = json->get<std::string>("user_id", "");
	dynamic_string user_id(cur_user_id.c_str());
	TPlatformID_t platform_id = json->get<TPlatformID_t>("platform_id", 1);
	on_client_connect(socket_index);
	TProcessID_t process_id = get_process_id_by_client_id(socket_index);
	log_info("login, socket index %" I64_FMT "u, user id %s, game id %u", socket_index, user_id.data(), process_id);
	DRpcWrapper.call_server(process_id, socket_index, "login", platform_id, user_id, token);
}

void gate_server::process_create_role(TSocketIndex_t socket_index, boost::property_tree::ptree* json)
{
	TSex_t sex = json->get<TSex_t>("sex", 1);
	std::string role_name = json->get<std::string>("role_name", "");
	dynamic_string name(role_name.c_str());
	DRpcWrapper.call_server(get_process_id_by_client_id(socket_index), socket_index, "create_role", sex, name);
}

TProcessID_t gate_server::get_process_id_by_client_id(TSocketIndex_t client_id) const
{
	auto itr = m_client_2_process.find(client_id);
	if (itr != m_client_2_process.end()) {
		return itr->second.process_id;
	}
	return INVALID_PROCESS_ID;
}
