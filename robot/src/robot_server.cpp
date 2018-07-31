

#include "robot_server.h"
#include "gate_handler.h"
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

	gate_handler::Setup();

	DRegisterClientRpc(this, robot_server, logout, 2);

	return true;
}

void robot_server::init_ws_process_func()
{
	m_cmd_2_parse_func["response"] = std::bind(&robot_server::process_response, this, std::placeholders::_1, std::placeholders::_2);
}

void robot_server::net_run()
{
	log_info("init socket manager success");
	while (true) {
		DNetMgr.update(0);
		//DNetMgr.test_kick();

		if (DNetMgr.socket_num() < 2) {
			if (!DNetMgr.start_connect<gate_handler>("127.0.0.1", DGameRandom.get_rand<int>(10010, 10012))) {
				log_info("connect server failed");
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void robot_server::ws_run()
{
	int random_port = DGameRandom.get_rand<int>(0, 10000);
	std::cout << random_port << std::endl;
	TPort_t port = 10101 + (random_port % 3) * 10;
	std::string url = gx_to_string("ws://127.0.0.1:%d", port);
	std::cout << url << std::endl;
	DWSNetMgr.start_connect(url);
	log_info("init websocket manager success");

	while (true) {
		DWSNetMgr.update(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}
void robot_server::process_ws_init_sockets(std::vector<web_socket_wrapper_base*>& sockets)
{
	for (auto socket : sockets) {
		std::string user_id = gx_to_string("mw_%d", m_server_info.process_info.process_id);
		std::string msg = gx_to_string("{\"cmd\":\"login\", \"platform_id\": 1, \"server_id\": 100, \"user_id\": \"%s\"}", user_id.c_str());
		push_ws_write_packets(socket->get_socket_index(), msg);
	}
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
