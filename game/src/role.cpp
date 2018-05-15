
#include "role.h"
#include "game_server.h"
#include "city.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

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
	DRegisterRoleRpc(m_mailbox_info.role_id, this, role, login, 2);
	DRegisterRoleRpc(m_mailbox_info.role_id, this, role, test_func_2, 3);
	DRegisterRoleRpc(m_mailbox_info.role_id, this, role, on_register, 1);
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
	log_info("role login, platform id = %u, user id = %s, role id = %"I64_FMT"u, gate id = %u", platform_id, user_id.data(), m_mailbox_info.role_id, m_proxy_info.gate_id);
	m_platform_id = platform_id;
	m_user_id = user_id;
	std::array<char, 33> p2_2;
	memset(p2_2.data(), 0, 33);
	memcpy(p2_2.data(), "mowang", 6);
	DRpcWrapper.call_client(get_proxy_info(), "robot_rpc_func_2", m_proxy_info.gate_id, m_proxy_info.client_id, m_mailbox_info.role_id, p2_2);
}

void role::test_func_2(const dynamic_string& s1, TServerID_t server_id, const dynamic_string& s2)
{
	log_info("role test func 2, server id = %u, s1 = %s, s2 = %s, role id = %"I64_FMT"u, gate id = %u", server_id, s1.data(), s2.data(), m_mailbox_info.role_id, m_proxy_info.gate_id);

	dynamic_string p1("xiedi");
	std::array<char, 127> p3;
	memset(p3.data(), 0, 127);
	memcpy(p3.data(), "hello world", 11);
	DRpcWrapper.call_client(get_proxy_info(), "robot_rpc_func_1", m_proxy_info.gate_id, m_proxy_info.client_id, p1, m_mailbox_info.role_id, p3, (TSocketIndex_t)0);
}

void role::on_register(bool status)
{
	if (status) {
		log_info("on register success, role id = '%"I64_FMT"u'", m_mailbox_info.role_id);
	}
	else {
		log_info("on register failed, role id = '%"I64_FMT"u'", m_mailbox_info.role_id);
	}
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

void role::set_gate_id(TProcessID_t gate_id)
{
	m_proxy_info.gate_id = gate_id;
}

void role::set_game_id(TProcessID_t game_id)
{
	m_mailbox_info.game_id = game_id;
}

void role::set_client_id(TSocketIndex_t client_id)
{
	m_proxy_info.client_id = client_id;
}

void role::set_role_id(TRoleID_t role_id)
{
	m_mailbox_info.role_id = role_id;
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