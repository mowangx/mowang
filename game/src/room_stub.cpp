
#include "room_stub.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

room_stub::room_stub()
{
}

room_stub::~room_stub()
{
}

bool room_stub::init()
{
	log_info("room stub init");
	DRegisterStubRpc(this, room_stub, create_room, 2);
	DRegisterStubRpc(this, room_stub, destroy_room, 1);
	DRegisterStubRpc(this, room_stub, enter_room, 3);
	return true;
}

void room_stub::create_room(TRoleID_t role_id, const dynamic_string & pwd)
{
}

void room_stub::destroy_room(TRoomID_t room_id)
{
}

void room_stub::enter_room(TRoleID_t role_id, TRoomID_t room_id, const dynamic_string & pwd)
{
}
