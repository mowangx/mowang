
#include "game_server.h"
#include "tbl_test.h"
#include "game_packet_handler.h"
#include "http_client_handler.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "tcp_manager.h"
#include "roll_stub.h"
#include "timer.h"
#include "entity_manager.h"
#include "etcd_manager.h"
#include "mailbox_manager.h"

#include "sequence.h"

game_server::game_server() :service(PROCESS_GAME)
{
	m_entity_id = INVALID_ENTITY_ID;
	for (int i = 0; i < MAX_PROCESS_TYPE_NUM; ++i) {
		m_process_num[i] = 0;
	}
}

game_server::~game_server()
{
	
}

bool game_server::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	//if (!DTblTestMgr.load("../config/server_test.xml")) {
	//	log_error("load config failed");
	//	return false;
	//}
	//log_info("load config success");

	m_entity_id = ((TEntityID_t)m_server_info.process_info.server_id << 48) + ((TEntityID_t)m_server_info.process_info.process_id << 40);

	game_packet_handler::Setup();
	http_client_handler::Setup();

	DRegisterServerRpc(this, game_server, register_server, 2);
	DRegisterServerRpc(this, game_server, login_server, 3);
	DRegisterServerRpc(this, game_server, logout_server, 2);
	DRegisterServerRpc(this, game_server, create_entity, 2);
	DRegisterServerRpc(this, game_server, on_opt_db_with_status, 3);
	DRegisterServerRpc(this, game_server, on_opt_db_with_result, 4);
	DRegisterServerRpc(this, game_server, destroy_entity, 1);
	DRegisterServerRpc(this, game_server, on_register_entities, 5);
	DRegisterServerRpc(this, game_server, on_unregister_process, 4);

	DRegisterStubRpc(this, game_server, remove_entity, 1);

	if (!DNetMgr.start_listen<game_packet_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	log_info("init socket manager success");

	DEntityMgr.init();
	log_info("init entity manager success");

	return true;
}

bool game_server::connect_server(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<game_packet_handler>(ip, port);
}

void game_server::db_remove(const char* table, const char* query, const std::function<void(bool)>& callback)
{
	db_opt_with_status(DB_OPT_DELETE, table, query, NULL, callback);
}

void game_server::db_insert(const char* table, const char* fields, const std::function<void(bool)>& callback)
{
	db_opt_with_status(DB_OPT_INSERT, table, NULL, fields, callback);
}

void game_server::db_update(const char* table, const char* query, const char* fields, const std::function<void(bool)>& callback)
{
	db_opt_with_status(DB_OPT_UPDATE, table, query, fields, callback);
}

void game_server::db_query(const char* table, const char* query, const char* fields, const std::function<void(bool, const binary_data&)>& callback)
{
	db_opt_with_result(DB_OPT_QUERY, table, query, fields, callback);
}

void game_server::db_opt_with_status(uint8 opt_type, const char* table, const char* query, const char* fields, const std::function<void(bool)>& callback)
{
	db_opt(opt_type, table, query, fields);
	m_db_status_callbacks[m_opt_id] = callback;
}

void game_server::db_opt_with_result(uint8 opt_type, const char * table, const char * query, const char * fields, const std::function<void(bool, const binary_data&)>& callback)
{
	db_opt(opt_type, table, query, fields);
	m_db_result_callbacks[m_opt_id] = callback;
}

void game_server::db_opt(uint8 opt_type, const char * table, const char * query, const char * fields)
{
	m_opt_id += 1;
	rpc_client* rpc = DRpcWrapper.get_client_by_process_type(PROCESS_DB);
	if (NULL != rpc) {
		dynamic_string tmp_table(table);
		dynamic_string tmp_query(query);
		dynamic_string tmp_fields(fields);
		rpc->call_remote_func("add_executor", opt_type, m_opt_id, tmp_table, tmp_query, tmp_fields);
	}
}

void game_server::login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, const account_info& account_data)
{
	// send msg to db manager to query role id from db by platform id and user id
	TProcessID_t gate_id = (TProcessID_t)((client_id >> 40) & 0xFFFF);
	account* p = (account*)DEntityMgr.create_entity("account", gate_id, client_id);
	p->login(account_data);
	log_info("login server, client id %" I64_FMT "u, gate id %u, entity id %" I64_FMT "u, platform id %u, user id %s", 
		client_id, gate_id, p->get_entity_id(), account_data.platform_id, account_data.user_id.data());
}

void game_server::logout_server(TSocketIndex_t socket_index, TSocketIndex_t client_id)
{
	log_info("logout server, client id %" I64_FMT "u", client_id);
	remove_entity_core(client_id);
}

void game_server::add_process(const game_server_info& server_info)
{
	TBaseType_t::add_process(server_info);
	if (server_info.process_info.process_type == PROCESS_DB) {
		if (DNetMgr.start_connect<game_packet_handler>(server_info.ip.data(), server_info.port)) {
			log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
		else {
			log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
	}
}

void game_server::register_server(TSocketIndex_t socket_index, const game_server_info & server_info)
{
	TBaseType_t::register_server(socket_index, server_info);
	if (server_info.process_info.process_type == PROCESS_DB) {
		m_process_num[PROCESS_DB] += 1;
		if (m_process_num[PROCESS_DB] >= m_config.get_desire_process_num(PROCESS_DB)) {
			on_game_start();
		}
	}
}

void game_server::create_entity(TSocketIndex_t socket_index, const TEntityName_t& entity_name)
{
	//create_entity_locally(entity_name.data());
}

void game_server::remove_entity(TSocketIndex_t client_id)
{
	if (!remove_entity_core(client_id)) {
		log_error("remove entity failed for not find client id, client id %" I64_FMT "u", client_id);
	}
}

void game_server::on_opt_db_with_status(TSocketIndex_t socket_index, TOptID_t opt_id, bool status)
{
	auto itr = m_db_status_callbacks.find(opt_id);
	if (itr == m_db_status_callbacks.end()) {
		return;
	}
	const std::function<void(bool)>& callback = itr->second;
	callback(status);
}

void game_server::on_opt_db_with_result(TSocketIndex_t socket_index, TOptID_t opt_id, bool status, const binary_data& result)
{
	auto itr = m_db_result_callbacks.find(opt_id);
	if (itr == m_db_result_callbacks.end()) {
		return;
	}
	const std::function<void(bool, const binary_data&)>& callback = itr->second;
	callback(status, result);
}

void game_server::on_connect(TSocketIndex_t socket_index)
{
	TBaseType_t::on_connect(socket_index);
	//game_process_info process_info;
	//if (process_info.process_type == PROCESS_DB) {
	//	DSequence.save();
	//}
	//else if (process_info.process_type == PROCESS_HTTP_CLIENT) {
	//	
	//	//http_request("localhost", "/sentry/process_trace", "user_name=mowang", false);
	//}
}

void game_server::on_disconnect(TSocketIndex_t socket_index)
{
	
}

bool game_server::remove_entity_core(TSocketIndex_t client_id)
{
	auto itr = m_client_id_2_role.find(client_id);
	if (itr != m_client_id_2_role.end()) {
		log_info("remove entity success! client id %" I64_FMT "u", client_id);
		role* p = itr->second;
		m_client_id_2_role.erase(itr);

		DTimer.add_timer(10, false, p, [](void* param, TTimerID_t timer_id) {
			role* r = (role*)param;
			DEntityMgr.destroy_entity(r->get_entity_id());
		});

		return true;
	}
	return false;
}

void game_server::on_game_start()
{
	log_info("on_game_start");
	//http_request("www.boost.org", "/LICENSE_1_0.txt", "user_name=mowang", true, [&](int status, const dynamic_string& result) {
	//	log_info("status %d, result %s", status, result.data());
	//});
	//http_request("km.netease.com", "/article/278519", "", false, [&](int status, const dynamic_string& result) {
	//	log_info("status %d, result %s", status, result.data());
	//});
	if (m_server_info.process_info.process_id == 1) {
		create_entity_locally("roll_stub_tag", "roll_stub");
	}
}

void game_server::create_entity_globally(const std::string & entity_name, bool check_repeat)
{
	//rpc_client* rpc = DRpcWrapper.get_random_client(get_server_id(), PROCESS_GAME_MANAGER);
	//if (NULL == rpc) {
	//	return;
	//}
	//TEntityName_t name;
	//memset(name.data(), 0, ENTITY_NAME_LEN);
	//memcpy((void*)name.data(), entity_name.c_str(), entity_name.length());
	//rpc->call_remote_func("create_entity", get_server_id(), name, check_repeat, get_game_id());
}

entity* game_server::create_entity_locally(const std::string& tag, const std::string& entity_name)
{
	entity* e = DEntityMgr.create_entity(entity_name);
	std::string server_id = gx_to_string("%d", m_server_info.process_info.server_id);
	TNamespace_t name;
	memset(name.data(), 0, NAMESPACE_LEN);
	memcpy(name.data(), server_id.c_str(), server_id.length());
	TTagName_t global_tag;
	memset(global_tag.data(), 0, TAG_NAME_LEN);
	memcpy(global_tag.data(), tag.c_str(), tag.length());
	TEntityName_t global_entity_name;
	memset(global_entity_name.data(), 0, ENTITY_NAME_LEN);
	memcpy(global_entity_name.data(), entity_name.c_str(), entity_name.length());
	DEtcdMgr.register_entity(name, global_tag, e->get_entity_id(), global_entity_name);
	return e;
}

void game_server::destroy_entity(TEntityID_t entity_id)
{
	DEntityMgr.destroy_entity(entity_id);
}

role* game_server::get_role_by_client_id(TSocketIndex_t client_id) const
{
	auto itr = m_client_id_2_role.find(client_id);
	return itr != m_client_id_2_role.end() ? itr->second : NULL;
}

void game_server::update_role_proxy_info(const proxy_info& old_proxy_info, const proxy_info& new_proxy_info)
{
	auto itr = m_client_id_2_role.find(old_proxy_info.client_id);
	if (itr != m_client_id_2_role.end()) {
		role* p = itr->second;
		m_client_id_2_role.erase(itr);
		m_client_id_2_role[new_proxy_info.client_id] = p;
	}
	else {
		log_error("update role process info failed for not find client id! old client id = %" I64_FMT "u, new client id = %" I64_FMT "u",
			old_proxy_info.client_id, new_proxy_info.client_id);
	}
}

TEntityID_t game_server::get_entity_id_by_client_id(TSocketIndex_t client_id) const
{
	auto itr = m_client_id_2_role.find(client_id);
	if (itr != m_client_id_2_role.end()) {
		return (itr->second)->get_entity_id();
	}
	return INVALID_ENTITY_ID;
}
