
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

void game_packet_handler::Setup()
{
	TBaseType_t::Setup();
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_CLIENT, (packet_handler_func)&game_packet_handler::handle_transfer_client);
}

service_interface * game_packet_handler::get_service() const
{
	return singleton<game_server>::get_instance_ptr();
}

bool game_packet_handler::handle_transfer_client(packet_base * packet)
{
	transfer_client_packet* transfer_info = (transfer_client_packet*)packet;
	TSocketIndex_t client_id = transfer_info->m_client_id;
	TProcessID_t gate_id = (TProcessID_t)((client_id >> 40) & 0xFFFF);
	packet_base* transfer_packet = (packet_base*)transfer_info->m_buffer;
	TPacketID_t packet_id = transfer_packet->get_packet_id();
	log_info("transfer client, gate id %u, client id %" I64_FMT "u, packet id %u", gate_id, client_id, packet_id);
	if (packet_id == PACKET_ID_RPC_BY_NAME) {
		rpc_by_name_packet* rpc_info = (rpc_by_name_packet*)packet;
		TEntityID_t entity_id = DEntityMgr.get_entity_id_by_client_id(client_id);
		DRpcEntity.call(entity_id, rpc_info->m_rpc_name, rpc_info->m_buffer);
	}
	else {
		log_error("transfer client failed, not find packet id, gate id %u, packet id %u", gate_id, packet_id);
	}
	return true;
}
