
#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include "server_entity.h"

class role;

class account : public server_entity
{
	typedef server_entity TBaseType_t;

public:
	account();
	virtual ~account();

public:
	virtual bool init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id) override;
	virtual void on_disconnect() override;

public:
	void login(const account_info& account_data);
	void create_role(TSex_t sex, const TRoleName_t& role_name);
	void on_register_account(bool status, const mailbox_info& mailbox);
	void on_relay_ready(const mailbox_info& mailbox);
	void on_relay_login();
	void on_wait_login(const mailbox_info& mailbox);

private:
	void on_load_account_callback(bool status, const binary_data& result);
	void on_load_role();
	void on_load_role_callback(bool status, const binary_data& result);
	void create_account();
	void register_account();
	void unregister_account();
	void relay_login(const mailbox_info& mailbox);
	role* on_login_success();

private:
	TPlatformID_t m_platform_id;
	TUserID_t m_user_id;
	TAccountID_t m_account_id;
	TRoleID_t m_role_id;
};

#endif // !_ACCOUNT_H_

