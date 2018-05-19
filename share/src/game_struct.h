
#ifndef _GAME_STRUCT_H_
#define _GAME_STRUCT_H_

#include <functional>

#include "base_util.h"

struct timer_node
{
	timer_node* pre;
	timer_node* next;
	TGameTime_t delay;
	uint16 slot_index_1;	// 5 bit day index, 5 bit hour index, 6 bit minute index
	uint8 slot_index_2;	// 1 bit repeate, 6 bit second index
	std::function<void(void*)> callback;
	void* param;
	timer_node() {
		clean_up();
	}

	void clean_up() {
		pre = NULL;
		next = NULL;
		delay = INVALID_GAME_TIME;
		slot_index_1 = 0;
		slot_index_2 = 0;
		callback = NULL;
		param = NULL;
	}
};

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

struct socket_kick_info
{
	TSocketIndex_t socket_index;
	TGameTime_t kick_time;
	socket_kick_info() {
		clean_up();
	}

	bool operator == (const socket_kick_info& rhs) const {
		return socket_index == rhs.socket_index;
	}

	void clean_up() {
		socket_index = INVALID_SOCKET_INDEX;
		kick_time = INVALID_GAME_TIME;
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