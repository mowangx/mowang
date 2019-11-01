
#include "role.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "string_common.h"
#include "sequence.h"
#include "entity_manager.h"

role::role() : server_entity()
{
	clean_up();
}

role::~role()
{
	log_info("role deconstructor! entity id %" I64_FMT "u", get_entity_id());
	clean_up();
}

bool role::init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id)
{
	if (!TBaseType_t::init(entity_id, gate_id, client_id)) {
		return false;
	}
	DRegisterEntityRpc(get_entity_id(), this, role, on_register_role, 1);
	DRegisterEntityRpc(get_entity_id(), this, role, on_relay_ready, 1);
	DRegisterEntityRpc(get_entity_id(), this, role, on_relay_login, 1);
	return true;
}

void role::on_register_role(bool status)
{
	if (status) {
		log_info("role register success! entity id %" I64_FMT "u ", get_entity_id());
	}
	else {
		logout();
		log_info("role register failed! entity id %" I64_FMT "u ", get_entity_id());
	}
}

void role::on_relay_ready(const mailbox_info& mailbox)
{
	DRpcWrapper.call_entity(mailbox, "on_wait_login");
	std::string msg = gx_to_string("{\"cmd\": \"kick\"}");
	DRpcWrapper.call_ws_client(get_proxy(), msg);
	log_info("role on relay ready! entity id %" I64_FMT "u", get_entity_id());
}

void role::on_relay_login(const proxy_info & proxy)
{
	update_proxy(proxy);
	log_info("role on relay login success! entity id %" I64_FMT "u", get_entity_id());
}

void role::register_role()
{
	log_info("register role, entity id %" I64_FMT "u", get_entity_id());
	DRpcWrapper.call_stub("roll_stub", "register_role", get_role_id(), get_mailbox());
}

void role::logout()
{
	log_info("role logout, entity id %" I64_FMT "u", get_entity_id());
	destroy();
	save();
	DRpcWrapper.call_stub("roll_stub", "unregister_role", get_account_id(), get_role_id());
	log_info("role logout %" I64_FMT "u", m_role_id);
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
	return m_level;
}

void role::add_level(TLevel_t lvl)
{
	m_level += lvl;
}

TSex_t role::get_sex() const
{
	return m_sex;
}

void role::set_sex(TSex_t sex)
{
	m_sex = sex;
}

void role::set_role_name(const TRoleName_t & role_name)
{
	m_role_name = role_name;
}

const TRoleName_t & role::get_role_name() const
{
	return m_role_name;
}

void role::set_account_id(TAccountID_t account_id)
{
	m_account_id = account_id;
}

TAccountID_t role::get_account_id() const
{
	return m_account_id;
}

void role::set_role_id(TRoleID_t role_id)
{
	m_role_id = role_id;
}

TRoleID_t role::get_role_id() const
{
	return m_role_id;
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
	m_destroy_flag = false;
	m_role_id = INVALID_ROLE_ID;
	m_level = 1;
	m_sex = INVALID_SEX;
	memset(m_role_name.data(), 0, ROLE_NAME_LEN);
}