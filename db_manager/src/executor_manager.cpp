
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
		char db_result[65000];
		memset(db_result, 0, 65000);
		int len = 0;
		bool status = m_db->query(opt_info->table_name.c_str(), opt_info->condition.c_str(), opt_info->fields.c_str(), db_result, len);
		binary_data result(db_result, len);
		//char buffer[60000];
		//int buffer_index = 0;
		//fill_packet(buffer, buffer_index, (uint16)3);
		//dynamic_array<soldier_info> soldiers;
		//soldier_info soldier_1;
		//soldier_1.soldier_type = 1;
		//soldier_1.soldier_num = 100;
		//soldiers.push_back(soldier_1);
		//soldier_info soldier_2;
		//soldier_2.soldier_type = 2;
		//soldier_2.soldier_num = 200;
		//soldiers.push_back(soldier_2);
		//soldier_info soldier_3;
		//soldier_3.soldier_type = 3;
		//soldier_3.soldier_num = 300;
		//soldiers.push_back(soldier_3);
		//dynamic_string* bstr = allocate_binary_string(sizeof(soldier_info)* soldiers.size() + sizeof(uint16));
		//dynamic_struct_2_bstr(bstr->data(), soldiers);
		//fill_packet(buffer, buffer_index, (TRoleID_t)0xF1F2F3F4F5F6F7F8, (TNpcIndex_t)0xA1A2, 
		//	(TPosValue_t)0x1B2B,(TPosValue_t)0x1C2C, (TPosValue_t)0x1D2D,(TPosValue_t)0x1E2E, *bstr, (TGameTime_t)123456);
		//fill_packet(buffer, buffer_index, (TRoleID_t)0xE1E2E3E4E5E6E7E8, (TNpcIndex_t)0xB2A3,
		//	(TPosValue_t)0x2B3B, (TPosValue_t)0x2C3C, (TPosValue_t)0x2D3D, (TPosValue_t)0x2E3E, *bstr, (TGameTime_t)234567);
		//fill_packet(buffer, buffer_index, (TRoleID_t)0xD1D2D3D4D5D6D7D8, (TNpcIndex_t)0xC2C3,
		//	(TPosValue_t)0x2C3C, (TPosValue_t)0x2D3D, (TPosValue_t)0x2E3E, (TPosValue_t)0x2F3F, *bstr, (TGameTime_t)345678);
		//dynamic_string result(buffer, buffer_index);
		rpc->call_remote_func("on_opt_db_with_result", opt_info->opt_id, status, result);
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
	// strtok not thread safe, should refactor it
	char* split_fields = strtok(s, ",");
	while (NULL != split_fields) {
		parse_key_and_value(opt_info, split_fields);
		split_fields = strtok(NULL, ",");
	}
}

void executor_manager::parse_key_and_value(db_opt_info* opt_info, char* split_fields)
{
	std::string k = strtok(split_fields, "=");
	std::string v;
	if (opt_info->opt_type == DB_OPT_QUERY) {
		v = "1";
	}
	else {
		v = strtok(NULL, "=");	
	}
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
