
#ifndef _NPC_H_
#define _NPC_H_

#include "entity.h"

class npc : public entity
{
public:
	npc();
	virtual ~npc();

public:
	TNpcIndex_t get_index() const;
	void set_index(TNpcIndex_t index);

	TLevel_t get_level() const;
	void add_level(TLevel_t lvl);
	void set_level(TLevel_t lvl);

	TAttrValue_t get_attr_value(TAttrType_t attr_type) const;
	void add_attr_value(TAttrType_t attr_type, TAttrValue_t attr_value);
	void set_attr_value(TAttrType_t attr_type, TAttrValue_t attr_value);

private:
	bool check_attr_type(TAttrType_t attr_type) const;

	void clean_up();

public:
	TNpcIndex_t m_index;
	TLevel_t m_lvl;
	TAttrValue_t m_attrs[MAX_ATTR_NUM];
};

#endif // !_NPC_H_

