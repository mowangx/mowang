
#include "game_packet_handler.h"
#include "log.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "game_server.h"
#include "entity_manager.h"

game_packet_handler::game_packet_handler() : packet_handler<game_packet_handler>()
{
}

game_packet_handler::~game_packet_handler()
{
}

void game_packet_handler::setup_handlers()
{
	TBaseType_t::setup_handlers();
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_PAKCET, (packet_handler_func)&game_packet_handler::handle_transfer_packet);
}

service_interface * game_packet_handler::get_service() const
{
	return singleton<game_server>::get_instance_ptr();
}

bool game_packet_handler::handle_transfer_packet(packet_base * packet)
{
	transfer_entity_packet* transfer_info = (transfer_entity_packet*)packet;
	TSocketIndex_t client_id = transfer_info->m_client_id;
	TProcessID_t gate_id = (TProcessID_t)((client_id >> 40) & 0xFFFF);
	packet_base* transfer_packet = (packet_base*)transfer_info->m_buffer;
	TPacketID_t packet_id = transfer_packet->get_packet_id();
	//log_info("transfer client, gate id %u, client id %" I64_FMT "u, packet id %u", gate_id, client_id, packet_id);
	if (packet_id == PACKET_ID_RPC_BY_NAME) {
		rpc_by_name_packet* rpc_info = (rpc_by_name_packet*)transfer_packet;
		TEntityID_t entity_id = DEntityMgr.get_entity_id_by_client_id(client_id);
		if (entity_id != INVALID_ENTITY_ID) {
			DRpcEntity.call(entity_id, rpc_info->m_rpc_name, rpc_info->m_buffer);
		}
		else {
			std::string func_name = rpc_info->m_rpc_name;
			if (func_name.find("login") != std::string::npos) {
				TProcessID_t gate_id = (TProcessID_t)((client_id >> 40) & 0xFFFF);
				account* p = (account*)DEntityMgr.create_entity("account", gate_id, client_id);

				int buffer_index = 0;
				TPlatformID_t platform_id = INVALID_PLATFORM_ID;
				rpc_param_parse<TPlatformID_t, TPlatformID_t>::parse_param(platform_id, rpc_info->m_buffer, buffer_index);
				dynamic_string user_id;
				rpc_param_parse<dynamic_string, dynamic_string>::parse_param(user_id, rpc_info->m_buffer, buffer_index);
				dynamic_string token;
				rpc_param_parse<dynamic_string, dynamic_string>::parse_param(token, rpc_info->m_buffer, buffer_index);
				p->login(platform_id, user_id, token);
				//log_info("login server, client id %" I64_FMT "u, gate id %u, entity id %" I64_FMT "u, platform id %u, user id %s, token %s",
				//	client_id, gate_id, p->get_entity_id(), platform_id, user_id.data(), token.data());
			}
		}
	}
	else {
		log_error("transfer client failed, not find packet id, gate id %u, packet id %u", gate_id, packet_id);
	}
	return true;
}
