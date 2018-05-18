
#include "roll_stub.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "string_common.h"

roll_stub::roll_stub()
{
	clean_up();
}

roll_stub::~roll_stub()
{
	clean_up();
}

bool roll_stub::init()
{
	DRegisterStubRpc(this, roll_stub, register_account, 5);
	DRegisterStubRpc(this, roll_stub, unregister_account, 2);
	DRegisterStubRpc(this, roll_stub, register_role, 6);
	DRegisterStubRpc(this, roll_stub, unregister_role, 1);
	return true;
}

void roll_stub::register_account(TPlatformID_t platform_id, TUserID_t user_id, const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id)
{
	std::string key_id = gx_to_string("%u#!#!", platform_id) + user_id.data();
	auto itr = m_user_id_2_role_info.find(key_id);
	if (itr == m_user_id_2_role_info.end()) {
		stub_role_info role_info;
		role_info.proxy = proxy;
		role_info.mailbox = mailbox;
		m_user_id_2_role_info[key_id] = role_info;
		DRpcWrapper.call_role(role_info.mailbox, "on_register_account", true, proxy, mailbox, test_client_id);
		log_info("register account success! platform id = %u, user id = %s, client id = %" I64_FMT "u", platform_id, user_id.data(), proxy.client_id);
	}
	else {
		stub_role_info& role_info = itr->second;
		DRpcWrapper.call_role(role_info.mailbox, "on_register_account", false, proxy, mailbox, test_client_id);
		log_info("register account success, but has registered, platform id = %u, user id = %s, cur client id = %" I64_FMT "u, old client id = %" I64_FMT "u",
			platform_id, user_id.data(), proxy.client_id, role_info.proxy.client_id);

		role_info.proxy = proxy;
	}
}

void roll_stub::unregister_account(TPlatformID_t platform_id, TUserID_t user_id)
{
	std::string key_id = gx_to_string("%u#!#!", platform_id) + user_id.data();
	auto itr = m_user_id_2_role_info.find(key_id);
	if (itr != m_user_id_2_role_info.end()) {
		const stub_role_info& role_info = itr->second;
		log_info("unregister account success! platform id = %u, user id = %s, client id = %" I64_FMT "u", platform_id, user_id.data(), role_info.proxy.client_id);
		m_user_id_2_role_info.erase(itr);
	}
	else {
		log_error("unregister account failed for not find user id! platform id = %u, user id = %s", platform_id, user_id.data());
	}
}

void roll_stub::register_role(TPlatformID_t platform_id, TUserID_t user_id, TRoleID_t role_id, const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id)
{
	auto itr = m_role_id_2_role_info.find(role_id);
	if (itr == m_role_id_2_role_info.end()) {
		std::string key_id = gx_to_string("%u#!#!", platform_id) + user_id.data();
		stub_role_info role_info;
		role_info.proxy = proxy;
		role_info.mailbox = mailbox;
		m_user_id_2_role_info[key_id] = role_info;
		m_role_id_2_role_info[role_id] = role_info;
		DRpcWrapper.call_role(mailbox, "on_register_role", true, proxy, mailbox, test_client_id);
		log_info("register role success! client id = %" I64_FMT "u, role id = %" I64_FMT "u", proxy.client_id, role_id);
	}
	else {
		stub_role_info& role_info = itr->second;
		DRpcWrapper.call_role(role_info.mailbox, "on_register_role", false, proxy, mailbox, test_client_id);
		log_info("register role success, but role has registered! client id = %" I64_FMT "u, role id = %" I64_FMT "u", role_info.proxy.client_id, role_id);

		role_info.proxy = proxy;
	}
}

void roll_stub::unregister_role(TRoleID_t role_id)
{
	auto itr = m_role_id_2_role_info.find(role_id);
	if (itr != m_role_id_2_role_info.end()) {
		m_role_id_2_role_info.erase(itr);
		log_info("unregister role sucess! role id = %" I64_FMT "u", role_id);
	}
	else {
		log_error("unregister role failed for not find role id! role id = %" I64_FMT "u", role_id);
	}
}

void roll_stub::clean_up()
{
	m_user_id_2_role_info.clear();
	m_role_id_2_role_info.clear();
}
