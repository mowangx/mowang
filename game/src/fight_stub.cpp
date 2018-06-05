
#include "fight_stub.h"
#include "game_server.h"

fight_stub::fight_stub()
{
	clean_up();
}

fight_stub::~fight_stub()
{
}

bool fight_stub::init()
{
	DGameServer.db_query("fight", NULL, "*", [&](bool status, const dynamic_string_array2& result) {
		// add fight
	});
	return true;
}

void fight_stub::add_fight(TRoleID_t role_id, TNpcIndex_t npc_id, const game_pos & src_pos, const game_pos & dest_pos, 
	const dynamic_array<soldier_info>& soldiers, TGameTime_t fight_time)
{
	// save db
	// add timer
}

void fight_stub::del_fight(TRoleID_t role_id, TNpcIndex_t npc_id)
{
}

void fight_stub::clean_up()
{
}
