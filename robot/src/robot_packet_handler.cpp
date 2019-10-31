
#include "robot_packet_handler.h"
#include "log.h"
#include "robot_server.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "timer.h"

robot_packet_handler::robot_packet_handler() : packet_handler<robot_packet_handler>()
{

}

robot_packet_handler::~robot_packet_handler()
{

}

service_interface * robot_packet_handler::get_service() const
{
	return singleton<robot_server>::get_instance_ptr();
}

void robot_packet_handler::handle_init()
{
	log_info("%" I64_FMT "u, gate connect success, handle init", get_socket_index());
	TBaseType_t::handle_init();
	DTimer.add_timer(2, false, nullptr, [&](void* param, TTimerID_t timer_id) {
		rpc_client* rpc = DRpcWrapper.get_client_by_socket_index(get_socket_index());
		if (NULL == rpc) {
			return;
		}
		TServerID_t server_id = 100;
		account_info account_data;
		account_data.platform_id = 99;
		memset(account_data.user_id.data(), 0, USER_ID_LEN);
		memcpy(account_data.user_id.data(), "xiedi", 5);
		memset(account_data.token.data(), 0, TOKEN_LEN);
		memcpy(account_data.user_id.data(), "token test", 10);
		rpc->call_remote_func("login_server", server_id, account_data);
	});
	
}