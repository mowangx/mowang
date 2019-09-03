
#include "room_stub.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "room.h"

room_stub::room_stub() : server_entity()
{
	m_room_id = 100;
}

room_stub::~room_stub()
{
}

bool room_stub::init(TEntityID_t entity_id)
{
	log_info("room stub init");
	DRegisterStubRpc(this, room_stub, create_room, 3);
	DRegisterStubRpc(this, room_stub, destroy_room, 1);
	DRegisterStubRpc(this, room_stub, enter_room, 5);
	return TBaseType_t::init(entity_id);
}

void room_stub::create_room(const mailbox_info& mailbox, const dynamic_string& pwd, const mailbox_info& role_mailbox)
{
	m_room_id += 1;
	m_room_id_2_mailbox[m_room_id] = mailbox;
	DRpcWrapper.call_entity(role_mailbox, "on_create_room", mailbox.entity_id, m_room_id);
}

void room_stub::destroy_room(TRoomID_t room_id)
{

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
