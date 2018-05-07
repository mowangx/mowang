
#ifndef _TIMER_H_
#define _TIMER_H_

#include <vector>

#include "base_util.h"
#include "singleton.h"

class entity;
struct timer_node;

class timer : public singleton<timer>
{
public:
	timer();
	~timer();

public:
	void init();
	void update(TGameTime_t diff);
private:
	void do_once_update();

public:
	void add_timer(TGameTime_t delay,  bool repeat, entity* e, uint8 data);
	void del_timer(timer_node* node);
private:
	void add_timer_core(timer_node*& node, TGameTime_t delay, bool repeat, entity* e, uint8 data);

private:
	void add_node(timer_node*& cur_node, timer_node* node);
	void proc_node(timer_node* node);
	void proc_up_layer_node(std::vector<timer_node*>& cur_node, timer_node*& node, int (*calc_index_func)(uint16, uint8));

private:
	void clean_up();

private:
	TGameTime_t m_last_time;
	uint8 m_second_index;
	uint8 m_minute_index;
	uint8 m_hour_index;
	uint8 m_day_index;
	std::vector<timer_node*> m_second_nodes;
	std::vector<timer_node*> m_minute_nodes;
	std::vector<timer_node*> m_hour_nodes;
	std::vector<timer_node*> m_day_nodes;
};

#define DTimer singleton<timer>::get_instance()

#endif // !_TIMER_H_
