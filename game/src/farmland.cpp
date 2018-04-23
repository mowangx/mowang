
#include "farmland.h"

farmland::farmland()
{
	clean_up();
}

farmland::~farmland()
{
	clean_up();
}

TResourceType_t farmland::get_type() const
{
	return m_resource_type;
}

void farmland::set_type(TResourceType_t resource_type)
{
	// fix formula = (pos.x + pos.y + time.hour) % MAX_TYPE_NUM
	m_resource_type = resource_type;
}

TLevel_t farmland::get_level() const
{
	return m_lvl;
}

void farmland::set_level(TLevel_t lvl)
{
	// fix formula = (pos.x + pos.y + time.hour) % MAX_LEVEL
	m_lvl = lvl;
}

const game_pos & farmland::get_pos() const
{
	return m_pos;
}

void farmland::set_pos(const game_pos & pos)
{
	m_pos = pos;
}

void farmland::clean_up()
{
	m_resource_type = INVALID_RESOURCE_TYPE;
	m_lvl = INVALID_LEVEL;
	m_pos.clean_up();
}
