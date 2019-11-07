
#include "executor_manager.h"
#include "mysql_conn.h"
#include "server_enum.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"

executor_manager::executor_manager()
{
	m_db = NULL;
	clean_up();
}

executor_manager::~executor_manager()
{
	if (NULL != m_db) {
		delete m_db;
		m_db = NULL;
	}
	clean_up();
}

bool executor_manager::init()
{
	m_db = new mysql_conn();
	if (NULL == m_db) {
		return false;
	}

	//bool status = m_db->init("127.0.0.1", 3306, "mwrootdb", "a23A5678!", "test");
	//if (!status) {
	//	log_error("connect db failed!");
	//	return false;
	//}

	//char result[65500];
	//int len = 0;
	//m_db->query("role", "", "`id`, `name`, unix_timestamp(birthday)", result, len);

	DRegisterServerRpc(this, executor_manager, add_executor, 6);
	return true;
}

void executor_manager::update(TGameTime_t diff)
{
	for (auto itr = m_executors.begin(); itr != m_executors.end(); ++itr) {
		std::vector<db_opt_info*>& executors = itr->second;
		for (auto opt_itr = executors.begin(); opt_itr != executors.end(); ++opt_itr) {
			db_opt_info* opt_info = *opt_itr;
			executor(opt_info);
		}
		executors.clear();
	}
}

void executor_manager::executor(db_opt_info* opt_info)
{
	rpc_client* rpc = DRpcWrapper.get_client_by_socket_index(opt_info->socket_index);
	if (NULL == rpc) {
		return;
	}
	if (opt_info->opt_type == DB_OPT_QUERY) {
		char db_result[65000];
		memset(db_result, 0, 65000);
		int len = 0;
		//bool status = m_db->query(opt_info->table_name.c_str(), opt_info->condition.c_str(), opt_info->fields.c_str(), db_result, len);
		//if (opt_info->table_name == "account") {
		//	rpc_param_fill<uint64, uint64>::fill_param(0x123456, db_result, len);
		//	rpc_param_fill<uint64, uint64>::fill_param(0x321789, db_result, len);
		//}
		//else {
		//	rpc_param_fill<uint8, uint8>::fill_param(0x33, db_result, len);
		//	rpc_param_fill<uint8, uint8>::fill_param(0x22, db_result, len);
		//	dynamic_string name = "test role name";
		//	rpc_param_fill<dynamic_string, dynamic_string>::fill_param(name, db_result, len);
		//}
		binary_data result(db_result, len);
		rpc->call_remote_func("on_opt_db_with_result", opt_info->opt_id, true, result);
	}
	else {
		if (opt_info->opt_type == DB_OPT_UPDATE) {
			//m_db->update(opt_info->table_name.c_str(), opt_info->condition.c_str(), opt_info->fields.c_str());
		}
		else if (opt_info->opt_type == DB_OPT_INSERT) {
			//m_db->insert(opt_info->table_name.c_str(), opt_info->condition.c_str(), opt_info->fields.c_str());
		}
		else if (opt_info->opt_type == DB_OPT_DELETE) {
			//m_db->remove(opt_info->table_name.c_str(), opt_info->condition.c_str());
		}
		rpc->call_remote_func("on_opt_db_with_status", opt_info->opt_id, true);
	}
	m_mem_pool.deallocate(opt_info);
}

void executor_manager::add_executor(TSocketIndex_t socket_index, uint8 opt_type, uint64 opt_id, const dynamic_string& table_name, const dynamic_string& query, const dynamic_string& fields)
{
	log_info("add executor, opt type %u, opt id %" I64_FMT "u, table name %s", opt_type, opt_id, table_name.data());
	db_opt_info* opt_info = m_mem_pool.allocate();
	opt_info->opt_type = opt_type;
	opt_info->opt_id = opt_id;
	opt_info->socket_index = socket_index;
	opt_info->table_name = table_name.data();
	opt_info->condition = query.data();
	opt_info->fields = fields.data();
	auto itr = m_executors.find(opt_info->table_name);
	if (itr == m_executors.end()) {
		m_executors[opt_info->table_name] = std::vector<db_opt_info*>();
		itr = m_executors.find(opt_info->table_name);
	}
	std::vector<db_opt_info*>& executors = itr->second;
	executors.push_back(opt_info);
}

void executor_manager::clean_up()
{
	m_executors.clear();
}
