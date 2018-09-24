#ifndef _FIGHT_STUB_H_
#define _FIGHT_STUB_H_

#include "entity.h"
#include "dynamic_array.h"
#include "packet_struct.h"
#include "game_struct.h"

class fight_stub : public entity
{
	typedef entity TBaseType_t;

public:
	fight_stub();
	virtual ~fight_stub();

public:
	virtual bool init(TServerID_t server_id, TProcessID_t game_id, TEntityID_t entity_id) override;

public:
	void add_fight(TRoleID_t role_id, TNpcIndex_t npc_id, const game_pos& src_pos, const game_pos& dest_pos,
		const dynamic_array<soldier_info>& soldiers, TGameTime_t fight_time);
	void del_fight(TRoleID_t role_id, TNpcIndex_t npc_id);

private:
	void clean_up();

private:
	std::vector<fight_info> m_fights;
};

#endif // !_FIGHT_STUB_H_

