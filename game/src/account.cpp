
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
	DRegisterEntityRpc(get_entity_id(), this, account, on_register_account, 2);
	DRegisterEntityRpc(get_entity_id(), this, account, on_relay_ready, 1);
	DRegisterEntityRpc(get_entity_id(), this, account, on_relay_login, 0);
	DRegisterEntityRpc(get_entity_id(), this, account, on_wait_login, 1);
	DRegisterEntityRpc(get_entity_id(), this, account, create_role, 2);
	DRegisterEntityRpc(get_entity_id(), this, account, disconnect_client, 0);
	return true;
}

void account::login(TPlatformID_t platform_id, const dynamic_string& user_id, const dynamic_string& token)
{
	log_info("start login! platform id %d, user id %s, entity id %" I64_FMT "u", platform_id, user_id.data(), get_entity_id());
	m_platform_id = platform_id;
	memcpy(m_user_id.data(), user_id.data(), user_id.size());
	DGameServer.db_query(
		"account",
		"account_id, role_id",
		gx_to_string("platform_id = %d and user_id = '%s'", m_platform_id, m_user_id.data()).c_str(),
		std::bind(&account::on_load_account_callback, this, std::placeholders::_1, std::placeholders::_2)
	);
}

void account::on_load_account_callback(bool status, const binary_data& result)
{
	if (!status) {
		log_error("on load account failed! client id %" I64_FMT "u, entity id %" I64_FMT "u", get_client_id(), get_entity_id());
		return;
	}
	if (result.empty()) {
		log_info("on load account success! start create account! entity id %" I64_FMT "u", get_entity_id());
		create_account();
	}
	else {
		int buffer_index = 0;
		uint16 row_count = 0;
		rpc_param_parse<uint16, uint16>::parse_param(row_count, result.data(), buffer_index);
		rpc_param_parse<TAccountID_t, TAccountID_t>::parse_param(m_account_id, result.data(), buffer_index);
		rpc_param_parse<TRoleID_t, TRoleID_t>::parse_param(m_role_id, result.data(), buffer_index);
		register_account();
		log_info("on load account success! role id % " I64_FMT "u, account id % " I64_FMT "u, entity id %" I64_FMT "u", m_role_id, m_account_id, get_entity_id());
	}
}

void account::create_account()
{
	TServerID_t server_id = DGameServer.get_server_id();
	TProcessID_t process_id = DGameServer.get_server_info().process_info.process_id;
	m_account_id = ((TAccountID_t)server_id << 48) + ((uint64)process_id << 32) + DSequence.gen_sequence_id(SEQUENCE_ACCOUNT);
	m_role_id = ((TRoleID_t)server_id << 48) + ((uint64)process_id << 32) + DSequence.gen_sequence_id(SEQUENCE_ROLE);
	DGameServer.db_insert(
		"account",
		"(platform_id, server_id, user_id, account_id, role_id)",
		gx_to_string("(%u, %d, '%s', '%" I64_FMT "u', '%" I64_FMT "u')", m_platform_id, server_id, m_user_id.data(), m_account_id, m_role_id).c_str(),
		[&](bool status) {
		if (status) {
			log_info("save account success! entity id %" I64_FMT "u", get_entity_id());
			register_account();
		}
		else {
			log_error("save account failed! entity id %" I64_FMT "u", get_entity_id());
		}
	});
}

void account::register_account()
{
	DRpcWrapper.call_stub("roll_stub", "register_account", m_account_id, get_mailbox());
}

void account::on_register_account(bool status, const mailbox_info& mailbox)
{
	if (status) {
		log_info("on register account success, start load role! entity id %" I64_FMT "u", get_entity_id());
		on_load_role();
	}
	else {
		log_info("on register account success, start relay login! entity id %" I64_FMT "u", get_entity_id());
		DRpcWrapper.call_entity(mailbox, "on_relay_ready", get_mailbox());
	}
}

void account::on_relay_ready(const mailbox_info& mailbox)
{
	log_info("kick current account by relay login %" I64_FMT "u", get_entity_id());
	DRpcWrapper.call_entity(mailbox, "on_relay_login");
	std::string msg = gx_to_string("{\"cmd\": \"login\", \"ret_code\": %d}", ERR_LOGIN_FAILED_BY_RELAY);
	DRpcWrapper.call_ws_client(get_proxy(), msg);
	destroy();
}

void account::on_wait_login(const mailbox_info& mailbox)
{
	log_info("wait login, migrate to role, destroy current account! %" I64_FMT "u", get_entity_id());
	DRpcWrapper.call_entity(mailbox, "on_relay_login", get_proxy());
	destroy();
}

void account::on_relay_login()
{
	if (is_destroy()) {
		log_info("account relay login success but account has destroy! entity id %" I64_FMT "u", get_entity_id());
		return;
	}
	on_load_role();
	log_info("account relay login success, start load role! %" I64_FMT "u", get_entity_id());
}

void account::on_load_role()
{
	DGameServer.db_query(
		"role",
		DQueryRoleSQL,
		gx_to_string("role_id = '%" I64_FMT "u'", m_role_id).c_str(),
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
		log_info("on load role callback, start create role! entity id %" I64_FMT "u", get_entity_id());
	}
	else {
		role_info role_data;
		role_data.account_id = m_account_id;
		role_data.role_id = m_role_id;
		int buffer_index = 0;
		uint16 row_count = 0;
		rpc_param_parse<uint16, uint16>::parse_param(row_count, result.data(), buffer_index);
		rpc_param_parse<TSex_t, TSex_t>::parse_param(role_data.sex, result.data(), buffer_index);
		rpc_param_parse<TLevel_t, TLevel_t>::parse_param(role_data.level, result.data(), buffer_index);
		dynamic_string name;
		rpc_param_parse<dynamic_string, dynamic_string>::parse_param(name, result.data(), buffer_index);
		memcpy(role_data.role_name.data(), name.data(), name.size());
		on_login_success(role_data);
		log_info("on load role callback success! entity id %" I64_FMT "u", get_entity_id());
	}
}

void account::create_role(TSex_t sex, const dynamic_string& role_name)
{
	m_sex = sex;
	memset(m_role_name.data(), 0, ROLE_NAME_LEN);
	memcpy(m_role_name.data(), role_name.data(), role_name.size());
	DGameServer.db_insert(
		"role",
		"(account_id, role_id, sex, level, role_name)",
		gx_to_string("('%" I64_FMT "u', '%" I64_FMT "u', %d, %d, '%s')", m_account_id, m_role_id, m_sex, 1, m_role_name.data()).c_str(),
		[&](bool status) {
		if (status) {
			log_info("save role success! client id %" I64_FMT "u, entity id %" I64_FMT "u", get_client_id(), get_entity_id());
			role_info role_data;
			role_data.account_id = m_account_id;
			role_data.role_id = m_role_id;
			role_data.level = 1;
			role_data.sex = m_sex;
			role_data.role_name = m_role_name;
			on_login_success(role_data);
		}
		else {
			log_error("save role failed! client id %" I64_FMT "u, entity id %" I64_FMT "u", get_client_id(), get_entity_id());
			unregister_account();
		}
	});
}

void account::on_login_success(const role_info& role_data)
{
	role* p = (role*)DEntityMgr.create_entity("role", get_gate_id(), get_client_id());
	p->set_account_id(role_data.account_id);
	p->set_role_id(role_data.role_id);
	p->set_level(role_data.level);
	p->set_sex(role_data.sex);
	p->set_role_name(role_data.role_name);
	p->register_role();
	destroy();
}

void account::disconnect_client()
{
	log_info("account disconnect client! %" I64_FMT "u", get_entity_id());
	if (is_destroy()) {
		return;
	}
	unregister_account();
}

void account::unregister_account()
{
	log_info("unregister account! account id %" I64_FMT "u, entity id %" I64_FMT "u", m_account_id, get_entity_id());
	DRpcWrapper.call_stub("roll_stub", "unregister_account", m_account_id);
	destroy();
}
