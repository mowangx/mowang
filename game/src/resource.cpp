
#include "resource.h"

resource::resource()
{
	clean_up();
}

resource::~resource()
{
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

TGameTime_t resource::get_level_end_time() const
{
	return m_lvl_time;
}

void resource::set_level_end_time(TGameTime_t time)
{
	m_lvl_time = time;
}

void resource::clean_up()
{
	m_resource_type = INVALID_RESOURCE_TYPE;
	m_index = INVALID_RESOURCE_INDEX;
	m_lvl = INVALID_LEVEL;
	m_lvl_time = INVALID_GAME_TIME;
}