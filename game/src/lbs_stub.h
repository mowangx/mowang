
#ifndef _LBS_STUB_H_
#define _LBS_STUB_H_

#include <unordered_map>

#include "server_entity.h"
#include "packet_struct.h"

class lbs_stub : public server_entity
{
	typedef server_entity TBaseType_t;

public:
	lbs_stub();
	virtual ~lbs_stub();

public:
	virtual bool init(TEntityID_t entity_id) override;

public:
	void query_city(const game_pos& pos, const mailbox_info& mailbox, const dynamic_string& callback);
	void update_city(const game_pos& pos, TRoleID_t role_id);

private:
	void clean_up();

public:
	std::unordered_map<uint32, TRoleID_t> m_pos_2_role;
};

#endif // !_LBS_STUB_H_
