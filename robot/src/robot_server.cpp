

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

	DRegisterServerRpc(this, robot_server, register_server, 2);
	DRegisterServerRpc(this, robot_server, on_register_entities, 5);
	DRegisterServerRpc(this, robot_server, on_unregister_process, 4);
	DRegisterServerRpc(this, robot_server, create_role, 1);
	DRegisterClientRpc(this, robot_server, logout, 2);

	return true;
}

void robot_server::init_ws_process_func()
{
	m_cmd_2_parse_func["login"] = std::bind(&robot_server::process_login, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["logout"] = std::bind(&robot_server::process_logout, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["create_role"] = std::bind(&robot_server::process_create_role, this, std::placeholders::_1, std::placeholders::_2);
}

void robot_server::net_run()
{
	loop_run([this](TGameTime_t diff) -> bool {
		DNetMgr.update(diff);
		//DNetMgr.test_kick();

		if (DNetMgr.socket_num() < 2) {
			TPort_t port = DGameRandom.get_rand<TPort_t>(0, 2);
			port = 10100;
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
	//TPort_t port = 10101 + DGameRandom.get_rand<int>(0, 2) * 10;
	//DWSNetMgr.start_connect("localhost", 10101);
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
		std::string msg = gx_to_string("{\"cmd\":\"login\", \"platform_id\": 1, \"server_id\": 100, \"user_id\": \"%s\", \"token\": \"token369\"}", user_id.c_str());
		push_ws_write_packets(socket->get_socket_index(), msg);
	}
}

void robot_server::process_login(TSocketIndex_t socket_index, boost::property_tree::ptree * json)
{
	int ret_code = json->get<int>("ret_code", 0);
	if (ret_code == 1) {
		TAccountID_t account_id = json->get<TAccountID_t>("account_id", 0);
		TRoleID_t role_id = json->get<TRoleID_t>("role_id", 0);
		TSex_t sex = json->get<TSex_t>("sex", 0);
		TLevel_t level = json->get<TLevel_t>("level", 0);
		std::string role_name = json->get<std::string>("role_name", "").c_str();
		log_debug("login success!!! socket index %" I64_FMT "u, account id %" I64_FMT "u, role id %" I64_FMT "u, sex %d, level %d, role name %s", 
			socket_index, account_id, role_id, sex, level, role_name.c_str());
	}
	else {
		log_debug("login failed!!! ret code %d", ret_code);
	}
}

void robot_server::process_logout(TSocketIndex_t socket_index, boost::property_tree::ptree* json)
{
	std::string reason = json->get<std::string>("reason", "").c_str();
	log_info("client logout! reason: %s", reason.c_str());
	kick_socket(socket_index);
}

void robot_server::process_create_role(TSocketIndex_t socket_index, boost::property_tree::ptree* json)
{
	int ret_code = json->get<int>("ret_code", 0);
	std::string user_id = json->get<std::string>("user_id", "").c_str();
	log_debug("parse response!!! socket index %" I64_FMT "u,  ret_code %d, user_id %s", socket_index, ret_code, user_id.c_str());

	TSex_t sex = 2;
	std::string role_name("mowang");
	std::string msg = gx_to_string("{\"cmd\":\"create_role\", \"sex\": %d, \"role_name\": \"%s\"}", sex, role_name.data()).c_str();
	push_ws_write_packets(socket_index, msg);
}

void robot_server::create_role(TSocketIndex_t socket_index)
{
	rpc_client* rpc = DRpcWrapper.get_client_by_socket_index(socket_index);
	if (NULL == rpc) {
		return;
	}
	TSex_t sex = 1;
	dynamic_string role_name("mowang");
	rpc->call_remote_func("create_role", sex, role_name);
}

void robot_server::logout(uint8 reason, TSocketIndex_t client_id)
{
	log_info("logout! reason = %u, client id = %" I64_FMT "u", reason, client_id);
	kick_socket(client_id);
}