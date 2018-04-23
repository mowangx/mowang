
#include "city.h"
#include "game_enum.h"
#include "game_server.h"
#include "time_manager.h"
#include "resource.h"
#include "farmland.h"
#include "npc.h"

city::city()
{
	clean_up();
	m_resources.clear();
	m_up_level_resources.clear();
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

	for (auto res : m_up_level_resources) {
		DGameServer.deallocate_resource(res);
	}
	m_up_level_resources.clear();

	for (auto p : m_npcs) {
		DGameServer.deallocate_npc(p);
	}
	m_npcs.clear();

	for (auto f : m_farmlands) {
		DGameServer.deallocate_farmland(f);
	}
	m_farmlands.clear();
}

void city::update(TGameTime_t diff)
{
	TGameTime_t cur_time = DTimeMgr.now_sys_time();
	std::vector<resource*> del_resources;

	for (auto res: m_up_level_resources) {
		if (res->get_level_end_time() > cur_time) {
			continue;
		}
		res->set_level_end_time(INVALID_GAME_TIME);
		res->add_level(1);
		del_resources.push_back(res);
	}

	if (del_resources.size() > 0) {
		for (auto res : del_resources) {
			remove_up_level_resource(res);
		}
	}
}

void city::random_resources(TLevel_t lvl)
{
}

void city::add_training_soldier(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TConsumeType_t consume_type)
{
	soldier_training_info soldier_info;
	soldier_info.soldier_type = soldier_type;
	soldier_info.soldier_num = soldier_num;
	soldier_info.end_time = DTimeMgr.now_sys_time() + calc_soldier_training_time(soldier_type, soldier_num, consume_type);
	m_soldier_trainings.push_back(soldier_info);
}

void city::del_training_soldier(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TGameTime_t end_time)
{
	for (auto itr = m_soldier_trainings.begin(); itr != m_soldier_trainings.end(); ++itr) {
		soldier_training_info& soldier_info = *itr;
		if (soldier_info.soldier_type != soldier_type || soldier_info.soldier_num != soldier_num || soldier_info.end_time != end_time) {
			continue;
		}
		m_soldier_trainings.erase(itr);
		return;
	}
}

void city::up_technology_level(TTechnologyType_t technology_type, TConsumeType_t consume_type)
{
	m_research_technology.technology_type = technology_type;
	m_research_technology.end_time = DTimeMgr.now_sys_time() + calc_research_technology_time(technology_type, consume_type);
}

void city::del_up_technology_level()
{
	m_research_technology.clean_up();
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

		for (auto del_itr = m_up_level_resources.begin(); del_itr != m_up_level_resources.end(); ++del_itr) {
			resource* del_res = (*del_itr);
			if (del_res->get_index() != index) {
				continue;
			}
			m_up_level_resources.erase(del_itr);
			break;
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

		if (res->get_level_end_time() > 0) {
			return;
		}

		TGameTime_t lvl_end_time = DTimeMgr.now_sys_time() + 10;
		res->set_level_end_time(lvl_end_time);
		m_up_level_resources.push_back(res);
	}
}

const game_pos & city::get_pos() const
{
	return m_pos;
}

void city::set_pos(const game_pos & pos)
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

void city::remove_up_level_resource(resource * res)
{
	for (auto itr = m_up_level_resources.begin(); itr != m_up_level_resources.end(); ++itr) {
		resource* r = *itr;
		if (r->get_index() != res->get_index()) {
			continue;
		}
		m_up_level_resources.erase(itr);
		return;
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
}
