
#include "roll_stub.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "string_common.h"

roll_stub::roll_stub() : server_entity()
{
	clean_up();
}

roll_stub::~roll_stub()
{
	clean_up();
}

bool roll_stub::init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id)
{
	log_info("roll stub init");
	if (!TBaseType_t::init(entity_id, gate_id, client_id)) {
		return false;
	}
	DRegisterEntityRpc(get_entity_id(), this, roll_stub, register_account, 2);
	DRegisterEntityRpc(get_entity_id(), this, roll_stub, unregister_account, 1);
	DRegisterEntityRpc(get_entity_id(), this, roll_stub, register_role, 3);
	DRegisterEntityRpc(get_entity_id(), this, roll_stub, unregister_role, 2);
	return true;
}

void roll_stub::register_account(TAccountID_t account_id, const mailbox_info& mailbox)
{
	auto itr = m_account_2_mailbox.find(account_id);
	if (itr == m_account_2_mailbox.end()) {
		m_account_2_mailbox[account_id] = mailbox;
		DRpcWrapper.call_entity(mailbox, "on_register_account", true, mailbox);
		log_info("register account success! account id %" I64_FMT "u, entity id %" I64_FMT "u", account_id, mailbox.entity_id);
	}
	else {
		const mailbox_info& old_mailbox = itr->second;
		DRpcWrapper.call_entity(mailbox, "on_register_account", false, old_mailbox);
		log_info("register account success, but has registered, account id %" I64_FMT "u, entity id %" I64_FMT "u, old entity id %" I64_FMT "u", 
			account_id, mailbox.entity_id, old_mailbox.entity_id);
		m_account_2_mailbox[account_id] = mailbox;
	}
}

void roll_stub::unregister_account(TAccountID_t account_id)
{
	auto itr = m_account_2_mailbox.find(account_id);
	if (itr != m_account_2_mailbox.end()) {
		log_info("unregister account success! account id %" I64_FMT "u", account_id);
		m_account_2_mailbox.erase(itr);
	}
	else {
		log_error("unregister account failed for not find account id %" I64_FMT "u", account_id);
	}
}

void roll_stub::register_role(TAccountID_t account_id, TRoleID_t role_id, const mailbox_info& mailbox)
{
	bool success = false;
	if (m_account_2_mailbox.find(account_id) == m_account_2_mailbox.end()) {
		log_error("register role failed for account not register! account id %" I64_FMT "u, entity id %" I64_FMT "u", account_id, mailbox.entity_id);
	}
	else if (m_role_2_mailbox.find(role_id) != m_role_2_mailbox.end()) {
		log_error("register role failed for role has registered! account id %" I64_FMT "u, entity id %" I64_FMT "u", account_id, mailbox.entity_id);
	}
	else {
		log_info("register role success! account id %" I64_FMT "u, entity id %" I64_FMT "u", account_id, mailbox.entity_id);
		success = true;
		m_account_2_mailbox[account_id] = mailbox;
		m_role_2_mailbox[role_id] = mailbox;
	}
	DRpcWrapper.call_entity(mailbox, "on_register_role", success);
}

void roll_stub::unregister_role(TAccountID_t account_id, TRoleID_t role_id)
{
	auto account_itr = m_account_2_mailbox.find(account_id);
	if (account_itr != m_account_2_mailbox.end()) {
		m_account_2_mailbox.erase(account_itr);
	}
	auto role_itr = m_role_2_mailbox.find(role_id);
	if (role_itr != m_role_2_mailbox.end()) {
		m_role_2_mailbox.erase(role_itr);
	}
	log_info("unregister role success! account id %" I64_FMT "u, role id %" I64_FMT "u", account_id, role_id);
}

void roll_stub::clean_up()
{
	m_account_2_mailbox.clear();
	m_role_2_mailbox.clear();
}
