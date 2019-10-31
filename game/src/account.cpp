
#include "account.h"
#include "string_common.h"
#include "game_server.h"
#include "sequence.h"
#include "entity_manager.h"
#include "rpc_proxy.h"

account::account()
{
}

account::~account()
{
}

bool account::init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id)
{
	if (!TBaseType_t::init(entity_id, gate_id, client_id)) {
		return false;
	}
	DRegisterEntityRpc(get_entity_id(), this, account, login, 1);
	DRegisterEntityRpc(get_entity_id(), this, account, create_role, 2);
	DRegisterEntityRpc(get_entity_id(), this, account, on_register_account, 2);
	DRegisterEntityRpc(get_entity_id(), this, account, on_relay_ready, 1);
	DRegisterEntityRpc(get_entity_id(), this, account, on_relay_login, 0);
	DRegisterEntityRpc(get_entity_id(), this, account, on_wait_login, 1);
	return true;
}

void account::login(const account_info& account_data)
{
	log_info("account login! platform id %d, user id %s", account_data.platform_id, account_data.user_id.data());
	m_platform_id = account_data.platform_id;
	memcpy(m_user_id.data(), account_data.user_id.data(), USER_ID_LEN);
	DGameServer.db_query(
		"account",
		"account_id, role_id",
		gx_to_string("platform_id %d, user_id %" I64_FMT "u", m_platform_id, m_user_id.data()).c_str(),
		std::bind(&account::on_load_account_callback, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void account::create_role(TSex_t sex, const TRoleName_t& role_name)
{
	DGameServer.db_insert(
		"role",
		gx_to_string("role_id %" I64_FMT "u, role_name %s, sex %d", m_role_id, role_name.data(), sex).c_str(),
		[&](bool status) {
		if (status) {
			log_info("save role success! client id %" I64_FMT "u", get_client_id());
			role* role = on_login_success();
			role->set_account_id(m_account_id);
			role->set_role_id(m_role_id);
			role->set_sex(sex);
			role->set_role_name(role_name);
			role->register_role();
		}
		else {
			log_error("save role failed for save failed! client id %" I64_FMT "u", get_client_id());
		}
	});
}

void account::on_register_account(bool status, const mailbox_info& mailbox)
{
	if (status) {
		on_load_role();
	}
	else {
		log_info("on register account success, but has registered! entity id %" I64_FMT "u", get_entity_id());
		relay_login(mailbox);
	}
}

void account::on_relay_ready(const mailbox_info& mailbox)
{
	DRpcWrapper.call_entity(mailbox, "on_relay_login");
	std::string msg = gx_to_string("{\"cmd\": \"kick\"}");
	DRpcWrapper.call_ws_client(get_proxy(), msg);
	destroy();
}

void account::on_relay_login()
{
	on_load_role();
}

void account::on_wait_login(const mailbox_info& mailbox)
{
	DRpcWrapper.call_entity(mailbox, "on_relay_login", get_proxy());
}

void account::on_load_account_callback(bool status, const binary_data& result)
{
	if (!status) {
		log_error("load from db failed! client id %" I64_FMT "u", get_client_id());
		return;
	}
	if (result.empty()) {
		create_account();
	}
	else {
		m_account_id = 1;
		m_role_id = 1;
		register_account();
	}
}

void account::on_load_role()
{
	log_info("on register account success! entity id %" I64_FMT "u", get_entity_id());
	DGameServer.db_query(
		"role",
		"*",
		gx_to_string("role_id %" I64_FMT "u", m_role_id).c_str(),
		std::bind(&account::on_load_role_callback, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void account::on_load_role_callback(bool status, const binary_data& result)
{
	if (!status) {
		log_error("load from db failed! client id %" I64_FMT "u", get_client_id());
		return;
	}
	if (result.empty()) {
		std::string msg = gx_to_string("{\"cmd\": \"create_role\"}");
		DRpcWrapper.call_ws_client(get_proxy(), msg);
	}
	else {
		role* role = on_login_success();
		role->set_account_id(m_account_id);
		role->set_role_id(m_role_id);
		role->register_role();
	}
}

void account::create_account()
{
	TServerID_t server_id = DGameServer.get_server_info().process_info.server_id;
	m_account_id = ((TAccountID_t)server_id << 48) + DSequence.gen_sequence_id(SEQUENCE_ACCOUNT);
	m_role_id = ((TRoleID_t)server_id << 48) + DSequence.gen_sequence_id(SEQUENCE_ROLE);
	DGameServer.db_insert(
		"account",
		gx_to_string("platform_id %u, user_id %s, account id %" I64_FMT "u, role_id %" I64_FMT "u", m_platform_id, m_user_id.data(), m_account_id, m_role_id).c_str(),
		[&](bool status) {
			if (status) {
				log_info("save account success! client id %" I64_FMT "u", get_client_id());
				register_account();
			}
			else {
				log_error("save account failed for save failed! client id %" I64_FMT "u", get_client_id());
			}
	});
}

void account::register_account()
{
	log_info("on account login success! account id %" I64_FMT "u", m_account_id);
	DRpcWrapper.call_stub("roll_stub", "register_account", m_account_id, get_mailbox());
}

void account::relay_login(const mailbox_info& mailbox)
{
	DRpcWrapper.call_entity(mailbox, "on_relay_ready", get_mailbox());
}

role* account::on_login_success()
{
	role* p = (role*)DEntityMgr.create_entity("role");
	return p;
}

void account::destroy()
{
}
