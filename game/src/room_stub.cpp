
#include "room_stub.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "room.h"

room_stub::room_stub()
{
	m_room_id = 100;
}

room_stub::~room_stub()
{
}

bool room_stub::init(TServerID_t server_id, TProcessID_t game_id, TEntityID_t entity_id)
{
	log_info("room stub init");
	DRegisterStubRpc(this, room_stub, create_room, 3);
	DRegisterStubRpc(this, room_stub, destroy_room, 1);
	DRegisterStubRpc(this, room_stub, enter_room, 5);
	return TBaseType_t::init(server_id, game_id, entity_id);
}

void room_stub::create_room(const mailbox_info& mailbox, const dynamic_string& pwd, const mailbox_info& role_mailbox)
{
	m_room_id += 1;
	m_room_id_2_mailbox[m_room_id] = mailbox;
	DRpcWrapper.call_entity(role_mailbox, "on_create_room", mailbox.entity_id, m_room_id);
}

void room_stub::destroy_room(TRoomID_t room_id)
{
	auto itr = m_room_id_2_mailbox.find(room_id);
	if (itr == m_room_id_2_mailbox.end()) {
		return;
	}
	mailbox_info& mailbox = itr->second;
	game_process_info process_info;
	process_info.server_id = mailbox.server_id;
	process_info.process_type = PROCESS_GAME;
	process_info.process_id = mailbox.game_id;
	rpc_client* rpc = DRpcWrapper.get_client(process_info);
	rpc->call_remote_func("destroy_entity", mailbox.entity_id);
	m_room_id_2_mailbox.erase(itr);
}

void room_stub::enter_room(TRoomID_t room_id, TRoleID_t role_id, const mailbox_info& mailbox, const proxy_info& proxy, const dynamic_string& pwd)
{
	auto itr = m_room_id_2_mailbox.find(room_id);
	if (itr == m_room_id_2_mailbox.end()) {
		return;
	}

	const mailbox_info& room_mailbox = itr->second;
	DRpcWrapper.call_entity(room_mailbox, "enter_room", role_id, mailbox, proxy);
	DRpcWrapper.call_entity(mailbox, "on_enter_room", room_mailbox);
}
