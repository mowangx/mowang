
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
	void register_role(TRoleID_t role_id, const mailbox_info& mailbox);
	void unregister_role(TRoleID_t role_id);

private:
	void clean_up();

public:
	std::unordered_map<TRoleID_t, mailbox_info> m_role_id_2_mailbox;
};

#endif // !_ROLL_STUB_H_

