
#ifndef _EXECUTOR_MANAGER_H_
#define _EXECUTOR_MANAGER_H_

#include <map>
#include <unordered_map>

#include "singleton.h"
#include "dynamic_array.h"
#include "db_struct.h"
#include "memory_pool.h"

class db_conn;

class executor_manager : public singleton<executor_manager>
{
public:
	executor_manager();
	~executor_manager();

public:
	bool init();
	void update(TGameTime_t diff);
private:
	void executor(db_opt_info* opt_info);

public:
	void add_executor(TSocketIndex_t socket_index, uint8 opt_type, uint64 opt_id, const dynamic_string& table_name, const dynamic_string& fields, const dynamic_string& query);

private:
	void clean_up();

private:
	db_conn * m_db;
	obj_memory_pool<db_opt_info, 100> m_mem_pool;
	std::map<std::string, std::vector<db_opt_info*>> m_executors;
};

#define DExecutorMgr singleton<executor_manager>::get_instance()

#endif // !_EXECUTOR_MANAGER_H_
