
#ifndef _ROLL_STUB_H_
#define _ROLL_STUB_H_

#include <unordered_map>

#include "server_entity.h"
#include "packet_struct.h"

class roll_stub : public server_entity
{
	typedef server_entity TBaseType_t;

public:
	roll_stub();
	virtual ~roll_stub();

public:
	virtual bool init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id) override;

public:
	void register_account(TAccountID_t account_id, const mailbox_info& mailbox);
	void unregister_account(TAccountID_t account_id);
	void register_role(TAccountID_t account_id, TRoleID_t role_id, const mailbox_info& mailbox);
	void unregister_role(TAccountID_t account_id, TRoleID_t role_id);

private:
	void clean_up();

public:
	std::unordered_map<TAccountID_t, mailbox_info> m_account_2_mailbox;
	std::unordered_map<TRoleID_t, mailbox_info> m_role_2_mailbox;
};

#endif // !_ROLL_STUB_H_

