
#include "roll_stub.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

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
	DRegisterStubRpc(this, roll_stub, register_role, 3);
	DRegisterStubRpc(this, roll_stub, unregister_role, 1);
	return true;
}

void roll_stub::register_role(TRoleID_t role_id, const proxy_info& proxy, const mailbox_info & mailbox)
{
	auto itr = m_role_id_2_role_info.find(role_id);
	if (itr == m_role_id_2_role_info.end()) {
		stub_role_info role_info;
		role_info.proxy = proxy;
		role_info.mailbox = mailbox;
		m_role_id_2_role_info[role_id] = role_info;
		DRpcWrapper.call_role(mailbox, "on_register", true, proxy, mailbox);
		log_info("register role success! role id = %" I64_FMT "u", role_id);
	}
	else {
		const stub_role_info& role_info = itr->second;
		DRpcWrapper.call_role(mailbox, "on_register", false, role_info.proxy, role_info.mailbox);
		log_error("register role failed for role has registered!, role id = %" I64_FMT "u", role_id);
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
	m_role_id_2_role_info.clear();
}
