
#include "role.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "string_common.h"
#include "sequence.h"
#include "entity_manager.h"
#include "game_enum.h"
#include "timer.h"
#include "error_code.h"

role::role() : server_entity()
{
	clean_up();
}

role::~role()
{
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
	DRegisterEntityRpc(get_entity_id(), this, role, kick_role, 0);
	DRegisterEntityRpc(get_entity_id(), this, role, disconnect_client, 0);
	return true;
}

void role::register_role()
{
	log_info("role login success, start register role! role id %" I64_FMT "u", get_role_id());
	DRpcWrapper.call_stub("roll_stub", "register_role", get_account_id(), get_role_id(), get_mailbox());
}

void role::on_register_role(bool status)
{
	if (status) {
		if (m_status == ROLE_STATUS_DISCONNECT) {
			log_info("on register role success, but disconnect client, start logout! role id %" I64_FMT "u ", get_role_id());
			logout(true);
		}
		else {
			log_info("on register role success! role id %" I64_FMT "u ", get_role_id());
			m_status = ROLE_STATUS_NORMAL;
			std::string msg = gx_to_string("{\"cmd\": \"login\", \"ret_code\": %d, \"account_id\": %" I64_FMT "u, \"role_id\": %" I64_FMT "u, \"level\": %d, \"sex\": %d, \"role_name\": \"%s\"}", 
				ERR_SUCCESS, m_account_id, m_role_id, m_level, m_sex, m_role_name.data());
			DRpcWrapper.call_ws_client(get_proxy(), msg);
		}
	}
	else {
		log_info("on register role failed, kick current role! role id %" I64_FMT "u ", get_role_id());
		std::string msg = gx_to_string("{\"cmd\": \"login\", \"ret_code\": %d}", ERR_LOGIN_FAILED_BY_RELAY);
		DRpcWrapper.call_ws_client(get_proxy(), msg);
		logout(false);
	}
}

void role::on_relay_ready(const mailbox_info& mailbox)
{
	log_info("kick current client by relay login! role id %" I64_FMT "u", get_role_id());
	m_status = ROLE_STATUS_RELAY;
	DRpcWrapper.call_entity(mailbox, "on_wait_login");
	std::string msg = gx_to_string("{\"cmd\": \"kick\"}");
	DRpcWrapper.call_ws_client(get_proxy(), msg);
}

void role::on_relay_login(const proxy_info& proxy)
{
	log_info("migrate finish, on relay login success! role id %" I64_FMT "u", get_role_id());
	m_status = ROLE_STATUS_NORMAL;
	update_proxy(proxy);
	DRpcWrapper.call_stub("roll_stub", "update_by_relay", get_account_id(), get_role_id(), get_mailbox());
}

void role::kick_role()
{
	log_info("kick role! role id %" I64_FMT "u", get_role_id());
	std::string msg = gx_to_string("{\"cmd\": \"kick\"}");
	DRpcWrapper.call_ws_client(get_proxy(), msg);
	logout(true);
}

void role::disconnect_client()
{
	log_info("role disconnect client! role id %" I64_FMT "u", get_role_id());
	if (is_destroy()) {
		return;
	}
	if (m_status == ROLE_STATUS_NORMAL) {
		logout(true);
	}
	else {
		m_status = ROLE_STATUS_DISCONNECT;
		DTimer.add_timer(5, false, nullptr, [&](void* param, TTimerID_t timer_id) {
			if (m_status != ROLE_STATUS_NORMAL) {
				logout(true);
			}
			log_info("role relay timeout! role id %" I64_FMT "u", get_role_id());
		});
	}
}

void role::logout(bool need_unregister)
{
	log_info("logout role! role id %" I64_FMT "u", m_role_id);
	if (need_unregister) {
		save();
		DRpcWrapper.call_stub("roll_stub", "unregister_role", get_account_id(), get_role_id());
	}
	destroy();
}

TLevel_t role::get_level() const
{
	return m_level;
}

void role::set_level(TLevel_t lvl)
{
	m_level = lvl;
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

void role::clean_up()
{
	m_status = INVALID_ROLE_STATUS;
	m_role_id = INVALID_ROLE_ID;
	m_level = 1;
	m_sex = INVALID_SEX;
	memset(m_role_name.data(), 0, ROLE_NAME_LEN);
}