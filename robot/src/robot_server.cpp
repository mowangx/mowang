

#include "robot_server.h"
#include "gate_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "game_random.h"
#include "socket_manager.h"

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

	DRegisterClientRpc(this, robot_server, logout, 1);

	return true;
}

void robot_server::net_run()
{
	log_info("init socket manager success");
	while (true) {
		DNetMgr.update(0);
		//DNetMgr.test_kick();

		if (DNetMgr.socket_num() < 1000) {
			if (!DNetMgr.start_connect<gate_handler>("127.0.0.1", DGameRandom.get_rand<int>(10010, 10012))) {
				log_info("connect server failed");
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void robot_server::logout(uint8 reason)
{
	log_info("logout! reason = %u", reason);
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
