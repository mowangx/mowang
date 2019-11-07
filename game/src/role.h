
#ifndef _ROLE_H_
#define _ROLE_H_

#include <vector>

#include "server_entity.h"
#include "server_struct.h"
#include "packet_struct.h"
#include "dynamic_array.h"

class role : public server_entity
{
	typedef server_entity TBaseType_t;

public:
	role();
	virtual ~role();

public:
	virtual bool init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id) override;

public:
	void on_register_role(bool status);
	void on_relay_ready(const mailbox_info& mailbox);
	void on_relay_login(const proxy_info& proxy);
	void disconnect_client();

public:
	void register_role();
	void logout(bool need_unregister);

public:
	TLevel_t get_level() const;
	void set_level(TLevel_t lvl);
	void add_level(TLevel_t lvl);

	TSex_t get_sex() const;
	void set_sex(TSex_t sex);

public:
	void set_role_name(const TRoleName_t& role_name);
	const TRoleName_t& get_role_name() const;

	void set_account_id(TAccountID_t account_id);
	TAccountID_t get_account_id() const;
	
	void set_role_id(TRoleID_t role_id);
	TRoleID_t get_role_id() const;

private:
	void save();

private:
	void clean_up();

public:
	uint8 m_status;
	TAccountID_t m_account_id;
	TRoleID_t m_role_id;
	TLevel_t m_level;
	TSex_t m_sex;
	TRoleName_t m_role_name;
};

#endif // !_ROLE_H_

