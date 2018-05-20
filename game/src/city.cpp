
#include "city.h"
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
	clean_up();

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

	for (auto itr = m_timer_ids.begin(); itr != m_timer_ids.end(); ++itr) {
		DTimer.del_timer(*itr);
	}
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
	m_soldier_trainings.push_back(*soldier_info);
	TTimerID_t training_timer_id = DTimer.add_timer(training_time, false, (void*)soldier_info, [&](void* param, TTimerID_t timer_id) {
		soldier_training_info* training_info = (soldier_training_info*)param;
		on_training_soldier_success(*training_info);
		DGameServer.deallocate_soldier_training(training_info);
		remove_timer_id(timer_id);
	});
	m_timer_ids.push_back(training_timer_id);
}

void city::del_training_soldier(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TGameTime_t end_time)
{
	soldier_training_info training_info(soldier_type, soldier_num, end_time);
	auto itr = std::find(m_soldier_trainings.begin(), m_soldier_trainings.end(), training_info);
	if (itr != m_soldier_trainings.end()) {
		m_soldier_trainings.erase(itr);
	}
}

void city::up_technology_level(TTechnologyType_t technology_type, TConsumeType_t consume_type)
{
	TGameTime_t up_time = calc_research_technology_time(technology_type, consume_type);
	m_research_technology.technology_type = technology_type;
	m_research_technology.end_time = DTimeMgr.now_sys_time() + up_time;
	TTimerID_t up_timer_id = DTimer.add_timer(up_time, false, NULL, [&](void* param, TTimerID_t timer_id) {
		on_up_technology_level_success();
		remove_timer_id(timer_id);
	});
	m_timer_ids.push_back(up_timer_id);
}

void city::del_up_technology_level()
{
	m_research_technology.clean_up();
}

void city::on_training_soldier_success(const soldier_training_info & training_info)
{
	m_soldier_num[training_info.soldier_type] += training_info.soldier_num;
	del_training_soldier(training_info.soldier_type, training_info.soldier_num, training_info.end_time);
}

void city::on_up_technology_level_success()
{
	m_technologys[m_research_technology.technology_type] += 1;
	del_up_technology_level();
}

void city::add_resource(uint8 index, uint8 resource_type)
{
	// 建筑索引小于64，资源索引大于64
	if (resource_type >= RESOURCE_FOOD) {
		if (index < 64) {
			return;
		}
	}
	else if (index >= 64) {
		return;
	}

	for (auto res : m_resources) {
		if (res->get_index() == index) {
			return;
		}
	}

	resource* res = DGameServer.allocate_resource();
	res->set_type(resource_type);
	res->set_index(index);
	res->set_level(1);
	m_resources.push_back(res);
}

void city::del_resource(uint8 index)
{
	for (auto itr = m_resources.begin(); itr != m_resources.end(); ++itr) {
		resource* res = (*itr);
		if (res->get_index() != index) {
			continue;
		}

		DGameServer.deallocate_resource(res);
		m_resources.erase(itr);
		return;
	}
}

void city::up_level(uint8 index)
{
	for (auto res : m_resources) {
		if (res->get_index() != index) {
			continue;
		}

		res->up_level();
	}
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

TGameTime_t city::calc_soldier_training_time(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TConsumeType_t consume_type)
{
	return 0;
}

TGameTime_t city::calc_research_technology_time(TTechnologyType_t technology_type, TConsumeType_t consume_type)
{
	return 0;
}

void city::remove_timer_id(TTimerID_t timer_id)
{
	auto itr = std::find(m_timer_ids.begin(), m_timer_ids.end(), timer_id);
	if (itr != m_timer_ids.end()) {
		m_timer_ids.erase(itr);
	}
	else {
		log_error("remove timer id failed! timer id = %" I64_FMT "u", timer_id);
	}
}

void city::clean_up()
{
	m_pos.clean_up();
	m_research_technology.clean_up();
	for (int i = 0; i < MAX_RESOURCE_TYPE_NUM; ++i) {
		m_resource_inc_num[i] = 0;
		m_resource_num[i] = 0;
	}
	for (int i = 0; i < MAX_SOLDIER_TYPE_NUM; ++i) {
		m_soldier_num[i] = 0;
	}
	m_soldier_trainings.clear();
	m_timer_ids.clear();
}
