#include "timer.h"
#include "game_struct.h"
#include "common_const.h"
#include "time_manager.h"
#include "entity.h"

timer::timer()
{
	clean_up();
}

timer::~timer()
{
	clean_up();
}

void timer::init()
{
	for (int i = 0; i < SECOND_IN_MINUTE; ++i) {
		m_second_nodes.push_back(NULL);
	}
	for (int i = 0; i < MINUTE_IN_HOUR; ++i) {
		m_minute_nodes.push_back(NULL);
	}
	for (int i = 0; i < HOUR_IN_DAY; ++i) {
		m_hour_nodes.push_back(NULL);
	}
	for (int i = 0; i < 30; ++i) {
		m_day_nodes.push_back(NULL);
	}
	m_last_time = DTimeMgr.now_sys_time();
}

void timer::update(TGameTime_t diff)
{
	TGameTime_t cur_time = DTimeMgr.now_sys_time();
	if (cur_time == m_last_time) {
		return;
	}
	int second = cur_time - m_last_time;
	m_last_time = cur_time;
	if (second == 1) {
		do_once_update();
		return;
	}
	for (int i = 0; i < second; ++i) {
		do_once_update();
	}
}

void timer::do_once_update()
{
	sint32 second_index = 1 + m_second_index;
	m_second_index = second_index % SECOND_IN_MINUTE;
	if (second_index >= SECOND_IN_MINUTE) {
		int minute_index = second_index / SECOND_IN_MINUTE + m_minute_index;
		m_minute_index = minute_index % MINUTE_IN_HOUR;
		if (minute_index >= MINUTE_IN_HOUR) {
			int hour_index = minute_index / MINUTE_IN_HOUR + m_hour_index;
			m_hour_index = hour_index % HOUR_IN_DAY;
			if (hour_index >= HOUR_IN_DAY) {
				int day_index = hour_index / HOUR_IN_DAY + m_day_index;
				m_day_index = day_index % 30;
				auto hour_func = [](uint16 slot_index_1, uint8 slot_index_2) -> int { return ((slot_index_1 >> 6) & 0x1F); };
				proc_up_layer_node(m_hour_nodes, m_day_nodes[m_day_index], hour_func);
				m_day_nodes[m_day_index] = NULL;
			}
			auto minute_func = [](uint16 slot_index_1, uint8 slot_index_2) -> int { return (slot_index_1 & 0x3F); };
			proc_up_layer_node(m_minute_nodes, m_hour_nodes[m_hour_index], minute_func);
			m_hour_nodes[m_hour_index] = NULL;
		}
		auto second_func = [](uint16 slot_index_1, uint8 slot_index_2) -> int { return (slot_index_2 & 0x3F); };
		proc_up_layer_node(m_second_nodes, m_minute_nodes[m_minute_index], second_func);
		m_minute_nodes[m_minute_index] = NULL;
	}
	proc_node(m_second_nodes[m_second_index]);
	m_second_nodes[m_second_index] = NULL;
}

void timer::add_timer(TGameTime_t delay, bool repeat, entity* e, uint8 data)
{
	timer_node* node = new timer_node;
	add_timer_core(node, delay, repeat, e, data);
}

void timer::del_timer(timer_node* node)
{
	if (NULL != node->pre) {
		node->pre->next = node->next;
	}
	if (NULL != node->next) {
		node->next->pre = node->pre;
	}
	delete node;
}

void timer::add_timer_core(timer_node *& node, TGameTime_t delay, bool repeat, entity* e, uint8 data)
{
	node->delay = delay;
	node->e = e;
	node->data = data;
	int day_index = int(delay / SECOND_IN_DAY);
	int slot_day_index = (day_index + m_day_index) % 30;
	int hour_index = int(delay / SECOND_IN_HOUR);
	int slot_hour_index = (hour_index + m_hour_index) % HOUR_IN_DAY;
	int minute_index = int(delay / SECOND_IN_MINUTE);
	int slot_minute_index = (minute_index + m_minute_index) % SECOND_IN_MINUTE;
	int second_index = delay % SECOND_IN_MINUTE;
	int slot_second_index = (second_index + m_second_index) % SECOND_IN_MINUTE;

	node->slot_index_1 = (uint16)((slot_day_index << 11) + (slot_hour_index << 6) + slot_minute_index);
	node->slot_index_2 = repeat ? 1 : 0;
	node->slot_index_2 = (uint8)((node->slot_index_2 << 7) + slot_second_index);

	if (day_index > 0) {
		add_node(m_day_nodes[slot_day_index], node);
		return;
	}

	if (hour_index > 0) {
		add_node(m_hour_nodes[slot_hour_index], node);
		return;
	}

	if (minute_index > 0) {
		add_node(m_minute_nodes[slot_minute_index], node);
	}
	else {
		add_node(m_second_nodes[slot_second_index], node);
	}
}

void timer::add_node(timer_node*& cur_node, timer_node* node)
{
	if (NULL == cur_node) {
		cur_node = node;
		cur_node->next = NULL;
		cur_node->pre = NULL;
		return;
	}
	node->next = cur_node->next;
	node->pre = cur_node;
	cur_node->next = node;
	if (NULL != node->next) {
		node->next->pre = node;
	}
}

void timer::proc_node(timer_node* node)
{
	while (NULL != node) {
		node->e->on_timer(node->data);
		timer_node* tmp_node = node;
		node = node->next;
		if ((tmp_node->slot_index_2 & 0x80) > 0) {
			add_timer_core(tmp_node, tmp_node->delay, true, tmp_node->e, tmp_node->data);
		}
		else {
			del_timer(tmp_node);
		}
	}
}

void timer::proc_up_layer_node(std::vector<timer_node*>& cur_node, timer_node*& node, int(*calc_index_func)(uint16, uint8))
{
	while (NULL != node) {
		int index = (*calc_index_func)(node->slot_index_1, node->slot_index_2);
		timer_node* tmp_node = node;
		node = node->next;
		add_node(cur_node[index], tmp_node);
	}
}

void timer::clean_up()
{
	m_last_time = INVALID_GAME_TIME;
	m_second_index = 0;
	m_minute_index = 0;
	m_hour_index = 0;
	m_day_index = 0;
	m_second_nodes.clear();
	m_minute_nodes.clear();
	m_hour_nodes.clear();
	m_day_nodes.clear();
}
