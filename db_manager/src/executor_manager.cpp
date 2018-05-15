
#include "executor_manager.h"
#include "mysql_conn.h"
#include "game_enum.h"
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
	//m_db = new mysql_conn();
	//if (NULL == m_db) {
	//	return false;
	//}

	//bool result = m_db->init("127.0.0.1", 3306, "root", "123456", "test");
	//if (!result) {
	//	log_error("connect db failed!");
	//	return false;
	//}

	DRegisterServerRpc(this, executor_manager, add_executor, 6);
	return true;
}

void executor_manager::update(TGameTime_t diff)
{
	for (auto itr = m_executors.begin(); itr != m_executors.end(); ++itr) {
		std::unordered_map<std::string, db_opt_info*>& executors = itr->second;
		for (auto opt_itr = executors.begin(); opt_itr != executors.end(); ++opt_itr) {
			db_opt_info* opt_info = opt_itr->second;
			executor(opt_info);
		}
		executors.clear();
	}
}

void executor_manager::executor(db_opt_info* opt_info)
{
	game_process_info process_info;
	if (!DRpcWrapper.get_server_simple_info_by_socket_index(process_info, opt_info->socket_index)) {
		return;
	}
	rpc_client* rpc = DRpcWrapper.get_client(process_info);
	if (NULL == rpc) {
		return;
	}
	if (opt_info->opt_type == DB_OPT_QUERY) {
		//m_db->query(opt_info->table_name.c_str(), opt_info->condition.c_str(), opt_info->fields.c_str());
		dynamic_string_array data;
		dynamic_string k1("name");
		dynamic_string v1("xty");
		dynamic_string k2("level");
		dynamic_string v2("28");
		data.push_back(k1);
		data.push_back(v1);
		data.push_back(k2);
		data.push_back(v2);
		rpc->call_remote_func("on_opt_db_with_result", opt_info->opt_id, true, data);
	}
	else {
		if (opt_info->opt_type == DB_OPT_UPDATE) {
			//m_db->update(opt_info->table_name.c_str(), opt_info->condition.c_str(), opt_info->fields.c_str());
		}
		else if (opt_info->opt_type == DB_OPT_INSERT) {
			//m_db->insert(opt_info->table_name.c_str(), opt_info->fields.c_str());
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
	log_info("add executor, opt type = %u, opt id = %"I64_FMT"u, table name = %s", opt_type, opt_id, table_name.data());
	db_opt_info* opt_info = m_mem_pool.allocate();
	opt_info->opt_type = opt_type;
	opt_info->opt_id = opt_id;
	opt_info->socket_index = socket_index;
	opt_info->table_name = table_name.data();
	opt_info->condition = query.data();
	opt_info->fields = fields.data();
	parse_fields(opt_info);
	auto itr = m_executors.find(opt_info->table_name);
	if (itr == m_executors.end()) {
		m_executors[opt_info->table_name] = std::unordered_map<std::string, db_opt_info*>();
		itr = m_executors.find(opt_info->table_name);
	}
	std::unordered_map<std::string, db_opt_info*>& executors = itr->second;
	auto opt_itr = executors.find(opt_info->condition);
	if (opt_itr == executors.end()) {
		executors[opt_info->condition] = opt_info;
	}
	else {
		if (merge_executor(opt_info, opt_itr->second)) {
			executors.erase(opt_itr);
		}
	}
}

void executor_manager::parse_fields(db_opt_info* opt_info)
{
	if (opt_info->fields.empty()) {
		return;
	}
	char* s = const_cast<char*>(opt_info->fields.data());
	char* split_fields = strtok(s, ",");
	while (NULL != split_fields) {
		parse_key_and_value(opt_info, split_fields);
		split_fields = strtok(NULL, ",");
	}
}

void executor_manager::parse_key_and_value(db_opt_info* opt_info, char* split_fields)
{
	std::string k = strtok(split_fields, "=");
	std::string v = strtok(NULL, "=");
	if (k.empty() || v.empty()) {
		return;
	}
	opt_info->key_2_value[k] = v;
}

bool executor_manager::merge_executor(db_opt_info* src_opt_info, db_opt_info*& dest_opt_info)
{
	if (src_opt_info->opt_type == 1 || src_opt_info->opt_type == 3) {
		executor(dest_opt_info);
		executor(src_opt_info);
		return true;
	}

	if (src_opt_info->opt_type == 2) {
		std::map<std::string, std::string>& src_key_2_value = src_opt_info->key_2_value;
		std::map<std::string, std::string>& dest_key_2_value = dest_opt_info->key_2_value;
		for (auto src_itr = src_key_2_value.begin(); src_itr != src_key_2_value.end(); ++src_itr) {
			dest_key_2_value[src_itr->first] = src_itr->second;
		}
	}

	if (src_opt_info->opt_type == 4) {
		db_opt_info* tmp_opt_info = dest_opt_info;
		dest_opt_info = src_opt_info;
		m_mem_pool.deallocate(tmp_opt_info);
		return false;
	}

	return false;
}

void executor_manager::clean_up()
{
	m_executors.clear();
}
