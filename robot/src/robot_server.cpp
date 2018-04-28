

#include "robot_server.h"
#include "gate_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

robot_server::robot_server() : service(PROCESS_ROBOT)
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

	DRegisterClientRpc(this, robot_server, robot_rpc_func_1, 4);
	DRegisterClientRpc(this, robot_server, robot_rpc_func_2, 3);

	return true;
}

void robot_server::robot_rpc_func_1(TSocketIndex_t socket_index, const dynamic_string & p1, uint16 p2, const std::array<char, 127>& p3)
{
	log_info("robot rpc func 1, p1 = %s, p2 = %d, p3 = %s", p1.data(), p2, p3.data());

	TPlatformID_t platform_id = 88;
	TUserID_t user_id;
	memset(user_id.data(), 0, USER_ID_LEN);
	memcpy(user_id.data(), "mowang", 6);
	rpc_client* rpc = get_client(socket_index);
	if (NULL != rpc) {
		rpc->call_server("login", platform_id, user_id);
	}
}

void robot_server::robot_rpc_func_2(TSocketIndex_t socket_index, uint8 p1, const std::array<char, 33>& p2)
{
	log_info("robot rpc func 2, p1 = %d, p2 = %s", p1, p2.data());
	dynamic_string p2_1("xiedi");
	uint16 p2_2 = 65500;
	std::array<char, 127> p2_3;
	memset(p2_3.data(), 0, 127);
	memcpy(p2_3.data(), "hello world", 11);
	rpc_client* rpc = get_client(socket_index);
	if (NULL != rpc) {
		rpc->call_remote_func("game_rpc_func_1", p2_1, p2_2, p2_3);
	}
}
