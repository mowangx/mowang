
#ifndef _SERVER_STRUCT_H_
#define _SERVER_STRUCT_H_

#include <functional>

#include "packet_struct.h"

struct timer_node
{
	timer_node* pre;
	timer_node* next;
	TGameTime_t delay;
	uint16 slot_index_1;	// 5 bit day index, 5 bit hour index, 6 bit minute index
	uint8 slot_index_2;	// 1 bit repeate, 6 bit second index
	TTimerID_t timer_id;
	std::function<void(void*, TTimerID_t)> callback;
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
		timer_id = INVALID_TIMER_ID;
		callback = NULL;
		param = NULL;
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

#endif // !_SERVER_STRUCT_H_