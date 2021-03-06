
#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include "server_entity.h"
#include "game_struct.h"

class role;

class account : public server_entity
{
	typedef server_entity TBaseType_t;

public:
	account();
	virtual ~account();

public:
	virtual bool init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id) override;

public:
	void login(TPlatformID_t platform_id, const dynamic_string& user_id, const dynamic_string& token);
private:
	void on_load_account_callback(bool status, const binary_data& result);
	void create_account();
	void register_account();
public:
	void on_register_account(bool status, const mailbox_info& mailbox);
	void on_relay_ready(const mailbox_info& mailbox);
	void on_wait_login(const mailbox_info& mailbox);
	void on_relay_login();
private:
	void on_load_role();
	void on_load_role_callback(bool status, const binary_data& result);
public:
	void create_role(TSex_t sex, const dynamic_string& role_name);
private:
	void on_login_success(const role_info& role_data);

public:
	void disconnect_client();
private:
	void unregister_account();

private:
	TPlatformID_t m_platform_id;
	TUserID_t m_user_id;
	TAccountID_t m_account_id;
	TRoleID_t m_role_id;
	TSex_t m_sex;
	TRoleName_t m_role_name;
};

#endif // !_ACCOUNT_H_

