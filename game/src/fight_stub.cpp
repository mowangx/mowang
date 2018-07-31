
#include "fight_stub.h"
#include "game_server.h"
#include "timer.h"
#include "time_manager.h"
#include "string_common.h"
#include "rpc_param.h"
#include "binary_string.h"

fight_stub::fight_stub()
{
	clean_up();
}

fight_stub::~fight_stub()
{
	for (auto itr = m_fights.begin(); itr != m_fights.end(); ++itr) {
		const fight_info& fight_data = *itr;
		DTimer.del_timer(fight_data.timer_id);
	}
	clean_up();
}

bool fight_stub::init()
{
	log_info("fight stub init");
	DGameServer.db_query("fight", NULL, "role_id, npc_id, src_x, src_y, dest_x, dest_y, soldiers, unix_timestamp(fight_time)", [&](bool status, const binary_data& result) {
		if (!status) {
			log_error("load fight data from db failed!");
			return;
		}

		int buffer_index = 0;
		uint16 num = 0;
		rpc_param_parse<uint16, uint16>::parse_param(num, result.data(), buffer_index);
		for (int i = 0; i < num; ++i) {
			fight_info fight_data;
			rpc_param_parse<TRoleID_t, TRoleID_t>::parse_param(fight_data.role_id, result.data(), buffer_index);
			rpc_param_parse<TNpcIndex_t, TNpcIndex_t>::parse_param(fight_data.npc_id, result.data(), buffer_index);
			rpc_param_parse<TPosValue_t, TPosValue_t>::parse_param(fight_data.src_pos.x, result.data(), buffer_index);
			rpc_param_parse<TPosValue_t, TPosValue_t>::parse_param(fight_data.src_pos.y, result.data(), buffer_index);
			rpc_param_parse<TPosValue_t, TPosValue_t>::parse_param(fight_data.dest_pos.x, result.data(), buffer_index);
			rpc_param_parse<TPosValue_t, TPosValue_t>::parse_param(fight_data.dest_pos.y, result.data(), buffer_index);
			db_param_parse<dynamic_array<soldier_info>, soldier_info>::parse_param(fight_data.soldiers, result.data(), buffer_index);
			uint64 fight_time = 0;
			rpc_param_parse<uint64, uint64>::parse_param(fight_time, result.data(), buffer_index);
			fight_data.fight_time = fight_time;
			log_info("fight stub init data! role id = %" I64_FMT "u, src pox x = %d, dest pos x = %d, fight time = %u",
				fight_data.role_id, fight_data.src_pos.x, fight_data.dest_pos.x, fight_data.fight_time);
		}
		
	});
	return true;
}

void fight_stub::add_fight(TRoleID_t role_id, TNpcIndex_t npc_id, const game_pos& src_pos, const game_pos& dest_pos, 
	const dynamic_array<soldier_info>& soldiers, TGameTime_t fight_time)
{
	// save db
	DGameServer.db_insert("fight", "", [&](bool status) {
		if (!status) {
			log_error("save fight data failed!");
			return;
		}
	});
	fight_info fight_data;
	fight_data.role_id = role_id;
	fight_data.npc_id = npc_id;
	fight_data.src_pos = src_pos;
	fight_data.dest_pos = dest_pos;
	fight_data.soldiers = soldiers;
	fight_data.fight_time = fight_time;
	// add timer
	TGameTime_t delay_time = fight_time - DTimeMgr.now_sys_time();
	fight_data.timer_id = DTimer.add_timer(delay_time, false, NULL, [&](void* param, TTimerID_t timer_id) {

	});
	m_fights.push_back(fight_data);
}

void fight_stub::del_fight(TRoleID_t role_id, TNpcIndex_t npc_id)
{
	for (auto itr = m_fights.begin(); itr != m_fights.end(); ++itr) {
		const fight_info& fight_data = *itr;
		if (fight_data.role_id != role_id || fight_data.npc_id != npc_id) {
			continue;
		}

		DTimer.del_timer(fight_data.timer_id);
		m_fights.erase(itr);
		return;
	}
}

void fight_stub::clean_up()
{
	m_fights.clear();
}
