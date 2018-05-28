
#ifndef _CITY_H_
#define _CITY_H_

#include <vector>

#include "entity.h"
#include "packet_struct.h"
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
	void fight(TNpcIndex_t npc_id, dynamic_array<soldier_info>& soldiers, const game_pos& pos);
	void gather(TNpcIndex_t npc_id, dynamic_array<soldier_info>& soldiers, const game_pos& pos);
private:
	bool check_npc(TNpcIndex_t npc_id) const;
	bool check_soldiers(dynamic_array<soldier_info>& soldiers) const;

public:
	void random_resources(TLevel_t lvl);

public:
	void add_training_soldier(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TConsumeType_t consume_type);
	void del_training_soldier(TTimerID_t timer_id);
private:
	void del_training_soldier_core(TTimerID_t timer_id, bool cancel_flag);
	void on_training_soldier_success(const soldier_training_info& training_info);

public:
	void up_technology_level(TTechnologyType_t technology_type, TConsumeType_t consume_type);
	void del_up_technology_level();
private:
	void del_up_technology_level_core(bool cancel_flag);
	void on_up_technology_level_success();

public:
	void up_resource_level(TResourceIndex_t index);
	void del_up_resource_level(TTimerID_t timer_id);
private:
	void del_up_resource_level_core(TTimerID_t timer_id, bool cancel_flag);
	void on_resource_up_level_success(TResourceIndex_t index);

public:
	void init_resource(TResourceIndex_t index, TResourceType_t resource_type, TLevel_t lvl);
	void add_resource(TResourceIndex_t index, TResourceType_t resource_type);
	void del_resource(TResourceIndex_t index);
private:
	resource* get_resource(TResourceIndex_t index);
	
public:
	TGameTime_t get_last_calc_time() const;
	void set_last_calc_time(TGameTime_t last_time);

	const game_pos& get_pos() const;
	void set_pos(const game_pos& pos);

	void add_npc(TNpcIndex_t index, TLevel_t lvl);
	void del_npc(TNpcIndex_t index);

	void add_farmland(const game_pos& pos, TResourceType_t resource_type, TLevel_t lvl);
	void del_farmland(const game_pos& pos);

	TResourceNum_t get_resource_num(TResourceType_t resource_type);
	TResourceNum_t get_resource_max_num(TResourceNum_t resource_type) const;

private:
	TGameTime_t calc_soldier_training_time(TSoldierType_t soldier_type, TSoldierNum_t soldier_num, TConsumeType_t consume_type) const;
	TGameTime_t calc_research_technology_time(TTechnologyType_t technology_type, TConsumeType_t consume_type) const;
	TGameTime_t calc_resource_up_time(TResourceType_t resoure_type, TLevel_t lvl) const;
	TGameTime_t calc_fight_time(TNpcIndex_t npc_id, const game_pos& pos) const;

private:
	void calc_resource_num();

private:
	void clean_up();

private:
	TGameTime_t m_last_calc_time;
	game_pos m_pos;
	technology_info m_research_technology;		// 正在研究的科技
	std::array<TResourceNum_t, MAX_RESOURCE_TYPE_NUM> m_resource_num;
	std::array<TSoldierNum_t, MAX_SOLDIER_TYPE_NUM> m_soldier_num;
	std::array<TTechnologyType_t, MAX_TECHNOLOGY_NUM> m_technologys;
	std::vector<resource*> m_resources;		// 建筑、矿产
	std::vector<npc*> m_npcs;
	std::vector<farmland*> m_farmlands;
	std::vector<soldier_training_info*> m_soldier_trainings;
	std::vector<resource_up_info*> m_resource_up_infos;
	std::vector<fight_info> m_fight_infos;
};

#endif // !_CITY_H_

