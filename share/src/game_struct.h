
#ifndef _GAME_STRUCT_H_
#define _GAME_STRUCT_H_

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

struct soldier_training_info
{
	TSoldierType_t soldier_type;
	TSoldierNum_t soldier_num;
	TGameTime_t end_time;
	TTimerID_t timer_id;
	soldier_training_info() {
		clean_up();
	}

	void clean_up() {
		soldier_type = INVALID_SOLDIER_TYPE;
		soldier_num = INVALID_SOLDIER_NUM;
		end_time = INVALID_GAME_TIME;
		timer_id = INVALID_TIMER_ID;
	}
};

struct technology_info
{
	TTechnologyType_t technology_type;
	TGameTime_t end_time;
	TTimerID_t timer_id;
	technology_info() {
		clean_up();
	}

	void clean_up() {
		technology_type = INVALID_TECHNOLOGY_TYPE;
		end_time = INVALID_GAME_TIME;
		timer_id = INVALID_TIMER_ID;
	}
};

struct resource_up_info
{
	TResourceIndex_t resource_index;
	TTimerID_t timer_id;
	resource_up_info() {
		clean_up();
	}

	void clean_up() {
		resource_index = INVALID_RESOURCE_INDEX;
		timer_id = INVALID_TIMER_ID;
	}
};

struct fight_info
{
	TRoleID_t role_id;
	TNpcIndex_t npc_id;
	game_pos src_pos;
	game_pos dest_pos;;
	TGameTime_t fight_time;
	TTimerID_t timer_id;
	dynamic_array<soldier_info> soldiers;
	fight_info() {
		clean_up();
	}

	void clean_up() {
		role_id = INVALID_ROLE_ID;
		npc_id = INVALID_NPC_INDEX;
		src_pos.clean_up();
		dest_pos.clean_up();
		fight_time = INVALID_GAME_TIME;
		timer_id = INVALID_TIMER_ID;
	}
};

struct room_role_info
{
	bool status;
	mailbox_info mailbox;
	proxy_info proxy;
	std::vector<TCardIndex_t> cards;

	room_role_info() {
		clean_up();
	}

	void clean_up() {
		status = false;
	}
};

#endif // !_GAME_STRUCT_H_