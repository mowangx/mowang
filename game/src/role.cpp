
#include "role.h"
#include "game_server.h"
#include "city.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "string_common.h"
#include "sequence.h"

role::role()
{
	clean_up();
	m_cities.clear();
}

role::~role()
{
	log_info("role deconstructor! entity id = %" I64_FMT "u", get_entity_id());
	clean_up();

	for (auto c : m_cities) {
		DGameServer.deallocate_city(c);
	}
	m_cities.clear();
}

bool role::init()
{
	DRegisterRoleRpc(m_mailbox_info.entity_id, this, role, on_register_account, 4);
	DRegisterRoleRpc(m_mailbox_info.entity_id, this, role, on_register_role, 4);
	DRegisterRoleRpc(m_mailbox_info.entity_id, this, role, on_relay_logout, 0);
	return true;
}

void role::login(TPlatformID_t platform_id, const TUserID_t& user_id)
{
	log_info("role login, entity id = %" I64_FMT "u", get_entity_id());
	set_platform_id(platform_id);
	set_user_id(user_id);
	DRpcWrapper.call_stub("roll_stub", "register_account", get_platform_id(), get_user_id(), get_proxy_info(), get_mailbox_info(), get_test_client_id());
}

void role::logout()
{
	log_info("role logout, entity id = %" I64_FMT "u", get_entity_id());
	destroy();
	logout_core(true);
}

void role::logout_core(bool need_unregister_flag)
{
	destroy();
	
	if (!need_unregister_flag) {
		return;
	}

	log_info("role logout, send unregister account, entity id = %" I64_FMT "u", get_entity_id());
	if (get_login_success()) {
		save();
		DRpcWrapper.call_stub("roll_stub", "unregister_role", get_role_id());
	}
	DRpcWrapper.call_stub("roll_stub", "unregister_account", get_platform_id(), get_user_id());
}

void role::on_register_account(bool status, const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id)
{
	// what will happen if call the function more than once ?
	if (status) {
		log_info("on register account success! entity id = %" I64_FMT "u", get_entity_id());
		DGameServer.db_query(
			"user",
			gx_to_string("user_id = %" I64_FMT "u", get_user_id().data()).c_str(),
			"role_id",
			std::bind(&role::on_load_account_callback, this, std::placeholders::_1, std::placeholders::_2));
	}
	else {
		log_info("on register account success, but has registered! entity id = %" I64_FMT "u", get_entity_id());
		on_relay_success(proxy, mailbox, test_client_id);
	}
}

void role::on_register_role(bool status, const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id)
{
	if (status) {
		log_info("role on register role success! entity id = %" I64_FMT "u", get_entity_id());
		DGameServer.db_query(
			"role", 
			gx_to_string("%" I64_FMT " u", get_role_id()).c_str(), 
			"*", 
			std::bind(&role::on_load_role_callback, this, std::placeholders::_1, std::placeholders::_2));
	}
	else {
		log_info("role on register role success, but has registered! entity id = %" I64_FMT "u", get_entity_id());
		on_relay_success(proxy, mailbox, test_client_id);
	}
}

void role::on_relay_logout()
{
	log_info("role on relay logout! entity id = %" I64_FMT "u", get_entity_id());
	logout_core(false);
}

void role::create_role()
{
	set_role_id(((TRoleID_t)m_mailbox_info.server_id << 48) + DSequence.gen_sequence_id(SEQUENCE_ROLE));
	DGameServer.db_insert("user", 
		gx_to_string("platform_id = %u, user_id = %s, role_id = %" I64_FMT "u", get_platform_id(), get_user_id().data(), get_role_id()).c_str(),
		[&](bool status) {
		if (status) {
			log_info("create role success! client id = %" I64_FMT "u, role id = %" I64_FMT "u", get_client_id(), get_role_id());
			on_account_login_success();
		}
		else {
			log_error("create role failed for save failed! client id = %" I64_FMT "u, role id = %" I64_FMT "u", get_client_id(), get_role_id());
		}
	});
}

void role::on_load_account_callback(bool status, const dynamic_string& result)
{
	if (get_destroy_flag()) {
		log_info("role will be destroy soon! client id = %" I64_FMT "u", get_client_id());
		return;
	}
	if (!status) {
		log_error("load from db failed! client id = %" I64_FMT "u", get_client_id());
		return;
	}
	if (result.empty()) {
		create_role();
	}
	else {
		m_role_id = 1;
		on_account_login_success();
	}
}

void role::on_load_role_callback(bool status, const dynamic_string& result)
{
	// init role data from db result
	on_role_login_success();
}

void role::on_relay_success(const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id)
{
	log_info("role on relay success! cur entity id = %" I64_FMT "u, kick entity id = %" I64_FMT "u", get_entity_id(), mailbox.entity_id);
	DRpcWrapper.call_client(get_proxy_info(), "logout", (uint8)LOGOUT_RELAY, get_test_client_id());
	game_process_info process_info;
	process_info.server_id = get_server_id();
	process_info.process_type = PROCESS_GATE;
	process_info.process_id = get_gate_id();
	rpc_client* rpc = DRpcWrapper.get_client(process_info);
	if (NULL != rpc) {
		rpc->call_remote_func("kick_socket_delay", get_client_id());
	}

	set_test_client_id(test_client_id);

	if (mailbox.game_id == get_game_id()) {
		role* p = DGameServer.get_role_by_client_id(proxy.client_id);
		if (NULL != p) {
			p->on_relay_logout();
		}
		else {
			log_error("find role by client id failed! client id = %" I64_FMT "u", proxy.client_id);
		}
	}
	else {
		DRpcWrapper.call_role(mailbox, "on_relay_logout");
	}
	DGameServer.update_role_proxy_info(get_proxy_info(), proxy);
	m_proxy_info = proxy;

	process_info.process_id = get_gate_id();
	rpc = DRpcWrapper.get_client(process_info);
	if (NULL != rpc) {
		process_info.process_type = PROCESS_GAME;
		process_info.process_id = get_game_id();
		rpc->call_remote_func("update_process_info", get_client_id(), process_info);
	}
	else {
		log_error("role update process info failed for rpc is NULL! client id = %" I64_FMT "u, gate id = %u", get_client_id(), get_gate_id());
	}

	if (get_login_success()) {
		on_role_login_success();
	}
}

void role::on_account_login_success()
{
	log_info("on account login success! entity id = %" I64_FMT "u", get_entity_id());
	DRpcWrapper.call_stub("roll_stub", "register_role", get_platform_id(), get_user_id(),  get_role_id(), get_proxy_info(), get_mailbox_info(), get_test_client_id());
}

void role::on_role_login_success()
{
	log_info("on role login success! entity id = %" I64_FMT "u", get_entity_id());
	set_login_success(true);
	// recv fight stub notify fight result first, then start load from fight data from db
	DGameServer.db_query(
		"fight",
		gx_to_string("%" I64_FMT " u", get_role_id()).c_str(),
		"*",
		[&](bool status, const dynamic_string& result) {

	});
}

void role::fight(TNpcIndex_t npc_id, dynamic_array<soldier_info>& soldiers, const game_pos& src_pos, const game_pos& dest_pos)
{
	city* c = get_city(src_pos);
	if (NULL == c) {
		return;
	}
	if (c->check_fight(npc_id, soldiers, dest_pos)) {
		DRpcWrapper.call_stub("fight_stub", "fight", get_role_id(), npc_id, c->get_pos(), dest_pos,
			soldiers, c->calc_move_time(npc_id, dest_pos));
	}
}

void role::gather(TNpcIndex_t npc_id, dynamic_array<soldier_info>& soldiers, const game_pos& src_pos, const game_pos& dest_pos)
{
	city* c = get_city(src_pos);
	if (NULL == c) {
		return;
	}
	c->gather(npc_id, soldiers, dest_pos);
}

void role::add_city(const game_pos& pos, TLevel_t lvl)
{
	city* c = DGameServer.allocate_city();
	c->set_pos(pos);
	c->random_resources(lvl);
	m_cities.push_back(c);
}

void role::del_city(const game_pos& pos)
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

city* role::get_city(const game_pos& pos)
{
	for (auto itr = m_cities.begin(); itr != m_cities.end(); ++itr) {
		city* c = *itr;
		if (c->get_pos() != pos) {
			continue;
		}
		return c;
	}

	return NULL;
}

void role::set_login_success(bool login_flag)
{
	m_login_success = login_flag;
}

bool role::get_login_success() const
{
	return m_login_success;
}

void role::set_destroy_flag(bool destroy_flag)
{
	m_destroy_flag = destroy_flag;
}

bool role::get_destroy_flag() const
{
	return m_destroy_flag;
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

void role::set_test_client_id(TSocketIndex_t client_id)
{
	m_test_client_id = client_id;
}

TSocketIndex_t role::get_test_client_id() const
{
	return m_test_client_id;
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

void role::set_platform_id(TPlatformID_t platform_id)
{
	m_platform_id = platform_id;
}

TPlatformID_t role::get_platform_id() const
{
	return m_platform_id;
}

void role::set_user_id(const TUserID_t & user_id)
{
	m_user_id = user_id;
}

const TUserID_t & role::get_user_id() const
{
	return m_user_id;
}

const proxy_info & role::get_proxy_info() const
{
	return m_proxy_info;
}

const mailbox_info & role::get_mailbox_info() const
{
	return m_mailbox_info;
}

void role::save()
{
}

void role::destroy()
{
	set_destroy_flag(true);
	DGameServer.remove_entity(get_client_id());
}

void role::clean_up()
{
	m_login_success = false;
	m_destroy_flag = false;
	m_proxy_info.clean_up();
	m_mailbox_info.clean_up();
	m_role_id = INVALID_ROLE_ID;
	m_honor = INVALID_HONOR_NUM;
	m_rmb = INVALID_RMB_NUM;
}