
#include "lbs_stub.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "game_server.h"

lbs_stub::lbs_stub()
{
	clean_up();
}

lbs_stub::~lbs_stub()
{
	clean_up();
}

bool lbs_stub::init()
{
	log_info("lbs stub init");
	DRegisterStubRpc(this, lbs_stub, query_city, 3);
	DRegisterStubRpc(this, lbs_stub, update_city, 2);

	DGameServer.db_query("lbs", NULL, "owner_id, pos_x, pos_y", [&](bool status, const binary_data& result) {
		if (!status) {
			log_error("load lbs info from db failed!");
			return;
		}
	});

	return true;
}

void lbs_stub::query_city(const game_pos& pos, const mailbox_info& mailbox, const dynamic_string& callback)
{
	uint32 index = ((uint32)pos.x << 16) + pos.y;
	auto itr = m_pos_2_role.find(index);
	TRoleID_t role_id = INVALID_ROLE_ID;
	if (itr != m_pos_2_role.end()) {
		role_id = itr->second;
	}
	DRpcWrapper.call_role(mailbox, callback.data(), role_id);
}

void lbs_stub::update_city(const game_pos& pos, TRoleID_t role_id)
{
	uint32 index = ((uint32)pos.x << 16) + pos.y;
	m_pos_2_role[index] = role_id;
}

void lbs_stub::clean_up()
{
	m_pos_2_role.clear();
}
