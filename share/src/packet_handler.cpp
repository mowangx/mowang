
#include "packet_handler.h"

#include "socket.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

game_handler::game_handler() : m_socket_index(INVALID_SOCKET_INDEX)
{

}

game_handler::~game_handler()
{

}

void game_handler::kick() const
{
	get_service()->kick_socket(get_socket_index());
}

void game_handler::handle_init()
{
	log_info("connect success, handle init, socket index %" I64_FMT "u", m_socket_index);
	setup_handlers();
	get_service()->register_handler(this);
	rpc_client* rpc = DRpcWrapper.get_client_by_socket_index(m_socket_index);
	if (need_register_server() && NULL != rpc) {
		rpc->call_remote_func("register_server", get_service()->get_server_info());
	}
}

void game_handler::handle_close()
{
	log_info("disconnect, handle close, socket index %" I64_FMT "u", m_socket_index);
	get_service()->unregister_handler(m_socket_index);
	if (need_register_server()) {
		DRpcWrapper.unregister_handler_info(m_socket_index);
	}
	m_socket_index = INVALID_SOCKET_INDEX;
}

bool game_handler::handle_rpc_by_index(packet_base* packet) const
{
	rpc_by_index_packet* rpc_info = (rpc_by_index_packet*)packet;
	DRpcStub.call_with_index(rpc_info->m_rpc_index, rpc_info->m_buffer, get_socket_index());
	return true;
}

bool game_handler::handle_rpc_by_name(packet_base* packet) const
{
	rpc_by_name_packet* rpc_info = (rpc_by_name_packet*)packet;
	DRpcStub.call_with_index(rpc_info->m_rpc_name, rpc_info->m_buffer, get_socket_index());
	return true;
}

bool game_handler::handle_entity_rpc_by_index(packet_base * packet) const
{
	entity_rpc_by_index_packet* rpc_info = (entity_rpc_by_index_packet*)packet;
	DRpcEntity.call(rpc_info->m_entity_id, rpc_info->m_rpc_index, rpc_info->m_buffer);
	return true;
}

bool game_handler::handle_entity_rpc_by_name(packet_base * packet) const
{
	entity_rpc_by_name_packet* rpc_info = (entity_rpc_by_name_packet*)packet;
	DRpcEntity.call(rpc_info->m_entity_id, rpc_info->m_rpc_name, rpc_info->m_buffer);
	return true;
}

void game_handler::set_socket_index(TSocketIndex_t socket_index)
{
	m_socket_index = socket_index;
}

TSocketIndex_t game_handler::get_socket_index() const
{
	return m_socket_index;
}

bool game_handler::is_valid() const
{
	return INVALID_SOCKET_INDEX != m_socket_index;
}