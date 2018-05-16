
#include "role.h"
#include "game_server.h"
#include "city.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "string_common.h"

role::role()
{
	clean_up();
	m_cities.clear();
}

role::~role()
{
	clean_up();

	for (auto c : m_cities) {
		DGameServer.deallocate_city(c);
	}
	m_cities.clear();
}

bool role::init()
{
	DRegisterRoleRpc(m_mailbox_info.entity_id, this, role, login, 2);
	DRegisterRoleRpc(m_mailbox_info.entity_id, this, role, on_register_callback, 3);
	DRegisterRoleRpc(m_mailbox_info.entity_id, this, role, on_relay_ready, 1);
	return true;
}

void role::update(TGameTime_t diff)
{
	for (auto c : m_cities) {
		c->update(diff);
	}
}

void role::login(TPlatformID_t platform_id, const TUserID_t & user_id)
{
	log_info("role login, platform id = %u, user id = %s, entity id = %" I64_FMT "u, gate id = %u", platform_id, user_id.data(), get_entity_id(), get_gate_id());
	m_platform_id = platform_id;
	m_user_id = user_id;
	DRpcWrapper.call_stub("roll_stub", "register_role", get_role_id(), get_proxy_info(), get_mailbox_info());
}

void role::logout()
{
	log_info("role logout, role id = %" I64_FMT "u, gate id = %u", get_role_id(), get_gate_id());
}

void role::on_register_callback(bool status, const proxy_info& proxy, const mailbox_info& mailbox)
{
	if (status) {
		std::string query = gx_to_string("%" I64_FMT " u", get_role_id());
		DGameServer.db_query("role", query.c_str(), "*", [](bool status, const dynamic_string_array& result) {
			// init role data from db result
		});
	}
	else {
		if (mailbox != m_mailbox_info) {
			DRpcWrapper.call_client(get_proxy_info(), "logout", (uint8)LOGOUT_RELAY);
		}
		DGameServer.update_role_process_info(m_proxy_info, proxy, mailbox);
		m_proxy_info = proxy;
		m_mailbox_info = mailbox;
	}
}

void role::on_relay_ready(const proxy_info & proxy)
{
}

void role::add_city(const game_pos & pos, TLevel_t lvl)
{
	city* c = DGameServer.allocate_city();
	c->set_pos(pos);
	c->random_resources(lvl);
	m_cities.push_back(c);
}

void role::del_city(const game_pos & pos)
{
	for (auto itr = m_cities.begin(); itr != m_cities.end(); ++itr) {
		city* c = *itr;
		if (c->get_pos() != pos) {
			continue;
		}
		DGameServer.deallocate_city(c);
		m_cities.erase(itr);
		return;
	}
}

TLevel_t role::get_level() const
{
	return m_lvl;
}

void role::add_level(TLevel_t lvl)
{
	m_lvl += lvl;
}

THonorNum_t role::get_honor() const
{
	return m_honor;
}

void role::add_honor(THonorNum_t num)
{
	m_honor += num;
}

TRmbNum_t role::get_rmb() const
{
	return m_rmb;
}

void role::add_rmb(TRmbNum_t num)
{
	m_rmb += num;
	if (m_rmb < 0) {
		m_rmb = 0;
	}
}

void role::set_server_id(TServerID_t server_id)
{
	m_proxy_info.server_id = server_id;
	m_mailbox_info.server_id = server_id;
}

TServerID_t role::get_server_id() const
{
	return m_mailbox_info.server_id;
}

void role::set_gate_id(TProcessID_t gate_id)
{
	m_proxy_info.gate_id = gate_id;
}

TProcessID_t role::get_gate_id() const
{
	return m_proxy_info.gate_id;
}

void role::set_game_id(TProcessID_t game_id)
{
	m_mailbox_info.game_id = game_id;
}

TProcessID_t role::get_game_id() const
{
	return m_mailbox_info.game_id;
}

void role::set_client_id(TSocketIndex_t client_id)
{
	m_proxy_info.client_id = client_id;
}

TSocketIndex_t role::get_client_id() const
{
	return m_proxy_info.client_id;
}

void role::set_entity_id(TEntityID_t entity_id)
{
	m_mailbox_info.entity_id = entity_id;
}

TEntityID_t role::get_entity_id() const
{
	return m_mailbox_info.entity_id;
}

void role::set_role_id(TRoleID_t role_id)
{
	m_role_id = role_id;
}

TRoleID_t role::get_role_id() const
{
	return m_role_id;
}

const proxy_info & role::get_proxy_info() const
{
	return m_proxy_info;
}

const mailbox_info & role::get_mailbox_info() const
{
	return m_mailbox_info;
}

void role::clean_up()
{
	m_proxy_info.clean_up();
	m_mailbox_info.clean_up();
	m_honor = INVALID_HONOR_NUM;
	m_rmb = INVALID_RMB_NUM;
}