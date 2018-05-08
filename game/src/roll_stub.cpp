
#include "roll_stub.h"
#include "rpc_proxy.h"

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
	DRegisterStubRpc(this, roll_stub, register_role, 2);
	DRegisterStubRpc(this, roll_stub, unregister_role, 1);
	return true;
}

void roll_stub::register_role(TRoleID_t role_id, const mailbox_info & mailbox)
{
	auto itr = m_role_id_2_mailbox.find(role_id);
	if (itr == m_role_id_2_mailbox.end()) {
		m_role_id_2_mailbox[role_id] = mailbox;
		log_info("register role success! role id = '%"I64_FMT"u'", role_id);
	}
	else {
		log_error("register role failed for role has registered!, role id = '%"I64_FMT"u'", role_id);
	}
}

void roll_stub::unregister_role(TRoleID_t role_id)
{
	auto itr = m_role_id_2_mailbox.find(role_id);
	if (itr != m_role_id_2_mailbox.end()) {
		m_role_id_2_mailbox.erase(itr);
		log_info("unregister role sucess! role id = '%"I64_FMT"u'", role_id);
	}
	else {
		log_error("unregister role failed for not find role id! role id = '%"I64_FMT"u'", role_id);
	}
}

void roll_stub::clean_up()
{
	m_role_id_2_mailbox.clear();
}
