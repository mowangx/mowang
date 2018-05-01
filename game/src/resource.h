
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "entity.h"

class resource : public entity
{
public:
	resource();
	virtual ~resource();

public:
	TResourceType_t get_type() const;
	void set_type(TResourceType_t resource_type);

	TResourceIndex_t get_index() const;
	void set_index(TResourceIndex_t index);

	TLevel_t get_level() const;
	void add_level(TLevel_t lvl);
	void set_level(TLevel_t lvl);

	TGameTime_t get_level_end_time() const;
	void set_level_end_time(TGameTime_t time);

private:
	void clean_up();

private:
	TResourceType_t m_resource_type;
	TResourceIndex_t m_index;
	TLevel_t m_lvl;
	TGameTime_t m_lvl_time;
};

#endif // !_RESOURCE_H_