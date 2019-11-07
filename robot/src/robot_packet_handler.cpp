
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
		TPlatformID_t platform_id = 99;
		dynamic_string user_id("xiedi");
		dynamic_string token("token123");
		rpc->call_remote_func("login", platform_id, user_id, token);
	});
	
}