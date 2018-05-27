
#include "resource.h"

#include <algorithm>

#include "timer.h"

resource::resource()
{
	clean_up();
}

resource::~resource()
{
	if (m_up_timer_id > 0) {
		DTimer.del_timer(m_up_timer_id);
	}
	clean_up();
}

TResourceType_t resource::get_type() const
{
	return m_resource_type;
}

void resource::set_type(TResourceType_t resource_type)
{
	m_resource_type = resource_type;
}

TResourceIndex_t resource::get_index() const
{
	return m_index;
}

void resource::set_index(TResourceIndex_t index)
{
	m_index = index;
}

TLevel_t resource::get_level() const
{
	return m_lvl;
}

void resource::add_level(TLevel_t lvl)
{
	m_lvl += lvl;
}

void resource::set_level(TLevel_t lvl)
{
	m_lvl = lvl;
}

void resource::up_level()
{
	if (m_lvl_time > 0) {
		return;
	}

	m_up_timer_id = DTimer.add_timer(calc_up_level_time(), false, NULL, [&](void* param, TTimerID_t timer_id) {
		add_level(1);
		m_up_timer_id = INVALID_TIMER_ID;
	});
}

TGameTime_t resource::get_level_end_time() const
{
	return m_lvl_time;
}

void resource::set_level_end_time(TGameTime_t time)
{
	m_lvl_time = time;
}

TResourceNum_t resource::get_inc_num() const
{
	return INVALID_RESOURCE_NUM;
}

TGameTime_t resource::calc_up_level_time() const
{
	return INVALID_GAME_TIME;
}

void resource::clean_up()
{
	m_resource_type = INVALID_RESOURCE_TYPE;
	m_index = INVALID_RESOURCE_INDEX;
	m_lvl = INVALID_LEVEL;
	m_lvl_time = INVALID_GAME_TIME;
	m_up_timer_id = INVALID_TIMER_ID;
}