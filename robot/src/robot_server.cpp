

#include "robot_server.h"
#include "robot_packet_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "game_random.h"
#include "tcp_manager.h"
#include "ws_manager.h"

robot_server::robot_server() : ws_service(PROCESS_ROBOT)
{
}

robot_server::~robot_server()
{
	
}

bool robot_server::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	robot_packet_handler::Setup();

	DRegisterServerRpc(this, robot_server, register_server, 2);
	DRegisterServerRpc(this, robot_server, on_register_entities, 5);
	DRegisterServerRpc(this, robot_server, on_unregister_process, 4);
	DRegisterClientRpc(this, robot_server, logout, 2);

	return true;
}

void robot_server::init_ws_process_func()
{
	m_cmd_2_parse_func["login"] = std::bind(&robot_server::process_login, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["response"] = std::bind(&robot_server::process_response, this, std::placeholders::_1, std::placeholders::_2);
}

void robot_server::net_run()
{
	loop_run([this](TGameTime_t diff) -> bool {
		DNetMgr.update(diff);
		//DNetMgr.test_kick();

		if (DNetMgr.socket_num() < 2) {
			TPort_t port = DGameRandom.get_rand<TPort_t>(0, 2);
			port = port * 10 + 10100;
			if (!DNetMgr.start_connect<robot_packet_handler>("127.0.0.1", port)) {
				log_info("connect server failed");
				return false;
			}
		}
		return true;
	});
}

void robot_server::ws_run()
{
	TPort_t port = 10101 + DGameRandom.get_rand<int>(0, 2) * 10;
	DWSNetMgr.start_connect("mowang.com", 10101);
	log_info("init websocket manager success");

	loop_run([this](TGameTime_t diff) -> bool {
		DWSNetMgr.update(diff);
		return true;
	});
}

void robot_server::process_ws_init_sockets(std::vector<web_socket_wrapper_base*>& sockets)
{
	for (auto socket : sockets) {
		std::string user_id = gx_to_string("mw_%d", m_server_info.process_info.process_id);
		std::string msg = gx_to_string("{\"cmd\":\"login\", \"platform_id\": 1, \"server_id\": 100, \"code\": \"%s\"}", user_id.c_str());
		push_ws_write_packets(socket->get_socket_index(), msg);
	}
}

void robot_server::process_login(TSocketIndex_t socket_index, boost::property_tree::ptree * json)
{
	int ret_code = json->get<int>("ret_code", 0);
	std::string user_id = json->get<std::string>("code", "").c_str();
	log_debug("parse response!!! socket index %" I64_FMT "u,  ret_code %d, user_id %s", socket_index, ret_code, user_id.c_str());

	push_ws_write_packets(socket_index, "{\"cmd\":\"test\", \"param_1\": 20}");
}

void robot_server::process_response(TSocketIndex_t socket_index, boost::property_tree::ptree* json)
{
	int ret_code = json->get<int>("ret_code", 0);
	std::string user_id = json->get<std::string>("user_id", "").c_str();
	log_debug("parse response!!! socket index %" I64_FMT "u,  ret_code %d, user_id %s", socket_index, ret_code, user_id.c_str());

	push_ws_write_packets(socket_index, "{\"cmd\":\"test\", \"param_1\": 20}");
}

void robot_server::logout(uint8 reason, TSocketIndex_t client_id)
{
	log_info("logout! reason = %u, client id = %" I64_FMT "u", reason, client_id);
	kick_socket(client_id);
}
