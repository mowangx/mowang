
#ifndef _GAME_STRUCT_H_
#define _GAME_STRUCT_H_

#include "base_util.h"

struct game_pos
{
	TPosValue_t x;
	TPosValue_t y;
	game_pos() {
		clean_up();
	}

	bool operator == (const game_pos& rhs) const {
		return x == rhs.x && y == rhs.y;
	}

	bool operator != (const game_pos& rhs) const {
		return x != rhs.x || y != rhs.y;
	}

	void clean_up() {
		x = INVALID_POS_VALUE;
		y = INVALID_POS_VALUE;
	}
};

struct soldier_training_info
{
	TSoldierType_t soldier_type;
	TSoldierNum_t soldier_num;
	TGameTime_t end_time;
	soldier_training_info() {
		clean_up();
	}

	void clean_up() {
		soldier_type = INVALID_SOLDIER_TYPE;
		soldier_num = INVALID_SOLDIER_NUM;
		end_time = INVALID_GAME_TIME;
	}
};

struct technology_info
{
	TTechnologyType_t technology_type;
	TGameTime_t end_time;
	technology_info() {
		clean_up();
	}

	void clean_up() {
		technology_type = INVALID_TECHNOLOGY_TYPE;
		end_time = INVALID_GAME_TIME;
	}
};

#endif // !_GAME_STRUCT_H_