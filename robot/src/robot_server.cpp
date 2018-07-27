

#include "robot_server.h"
#include "gate_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "game_random.h"
#include "tcp_manager.h"

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

	DRegisterClientRpc(this, robot_server, logout, 2);

	return true;
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

void robot_server::logout(uint8 reason, TSocketIndex_t client_id)
{
	log_info("logout! reason = %u, client id = %" I64_FMT "u", reason, client_id);
	//kick_socket(client_id);
}
