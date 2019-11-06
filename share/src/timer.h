
#ifndef _TIMER_H_
#define _TIMER_H_

#include <vector>
#include <unordered_map>

#include "singleton.h"
#include "memory_pool.h"
#include "server_struct.h"

class timer : public singleton<timer>
{
public:
	timer();
	~timer();

public:
	void init();
	void update(TGameTime_t diff);
private:
	void proc_second_nodes();
	void proc_minute_nodes();
	void proc_hour_nodes();
	void proc_day_nodes();

public:
	TTimerID_t add_timer(TGameTime_t delay, bool repeat, void* param, const std::function<void(void*, TTimerID_t)>& callback);
	void del_timer(TTimerID_t timer_id);
private:
	void add_timer_core(timer_node*& node, TGameTime_t delay, bool repeat, void* param, const std::function<void(void*, TTimerID_t)>& callback);
	void del_timer_core(timer_node* node);

private:
	void add_node(timer_node*& cur_node, timer_node* node);
	void proc_node(timer_node* node);
	void proc_up_layer_node(std::vector<timer_node*>& cur_node, timer_node*& node, int (*calc_index_func)(uint16, uint8));

private:
	TTimerID_t gen_timer_id();

private:
	void clean_up();

private:
	TTimerID_t m_timer_id;
	TGameTime_t m_last_time;
	uint8 m_second_index;
	uint8 m_minute_index;
	uint8 m_hour_index;
	uint8 m_day_index;
	std::vector<timer_node*> m_second_nodes;
	std::vector<timer_node*> m_minute_nodes;
	std::vector<timer_node*> m_hour_nodes;
	std::vector<timer_node*> m_day_nodes;
	std::unordered_map<TTimerID_t, timer_node*> m_timer_id_2_node;
	obj_memory_pool<timer_node, 100> m_node_pool;
};

#define DTimer singleton<timer>::get_instance()

#endif // !_TIMER_H_
