
#include "account.h"
#include "string_common.h"
#include "game_server.h"
#include "sequence.h"
#include "entity_manager.h"
#include "rpc_proxy.h"
#include "sql_utils.h"
#include "error_code.h"

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
	DRegisterEntityRpc(get_entity_id(), this, account, login, 3);
	DRegisterEntityRpc(get_entity_id(), this, account, disconnect_client, 0);
	DRegisterEntityRpc(get_entity_id(), this, account, create_role, 2);
	DRegisterEntityRpc(get_entity_id(), this, account, on_register_account, 2);
	DRegisterEntityRpc(get_entity_id(), this, account, on_relay_ready, 1);
	DRegisterEntityRpc(get_entity_id(), this, account, on_relay_login, 0);
	DRegisterEntityRpc(get_entity_id(), this, account, on_wait_login, 1);
	return true;
}

void account::login(TPlatformID_t platform_id, const dynamic_string& user_id, const dynamic_string& token)
{
	log_info("account login! platform id %d, user id %s", platform_id, user_id.data());
	m_platform_id = platform_id;
	memcpy(m_user_id.data(), user_id.data(), user_id.size());
	DGameServer.db_query(
		"account",
		"account_id, role_id",
		gx_to_string("platform_id = %d, user_id = %" I64_FMT "u", m_platform_id, m_user_id.data()).c_str(),
		std::bind(&account::on_load_account_callback, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void account::disconnect_client()
{
	log_info("account disconnect! %" I64_FMT "u", get_entity_id());
	if (is_destroy()) {
		return;
	}
	unregister_account();
}

void account::create_role(TSex_t sex, const dynamic_string& role_name)
{
	m_sex = sex;
	memset(m_role_name.data(), 0, ROLE_NAME_LEN);
	memcpy(m_role_name.data(), role_name.data(), role_name.size());
	DGameServer.db_insert(
		"role",
		gx_to_string("(role_id, sex, level, role_name)").c_str(),
		gx_to_string("(%" I64_FMT "u, %d, %d, %s)", m_role_id, m_sex, 1, m_role_name.data()).c_str(),
		[&](bool status) {
		if (status) {
			log_info("save role success! client id %" I64_FMT "u, entity id %" I64_FMT "u", get_client_id(), get_entity_id());
			role* role = on_login_success();
			role->set_account_id(m_account_id);
			role->set_role_id(m_role_id);
			role->set_sex(m_sex);
			role->set_role_name(m_role_name);
			role->register_role();
			destroy();
		}
		else {
			log_error("save role failed for save failed! client id %" I64_FMT "u, entity id %" I64_FMT "u", get_client_id(), get_entity_id());
			unregister_account();
		}
	});
}

void account::on_register_account(bool status, const mailbox_info& mailbox)
{
	if (status) {
		log_info("on register account success! entity id %" I64_FMT "u", get_entity_id());
		on_load_role();
	}
	else {
		log_info("on register account success, but has registered! entity id %" I64_FMT "u", get_entity_id());
		relay_login(mailbox);
	}
}

void account::on_relay_ready(const mailbox_info& mailbox)
{
	log_info("account on relay ready! %" I64_FMT "u", get_entity_id());
	DRpcWrapper.call_entity(mailbox, "on_relay_login");
	std::string msg = gx_to_string("{\"cmd\": \"login\", \"ret_code\": %d}", ERR_LOGIN_FAILED_BY_RELAY);
	DRpcWrapper.call_ws_client(get_proxy(), msg);
	destroy();
}

void account::on_relay_login()
{
	if (is_destroy()) {
		log_info("account on relay login, but account has destroy! entity id %" I64_FMT "u", get_entity_id());
		return;
	}
	on_load_role();
	log_info("account on relay login success! %" I64_FMT "u", get_entity_id());
}

void account::on_wait_login(const mailbox_info& mailbox)
{
	log_info("account on wait login! %" I64_FMT "u", get_entity_id());
	DRpcWrapper.call_entity(mailbox, "on_relay_login", get_proxy());
}

void account::on_load_account_callback(bool status, const binary_data& result)
{
	if (!status) {
		log_error("on load account failed! client id %" I64_FMT "u, entity id %" I64_FMT "u", get_client_id(), get_entity_id());
		return;
	}
	if (result.empty()) {
		create_account();
	}
	else {
		int buffer_index = 0;
		rpc_param_parse<TAccountID_t, TAccountID_t>::parse_param(m_account_id, result.data(), buffer_index);
		rpc_param_parse<TRoleID_t, TRoleID_t>::parse_param(m_role_id, result.data(), buffer_index);
		register_account();
		log_info("on load account success! role id % " I64_FMT "u, account id % " I64_FMT "u, entity id %" I64_FMT "u", m_role_id, m_account_id, get_entity_id());
	}
}

void account::on_load_role()
{
	DGameServer.db_query(
		"role",
		DQueryRoleSQL,
		gx_to_string("role_id = %" I64_FMT "u", m_role_id).c_str(),
		std::bind(&account::on_load_role_callback, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void account::on_load_role_callback(bool status, const binary_data& result)
{
	if (is_destroy()) {
		log_info("on load role callback, but account has destroy! entity id %" I64_FMT "u", get_entity_id());
		return;
	}
	if (!status) {
		log_error("on load role callback failed! entity id %" I64_FMT "u", get_entity_id());
		unregister_account();
		return;
	}
	if (result.empty()) {
		std::string msg = gx_to_string("{\"cmd\": \"create_role\"}");
		call_ws_client(msg);
		call_client("create_role");
	}
	else {
		role* role = on_login_success();
		role->set_account_id(m_account_id);
		role->set_role_id(m_role_id);
		int buffer_index = 0;
		TSex_t sex = INVALID_SEX;
		rpc_param_parse<TSex_t, TSex_t>::parse_param(sex, result.data(), buffer_index);
		role->set_sex(sex);
		TLevel_t level = INVALID_LEVEL;
		rpc_param_parse<TLevel_t, TLevel_t>::parse_param(level, result.data(), buffer_index);
		role->set_level(level);
		dynamic_string name;
		rpc_param_parse<dynamic_string, dynamic_string>::parse_param(name, result.data(), buffer_index);
		TRoleName_t role_name;
		memset(role_name.data(), 0, ROLE_NAME_LEN);
		memcpy(role_name.data(), name.data(), name.size());
		role->set_role_name(role_name);
		role->register_role();
		destroy();
		log_info("on load role callback success! role id % " I64_FMT "u, account id % " I64_FMT "u, sex %d, level %d, role name %s, entity id %" I64_FMT "u",
			m_role_id, m_account_id, sex, level, role_name.data(), get_entity_id());
	}
}

void account::create_account()
{
	TServerID_t server_id = DGameServer.get_server_info().process_info.server_id;
	m_account_id = ((TAccountID_t)server_id << 48) + DSequence.gen_sequence_id(SEQUENCE_ACCOUNT);
	m_role_id = ((TRoleID_t)server_id << 48) + DSequence.gen_sequence_id(SEQUENCE_ROLE);
	DGameServer.db_insert(
		"account",
		gx_to_string("(platform_id, user_id, account_id, role_id)").c_str(),
		gx_to_string("(%u, %s, %" I64_FMT "u, %" I64_FMT "u)", m_platform_id, m_user_id.data(), m_account_id, m_role_id).c_str(),
		[&](bool status) {
			if (status) {
				log_info("save account success! entity id %" I64_FMT "u", get_entity_id());
				register_account();
			}
			else {
				log_error("save account failed for save failed! entity id %" I64_FMT "u", get_entity_id());
			}
	});
}

void account::register_account()
{
	log_info("start register account! account id %" I64_FMT "u, entity id %" I64_FMT "u", m_account_id, get_entity_id());
	DRpcWrapper.call_stub("roll_stub", "register_account", m_account_id, get_mailbox());
}

void account::unregister_account()
{
	log_info("unregister account! account id %" I64_FMT "u, entity id %" I64_FMT "u", m_account_id, get_entity_id());
	DRpcWrapper.call_stub("roll_stub", "unregister_account", m_account_id);
	destroy();
}

void account::relay_login(const mailbox_info& mailbox)
{
	DRpcWrapper.call_entity(mailbox, "on_relay_ready", get_mailbox());
}

role* account::on_login_success()
{
	role* p = (role*)DEntityMgr.create_entity("role", get_gate_id(), get_client_id());
	return p;
}
