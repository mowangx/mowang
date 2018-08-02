
#include "gate_handler.h"
#include "log.h"
#include "robot_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "game_enum.h"

gate_handler::gate_handler() : packet_handler<gate_handler>()
{
	m_rpc_client = new rpc_client(this);
}

gate_handler::~gate_handler()
{

}

service_interface * gate_handler::get_service() const
{
	return singleton<robot_server>::get_instance_ptr();
}

void gate_handler::handle_init()
{
	log_info("%" I64_FMT "u, gate connect success, handle init", get_socket_index());
	DRobotServer.register_client(m_rpc_client);

	TPlatformID_t platform_id = 99;
	TServerID_t server_id = 100;
	TUserID_t user_id;
	memset(user_id.data(), 0, USER_ID_LEN);
	memcpy(user_id.data(), "xiedi", 5);
	TSocketIndex_t client_id = get_socket_index();
	m_rpc_client->call_remote_func("login_server", platform_id, server_id, user_id, client_id);
}

void gate_handler::handle_close()
{
	log_info("%" I64_FMT "u, gate disconnect, handle close", get_socket_index());
	TBaseType_t::handle_close();
}

bool gate_handler::handle_rpc_by_index(packet_base * packet)const
{
	rpc_by_index_packet* rpc_info = (rpc_by_index_packet*)packet;
	DRpcStub.call(rpc_info->m_rpc_index, rpc_info->m_buffer);
	return true;
}

bool gate_handler::handle_rpc_by_name(packet_base * packet)const
{
	rpc_by_name_packet* rpc_info = (rpc_by_name_packet*)packet;
	DRpcStub.call(rpc_info->m_rpc_name, rpc_info->m_buffer);
	return true;
}
