
#include "role.h"
#include "game_server.h"
#include "city.h"

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

void role::login(TPlatformID_t platform_id, const TUserID_t & user_id)
{
	m_platform_id = platform_id;
	m_user_id = user_id;
}

void role::update(TGameTime_t diff)
{
	for (auto c : m_cities) {
		c->update(diff);
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

void role::clean_up()
{
	m_proxy_info.clean_up();
	m_mailbox_info.clean_up();
	m_honor = INVALID_HONOR_NUM;
	m_rmb = INVALID_RMB_NUM;
}