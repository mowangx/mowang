
#ifndef _ROLL_STUB_H_
#define _ROLL_STUB_H_

#include <unordered_map>

#include "entity.h"
#include "packet_struct.h"

class roll_stub : public entity
{
public:
	roll_stub();
	virtual ~roll_stub();

public:
	virtual bool init() override;

public:
	void register_account(TPlatformID_t platform_id, TUserID_t user_id, const proxy_info& proxy, const mailbox_info& mailbox);
	void unregister_account(TPlatformID_t platform_id, TUserID_t user_id);
	void register_role(TPlatformID_t platform_id, TUserID_t user_id, TRoleID_t role_id, const proxy_info& proxy, const mailbox_info& mailbox);
	void unregister_role(TRoleID_t role_id);

private:
	void clean_up();

public:
	std::unordered_map<std::string, stub_role_info> m_user_id_2_role_info;
	std::unordered_map<TRoleID_t, stub_role_info> m_role_id_2_role_info;
};

#endif // !_ROLL_STUB_H_

