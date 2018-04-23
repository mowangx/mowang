
#include "npc.h"
#include "game_enum.h"

npc::npc()
{
	clean_up();
}

npc::~npc()
{
	clean_up();
}

TNpcIndex_t npc::get_index() const
{
	return m_index;
}

void npc::set_index(TNpcIndex_t index)
{
	m_index = index;
}

TLevel_t npc::get_level() const
{
	return m_lvl;
}

void npc::add_level(TLevel_t lvl)
{
	m_lvl += lvl;
}

void npc::set_level(TLevel_t lvl)
{
	m_lvl = lvl;
}

TAttrValue_t npc::get_attr_value(TAttrType_t attr_type) const
{
	if (!check_attr_type(attr_type)) {
		return 0;
	}

	return m_attrs[attr_type];
}

void npc::add_attr_value(TAttrType_t attr_type, TAttrValue_t attr_value)
{
	if (!check_attr_type(attr_type)) {
		return;
	}

	m_attrs[attr_type] += attr_value;
}

void npc::set_attr_value(TAttrType_t attr_type, TAttrValue_t attr_value)
{
	if (!check_attr_type(attr_type)) {
		return;
	}

	m_attrs[attr_type] = attr_value;
}

bool npc::check_attr_type(TAttrType_t attr_type) const
{
	return (attr_type >= ATTR_AFFAIRS && attr_type <= ATTR_STRATEGY);
}

void npc::clean_up()
{
	m_index = INVALID_NPC_INDEX;
	m_lvl = INVALID_LEVEL;
	for (int i = 0; i < MAX_ATTR_NUM; ++i) {
		m_attrs[i] = INVALID_ATTR_VALUE;
	}
}
