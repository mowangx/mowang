
#ifndef _CITY_H_
#define _CITY_H_

#include <vector>

#include "entity.h"
#include "game_struct.h"

class resource;
class farmland;
class npc;

class city : public entity
{
public:
	city();
	virtual ~city();

public:
	void random_resources(TLevel_t lvl);

public:
	void add_training_soldier(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TConsumeType_t consume_type);
	void del_training_soldier(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TGameTime_t end_time);

	void up_technology_level(TTechnologyType_t technology_type, TConsumeType_t consume_type);
	void del_up_technology_level();

private:
	void on_training_soldier_success(const soldier_training_info& training_info);
	void on_up_technology_level_success();

public:
	void add_resource(uint8 index, uint8 resource_type);
	void del_resource(uint8 index);
	void up_level(uint8 index);
	
	const game_pos& get_pos() const;
	void set_pos(const game_pos& pos);

	void add_npc(TNpcIndex_t index, TLevel_t lvl);
	void del_npc(TNpcIndex_t index);

	void add_farmland(const game_pos& pos, TResourceType_t resource_type, TLevel_t lvl);
	void del_farmland(const game_pos& pos);

private:
	TGameTime_t calc_soldier_training_time(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TConsumeType_t consume_type);
	TGameTime_t calc_research_technology_time(TTechnologyType_t technology_type, TConsumeType_t consume_type);

	void remove_timer_id(TTimerID_t timer_id);

private:
	void clean_up();

private:
	game_pos m_pos;
	technology_info m_research_technology;		// 正在研究的科技
	std::array<TResourceNum_t, MAX_RESOURCE_TYPE_NUM> m_resource_inc_num;
	std::array<TResourceNum_t, MAX_RESOURCE_TYPE_NUM> m_resource_num;
	std::array<TSoldierNum_t, MAX_SOLDIER_TYPE_NUM> m_soldier_num;
	std::array<TTechnologyType_t, MAX_TECHNOLOGY_NUM> m_technologys;
	std::vector<resource*> m_resources;		// 建筑、矿产
	std::vector<npc*> m_npcs;
	std::vector<farmland*> m_farmlands;
	std::vector<soldier_training_info> m_soldier_trainings;
	std::vector<TTimerID_t> m_timer_ids;
};

#endif // !_CITY_H_

