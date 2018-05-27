
#ifndef _FARMLAND_H_
#define _FARMLAND_H_

#include "entity.h"
#include "packet_struct.h"

class farmland : public entity
{
public:
	farmland();
	virtual ~farmland();

public:
	TResourceType_t get_type() const;
	void set_type(TResourceType_t resource_type);

	TLevel_t get_level() const;
	void set_level(TLevel_t lvl);

	const game_pos& get_pos() const;
	void set_pos(const game_pos& pos);

private:
	void clean_up();

private:
	TResourceType_t m_resource_type;
	TLevel_t m_lvl;
	game_pos m_pos;
};

#endif // !_FARMLAND_H_
