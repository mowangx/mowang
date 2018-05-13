

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

	DRegisterClientRpc(this, robot_server, robot_rpc_func_1, 6);
	DRegisterClientRpc(this, robot_server, robot_rpc_func_2, 4);

	return true;
}

void robot_server::robot_rpc_func_1(TProcessID_t process_id, TSocketIndex_t socket_index, const dynamic_string & p1, TRoleID_t role_id, const std::array<char, 127>& p3, TSocketIndex_t client_id)
{
	log_info("robot rpc func 1, gate id = %u, role id = %"I64_FMT"u, p1 = %s, p3 = %s, socket index = %"I64_FMT"u", 
		process_id, role_id, p1.data(), p3.data(), socket_index);
	TPlatformID_t platform_id = 88;
	TUserID_t user_id;
	memset(user_id.data(), 0, USER_ID_LEN);
	memcpy(user_id.data(), "mowang", 6);
	if (client_id > 0) {
		TSocketIndex_t key_id = process_id;
		key_id = (key_id << 48) + socket_index;
		m_sockets[key_id] = client_id;
	}
	rpc_client* rpc = get_robot_client(process_id, socket_index);
	if (NULL != rpc) {
		rpc->call_server("login", platform_id, user_id);
	}
	else {
		log_error("not find socket index = '%"I64_FMT"u'", socket_index);
	}
}

void robot_server::robot_rpc_func_2(TProcessID_t process_id, TSocketIndex_t socket_index, TRoleID_t role_id, const std::array<char, 33>& p2)
{
	log_info("robot rpc func 2, gate id = %u, role id = %"I64_FMT"u, p2 = %s, socket index = %"I64_FMT"u", 
		process_id, role_id, p2.data(), socket_index);
	dynamic_string p2_1("xiedi");
	TServerID_t p2_2 = 65500;
	dynamic_string p2_3("hello world");
	rpc_client* rpc = get_robot_client(process_id, socket_index);
	if (NULL != rpc) {
		rpc->call_server("test_func_2", p2_1, p2_2, p2_3);
	}
	else {
		log_error("not find socket index = '%"I64_FMT"u'", socket_index);
	}
}

rpc_client * robot_server::get_robot_client(TProcessID_t process_id, TSocketIndex_t socket_index)
{
	TSocketIndex_t key_id = process_id;
	key_id = (key_id << 48) + socket_index;
	auto itr = m_sockets.find(key_id);
	if (itr != m_sockets.end()) {
		return get_client(itr->second);
	}
	return NULL;
}
