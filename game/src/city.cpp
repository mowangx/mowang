
#include "city.h"

#include <algorithm>

#include "game_enum.h"
#include "game_server.h"
#include "resource.h"
#include "farmland.h"
#include "npc.h"
#include "time_manager.h"
#include "timer.h"

city::city()
{
	clean_up();
	m_resources.clear();
	m_npcs.clear();
	m_farmlands.clear();
}

city::~city()
{
	if (INVALID_TIMER_ID != m_research_technology.timer_id) {
		DTimer.del_timer(m_research_technology.timer_id);
	}
	m_research_technology.clean_up();

	for (auto training_info : m_soldier_trainings) {
		DTimer.del_timer(training_info->timer_id);
		DGameServer.deallocate_soldier_training(training_info);
	}
	m_soldier_trainings.clear();

	for (auto up_info : m_resource_up_infos) {
		DTimer.del_timer(up_info->timer_id);
		DGameServer.deallocate_resource_up_info(up_info);
	}
	m_resource_up_infos.clear();

	for (auto res : m_resources) {
		DGameServer.deallocate_resource(res);
	}
	m_resources.clear();

	for (auto p : m_npcs) {
		DGameServer.deallocate_npc(p);
	}
	m_npcs.clear();

	for (auto f : m_farmlands) {
		DGameServer.deallocate_farmland(f);
	}
	m_farmlands.clear();

	clean_up();
}

void city::fight(TNpcIndex_t npc_id, dynamic_array<soldier_info>& soldiers, const game_pos& pos)
{
	if (!check_npc(npc_id) || !check_soldiers(soldiers)) {
		return;
	}
	fight_info f;
	f.npc_id = npc_id;
	f.soldiers = soldiers;
	f.pos = pos;
	TGameTime_t move_time = calc_fight_time(npc_id, pos);
	f.fight_time = DTimeMgr.now_sys_time() + move_time;
	m_fight_infos.push_back(f);
	DTimer.add_timer(move_time, false, NULL, [&](void* param, TTimerID_t timer_id) {

	});
}

void city::gather(TNpcIndex_t npc_id, dynamic_array<soldier_info>& soldiers, const game_pos& pos)
{

}

bool city::check_npc(TNpcIndex_t npc_id) const
{
	for (auto p : m_npcs) {
		if (p->get_index() == npc_id) {
			return true;
		}
	}
	return false;
}

bool city::check_soldiers(dynamic_array<soldier_info>& soldiers) const
{
	for (int i = 0; i < soldiers.size(); ++i) {
		const soldier_info& solider = soldiers[i];
		if (solider.soldier_num < m_soldier_num[solider.soldier_type]) {
			return false;
		}
	}
	return true;
}

void city::random_resources(TLevel_t lvl)
{
}

void city::add_training_soldier(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TConsumeType_t consume_type)
{
	TGameTime_t training_time = calc_soldier_training_time(soldier_type, soldier_num, consume_type);
	soldier_training_info* soldier_info = DGameServer.allocate_soldier_training();
	soldier_info->soldier_type = soldier_type;
	soldier_info->soldier_num = soldier_num;
	soldier_info->end_time = DTimeMgr.now_sys_time() + training_time;
	soldier_info->timer_id = DTimer.add_timer(training_time, false, NULL, [&](void* param, TTimerID_t timer_id) {
		del_training_soldier_core(timer_id, false);
	});
	m_soldier_trainings.push_back(soldier_info);
}

void city::del_training_soldier(TTimerID_t timer_id)
{
	del_training_soldier_core(timer_id, true);
}

void city::del_training_soldier_core(TTimerID_t timer_id, bool cancel_flag)
{
	for (auto itr = m_soldier_trainings.begin(); itr != m_soldier_trainings.end(); ++itr) {
		soldier_training_info* training_info = *itr;
		if (training_info->timer_id != timer_id) {
			continue;
		}
		if (cancel_flag) {
			DTimer.del_timer(training_info->timer_id);
		}
		else {
			on_training_soldier_success(*training_info);
		}
		DGameServer.deallocate_soldier_training(training_info);
		m_soldier_trainings.erase(itr);
		break;
	}
	
}

void city::on_training_soldier_success(const soldier_training_info & training_info)
{
	m_soldier_num[training_info.soldier_type] += training_info.soldier_num;
}

void city::up_technology_level(TTechnologyType_t technology_type, TConsumeType_t consume_type)
{
	TGameTime_t up_time = calc_research_technology_time(technology_type, consume_type);
	m_research_technology.technology_type = technology_type;
	m_research_technology.end_time = DTimeMgr.now_sys_time() + up_time;
	m_research_technology.timer_id = DTimer.add_timer(up_time, false, NULL, [&](void* param, TTimerID_t timer_id) {
		del_up_technology_level_core(false);
	});
}

void city::del_up_technology_level()
{
	del_up_technology_level_core(true);
}

void city::del_up_technology_level_core(bool cancel_flag)
{
	if (cancel_flag) {
		DTimer.del_timer(m_research_technology.timer_id);
	}
	else {
		on_up_technology_level_success();
	}
	m_research_technology.clean_up();
}

void city::on_up_technology_level_success()
{
	m_technologys[m_research_technology.technology_type] += 1;
}

void city::up_resource_level(TResourceIndex_t index)
{
	resource* res = get_resource(index);
	if (NULL == res) {
		return;
	}

	TGameTime_t up_time = calc_resource_up_time(res->get_type(), res->get_level());
	resource_up_info* up_info = DGameServer.allocate_resource_up_info();
	up_info->resource_index = res->get_index();
	up_info->timer_id = DTimer.add_timer(up_time, false, NULL, [&](void* param, TTimerID_t timer_id) {
		del_up_resource_level_core(timer_id, false);
	});
	m_resource_up_infos.push_back(up_info);
}

void city::del_up_resource_level(TTimerID_t timer_id)
{
	del_up_resource_level_core(timer_id, true);
}

void city::del_up_resource_level_core(TTimerID_t timer_id, bool cancel_flag)
{
	for (auto itr = m_resource_up_infos.begin(); itr != m_resource_up_infos.end(); ++itr) {
		resource_up_info* up_info = *itr;
		if (up_info->timer_id != timer_id) {
			continue;
		}

		if (cancel_flag) {
			DTimer.del_timer(timer_id);
		}
		else {
			on_resource_up_level_success(up_info->resource_index);
		}
		DGameServer.deallocate_resource_up_info(up_info);
		m_resource_up_infos.erase(itr);
		break;
	}
}

void city::on_resource_up_level_success(TResourceIndex_t index)
{
	resource* res = get_resource(index);
	if (NULL == res) {
		return;
	}

	res->up_level();
	calc_resource_num();
}

void city::init_resource(TResourceIndex_t index, TResourceType_t resource_type, TLevel_t lvl)
{
	resource* res = DGameServer.allocate_resource();
	res->set_type(resource_type);
	res->set_index(index);
	res->set_level(lvl);
	m_resources.push_back(res);
	calc_resource_num();
}

void city::add_resource(TResourceIndex_t index, TResourceType_t resource_type)
{
	if (resource_type >= RESOURCE_FOOD) {
		if (index < MAX_RESOURCE_INDEX) {
			return;
		}
	}
	else if (index >= MAX_RESOURCE_INDEX) {
		return;
	}

	for (auto res : m_resources) {
		if (res->get_index() == index) {
			return;
		}
	}

	init_resource(index, resource_type, 0);
	up_resource_level(index);
}

void city::del_resource(TResourceIndex_t index)
{
	for (auto itr = m_resources.begin(); itr != m_resources.end(); ++itr) {
		resource* res = (*itr);
		if (res->get_index() != index) {
			continue;
		}

		DGameServer.deallocate_resource(res);
		m_resources.erase(itr);
		break;
	}
}

resource * city::get_resource(TResourceIndex_t index)
{
	for (auto res : m_resources) {
		if (res->get_index() != index) {
			continue;
		}
		return res;
	}
	return NULL;
}

TGameTime_t city::get_last_calc_time() const
{
	return m_last_calc_time;
}

void city::set_last_calc_time(TGameTime_t last_time)
{
	m_last_calc_time = last_time;
}

const game_pos& city::get_pos() const
{
	return m_pos;
}

void city::set_pos(const game_pos& pos)
{
	m_pos = pos;
}

void city::add_npc(TNpcIndex_t index, TLevel_t lvl)
{
	npc* p = DGameServer.allocate_npc();
	p->set_index(index);
	p->set_level(lvl);
	m_npcs.push_back(p);
}

void city::del_npc(TNpcIndex_t index)
{
	for (auto itr = m_npcs.begin(); itr != m_npcs.end(); ++itr) {
		npc* p = *itr;
		if (p->get_index() != index) {
			continue;
		}
		DGameServer.deallocate_npc(p);
		m_npcs.erase(itr);
		return;
	}
}

void city::add_farmland(const game_pos & pos, TResourceType_t resource_type, TLevel_t lvl)
{
	farmland* f = DGameServer.allocate_farmland();
	f->set_type(resource_type);
	f->set_level(lvl);
	f->set_pos(pos);
	m_farmlands.push_back(f);
}

void city::del_farmland(const game_pos & pos)
{
	for (auto itr = m_farmlands.begin(); itr != m_farmlands.end(); ++itr) {
		farmland* f = *itr;
		if (f->get_pos() != pos) {
			continue;
		}
		DGameServer.deallocate_farmland(f);
		m_farmlands.erase(itr);
		return;
	}
}

TResourceNum_t city::get_resource_num(TResourceType_t resource_type)
{
	calc_resource_num();
	return m_resource_num[resource_type];
}

TResourceNum_t city::get_resource_max_num(TResourceNum_t resource_type) const
{
	return INVALID_RESOURCE_NUM;
}

TGameTime_t city::calc_soldier_training_time(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TConsumeType_t consume_type) const
{
	return 0;
}

TGameTime_t city::calc_research_technology_time(TTechnologyType_t technology_type, TConsumeType_t consume_type) const
{
	return 0;
}

TGameTime_t city::calc_resource_up_time(TResourceType_t resoure_type, TLevel_t lvl) const
{
	return INVALID_GAME_TIME;
}

TGameTime_t city::calc_fight_time(TNpcIndex_t npc_id, const game_pos & pos) const
{
	return INVALID_GAME_TIME;
}

void city::calc_resource_num()
{
	TGameTime_t cur_time = DTimeMgr.now_sys_time();
	if (cur_time < m_last_calc_time) {
		log_error("city last calc time is large than current time! last calc time = %u, cur time = %u, x = %d, y = %d", 
			m_last_calc_time, cur_time, m_pos.x, m_pos.y);
		return;
	}

	TGameTime_t diff_time = cur_time - m_last_calc_time;
	for (auto res : m_resources) {
		TResourceNum_t resource_num = res->get_inc_num() * diff_time;
		TResourceType_t resource_type = res->get_type();
		TResourceNum_t max_resource_num = get_resource_max_num(resource_type);
		m_resource_num[resource_type] = min(max_resource_num, resource_num + m_resource_num[resource_type]);
	}
	m_last_calc_time = cur_time;
}

void city::clean_up()
{
	m_last_calc_time = INVALID_GAME_TIME;
	m_pos.clean_up();
	m_research_technology.clean_up();
	for (int i = 0; i < MAX_RESOURCE_TYPE_NUM; ++i) {
		m_resource_num[i] = 0;
	}
	for (int i = 0; i < MAX_SOLDIER_TYPE_NUM; ++i) {
		m_soldier_num[i] = 0;
	}
	m_soldier_trainings.clear();
	m_fight_infos.clear();
}
