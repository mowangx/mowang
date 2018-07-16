
#include "gate_server.h"
#include "game_manager_handler.h"
#include "game_server_handler.h"
#include "client_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "socket_manager.h"
#include "time_manager.h"

gate_server::gate_server() : service(PROCESS_GATE)
{
	m_ws_list_port = 0;
	m_delay_kick_sockets.clear();
	m_client_2_process.clear();
}

gate_server::~gate_server()
{

}

bool gate_server::load_config(ini_file& ini, const std::string & module_name)
{
	if (!ini.read_type_if_exist(module_name.c_str(), "ws_port", m_ws_list_port)) {
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
	DRegisterServerRpc(this, gate_server, on_register_servers, 4);
	DRegisterServerRpc(this, gate_server, login_server, 5);
	DRegisterServerRpc(this, gate_server, update_process_info, 3);
	DRegisterServerRpc(this, gate_server, kick_socket_delay, 2);

	game_manager_handler::Setup();
	game_server_handler::Setup();
	client_handler::Setup();

	return true;
}

void gate_server::init_threads()
{
	std::thread log_thread(std::bind(&service::log_run, this));
	std::thread net_thread(std::bind(&service::net_run, this));
	std::thread net_ws_thread(std::bind(&gate_server::ws_net_run, this));

	work_run();

	log_thread.join();
	net_thread.join();
}

void gate_server::work_run()
{
	connect_game_manager_loop(m_config.get_game_manager_listen_ip(), m_config.get_game_manager_listen_port());
	TBaseType_t::work_run();
}

void gate_server::net_run()
{
	if (!DNetMgr.start_listen<client_handler>(m_server_info.port)) {
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
	game_process_info process_info;
	if (!DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index) ||
		process_info.process_type != PROCESS_GAME) {
		return;
	}

	for (auto itr = m_client_2_process.begin(); itr != m_client_2_process.end(); ++itr) {
		const game_process_info& tmp_process_info = itr->second;
		if (tmp_process_info.process_id == process_info.process_id &&
			tmp_process_info.server_id == process_info.server_id) {
			kick_socket(itr->first);
		}
	}
}

void gate_server::ws_net_run()
{
	WebsocketServer server;
	server.set_access_channels(websocketpp::log::alevel::all);
	server.clear_access_channels(websocketpp::log::alevel::frame_payload);

	server.init_asio();

	server.set_open_handler(std::bind(&gate_server::ws_on_open, this, &server, std::placeholders::_1));
	server.set_close_handler(std::bind(&gate_server::ws_on_close, this, &server, std::placeholders::_1));
	server.set_message_handler(std::bind(&gate_server::ws_on_message, this, &server, std::placeholders::_1, std::placeholders::_2));

	server.listen(m_ws_list_port);
	server.start_accept();
	server.run();
}

void gate_server::ws_on_open(WebsocketServer * server, websocketpp::connection_hdl hdl)
{
	std::cout << "have client connected" << std::endl;
}

void gate_server::ws_on_close(WebsocketServer * server, websocketpp::connection_hdl hdl)
{
	std::cout << "have client disconnected" << std::endl;
}

void gate_server::ws_on_message(WebsocketServer * server, websocketpp::connection_hdl hdl, message_ptr msg)
{
	std::string str_msg = msg->get_payload();
	std::cout << str_msg << std::endl;

	std::string str_response = "receive: ";
	str_response.append(str_msg);

	server->send(hdl, str_response, websocketpp::frame::opcode::text);
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
	log_info("login, client id = %" I64_FMT "u, user id = %s, game id = %u", socket_index, user_id.data(), process_info.process_id);
	rpc_client* rpc = DRpcWrapper.get_client(process_info);
	if (NULL != rpc) {
		rpc->call_remote_func("login_server", socket_index, platform_id, user_id, test_client_id);
	}
	else {
		log_error("login failed for rpc is NULL! server id = %u, process id = %u, client id = %" I64_FMT "u", 
			process_info.server_id, process_info.process_id, socket_index);
	}
}

void gate_server::logout_server(TSocketIndex_t socket_index)
{
	log_info("logout, client id = %" I64_FMT "u", socket_index);
	auto itr = m_client_2_process.find(socket_index);
	if (itr != m_client_2_process.end()) {
		const game_process_info& process_info = itr->second;
		rpc_client* rpc = DRpcWrapper.get_client(process_info);
		if (NULL != rpc) {
			rpc->call_remote_func("logout_server", socket_index);
		}
		else {
			log_error("logout failed for rpc is NULL! server id = %u, process id = %u, client id = %" I64_FMT "u", 
				process_info.server_id, process_info.process_id, socket_index);
		}
	}
}

void gate_server::update_process_info(TSocketIndex_t socket_index, TSocketIndex_t client_id, const game_process_info & process_info)
{
	auto itr = m_client_2_process.find(client_id);
	if (itr != m_client_2_process.end()) {
		log_info("update process info success! client id = %" I64_FMT "u", client_id);
		itr->second = process_info;
	}
	else {
		log_error("update process info failed for not find client id! client id = %" I64_FMT "u", client_id);
	}
}

void gate_server::kick_socket_delay(TSocketIndex_t socket_index, TSocketIndex_t client_id)
{
	socket_kick_info kick_info;
	kick_info.socket_index = client_id;
	kick_info.kick_time = DTimeMgr.now_sys_time() + DELAY_KICK_SOCKET_TIME;
	m_delay_kick_sockets.push_back(kick_info);
}

TSocketIndex_t gate_server::get_server_socket_index(TSocketIndex_t socket_index) const
{
	auto itr = m_client_2_process.find(socket_index);
	if (itr != m_client_2_process.end()) {
		return DRpcWrapper.get_socket_index(itr->second);
	}
	return INVALID_SOCKET_INDEX;
}
