
#include "game_server.h"
#include "tbl_test.h"
#include "gate_handler.h"
#include "game_manager_handler.h"
#include "db_manager_handler.h"
#include "http_client_handler.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "tcp_manager.h"
#include "lbs_stub.h"
#include "roll_stub.h"
#include "fight_stub.h"
#include "timer.h"
#include "entity_manager.h"

#include "sequence.h"

game_server::game_server() :service(PROCESS_GAME)
{
	m_entity_id = INVALID_ENTITY_ID;
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
	m_opt_id = ((TOptID_t)m_server_info.process_info.server_id << 48) + ((TOptID_t)m_server_info.process_info.process_id << 40);

	gate_handler::Setup();
	game_manager_handler::Setup();
	db_manager_handler::Setup();
	http_client_handler::Setup();

	DRegisterServerRpc(this, game_server, register_server, 2);
	DRegisterServerRpc(this, game_server, on_register_servers, 4);
	DRegisterServerRpc(this, game_server, login_server, 4);
	DRegisterServerRpc(this, game_server, logout_server, 2);
	DRegisterServerRpc(this, game_server, create_entity, 2);
	DRegisterServerRpc(this, game_server, on_register_entity, 2);
	DRegisterServerRpc(this, game_server, on_opt_db_with_status, 3);
	DRegisterServerRpc(this, game_server, on_opt_db_with_result, 4);
	DRegisterServerRpc(this, game_server, on_http_response, 4);
	DRegisterServerRpc(this, game_server, destroy_entity, 1);

	DRegisterStubRpc(this, game_server, remove_entity, 1);

	if (!DNetMgr.start_listen<gate_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	log_info("init socket manager success");

	DEntityMgr.init();
	log_info("init entity manager success");

	return true;
}

void game_server::work_run()
{
	connect_game_manager_loop(m_config.get_game_manager_listen_ip(), m_config.get_game_manager_listen_port());
	TBaseType_t::work_run();
}

bool game_server::connect_game_manager(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<game_manager_handler>(ip, port);
}

resource* game_server::allocate_resource()
{
	return m_resource_pool.allocate();
}

void game_server::deallocate_resource(resource* res)
{
	m_resource_pool.deallocate(res);
}

city* game_server::allocate_city()
{
	return m_city_pool.allocate();
}

void game_server::deallocate_city(city* c)
{
	m_city_pool.deallocate(c);
}

npc* game_server::allocate_npc()
{
	return m_npc_pool.allocate();
}

void game_server::deallocate_npc(npc* p)
{
	m_npc_pool.deallocate(p);
}

farmland * game_server::allocate_farmland()
{
	return m_farmland_pool.allocate();
}

void game_server::deallocate_farmland(farmland * f)
{
	m_farmland_pool.deallocate(f);
}

soldier_training_info * game_server::allocate_soldier_training()
{
	return nullptr;
}

void game_server::deallocate_soldier_training(soldier_training_info * s)
{
}

resource_up_info * game_server::allocate_resource_up_info()
{
	return nullptr;
}

void game_server::deallocate_resource_up_info(resource_up_info * r)
{
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
	rpc_client* rpc = DRpcWrapper.get_random_client(m_server_info.process_info.server_id, PROCESS_DB);
	if (NULL != rpc) {
		dynamic_string tmp_table(table);
		dynamic_string tmp_query(query);
		dynamic_string tmp_fields(fields);
		rpc->call_remote_func("add_executor", opt_type, m_opt_id, tmp_table, tmp_query, tmp_fields);
	}
}

void game_server::http_request(const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl, const std::function<void(int, const dynamic_string&)>& callback)
{
	m_opt_id += 1;
	rpc_client* rpc = DRpcWrapper.get_random_client(m_server_info.process_info.server_id, PROCESS_HTTP_CLIENT);
	if (NULL != rpc) {
		rpc->call_remote_func("http_request", m_opt_id, host, url, params, usessl);
	}
	m_http_response_callbacks[m_opt_id] = callback;
}

void game_server::login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, TPlatformID_t platform_id, const account_info& account)
{
	// send msg to db manager to query role id from db by platform id and user id
	TProcessID_t gate_id = (TProcessID_t)((client_id >> 40) & 0xFFFF);
	role* p = (role*)create_entity_locally("role");
	p->set_client_id(client_id);
	p->set_gate_id(gate_id);
	p->set_role_name(account.role_name);
	p->set_sex(account.sex);
	// @TODO just test, should load from db
	p->set_test_client_id(account.test_client_id);
	m_client_id_2_role[client_id] = p;
	p->login(platform_id, account.token);

	log_info("login server, client id %" I64_FMT "u, gate id %u, entity id %" I64_FMT "u, platform id %u, token %s", 
		client_id, gate_id, p->get_entity_id(), platform_id, account.token.data());
}

void game_server::logout_server(TSocketIndex_t socket_index, TSocketIndex_t client_id)
{
	log_info("logout server, client id %" I64_FMT "u", client_id);
	remove_entity_core(client_id);
}

void game_server::register_server(TSocketIndex_t socket_index, const game_server_info & server_info)
{
	TBaseType_t::register_server(socket_index, server_info);
	if (server_info.process_info.process_type == PROCESS_DB) {
		create_entity_globally("roll_stub");
		create_entity_globally("lbs_stub");
		create_entity_globally("fight_stub");
	}
}

void game_server::on_register_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers)
{
	log_info("on_register_servers, server id = %d, process type = %d, server size = %u", server_id, process_type, servers.size());
	for (int i = 0; i < servers.size(); ++i) {
		const game_server_info& server_info = servers[i];

		game_server_info tmp_server_info;
		if (DRpcWrapper.get_server_info(server_info.process_info, tmp_server_info)) {
			log_info("server has registerted, ip = %s, port = %d", server_info.ip.data(), server_info.port);
			continue;
		}

		if (server_info.process_info.process_type == PROCESS_DB) {
			if (DNetMgr.start_connect<db_manager_handler>(server_info.ip.data(), server_info.port)) {
				log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
			}
			else {
				log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
			}
		}
		else if (server_info.process_info.process_type == PROCESS_HTTP_CLIENT) {
			if (DNetMgr.start_connect<http_client_handler>(server_info.ip.data(), server_info.port)) {
				log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
			}
			else {
				log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
			}
		}
	}
}

void game_server::create_entity(TSocketIndex_t socket_index, const TEntityName_t& entity_name)
{
	create_entity_locally(entity_name.data());
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

void game_server::on_http_response(TSocketIndex_t socket_index, TOptID_t opt_id, int status, const dynamic_string& result)
{
	auto itr = m_http_response_callbacks.find(opt_id);
	if (itr == m_http_response_callbacks.end()) {
		return;
	}
	const std::function<void(int, const dynamic_string&)>& callback = itr->second;
	callback(status, result);
}

void game_server::on_connect(TSocketIndex_t socket_index)
{
	TBaseType_t::on_connect(socket_index);
	game_process_info process_info;
	if (DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index)) {
		if (process_info.process_type == PROCESS_DB) {
			DSequence.save();
		}
		else if (process_info.process_type == PROCESS_HTTP_CLIENT) {
			http_request("www.boost.org", "/LICENSE_1_0.txt", "user_name=mowang", true, [&](int status, const dynamic_string& result) {
				log_info("status %d, result %s", status, result.data());
			});
			//http_request("localhost", "/sentry/process_trace", "user_name=mowang", false);
		}
	}
}

void game_server::on_disconnect(TSocketIndex_t socket_index)
{
	game_process_info process_info;
	if (!DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index) ||
		process_info.process_type != PROCESS_GAME) {
		return;
	}

	for (auto itr = m_client_id_2_role.begin(); itr != m_client_id_2_role.end(); ++itr) {
		role* p = itr->second;
		p->logout();
	}
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

void game_server::transfer_client(TSocketIndex_t client_id, packet_base* packet)
{
	TPacketID_t packet_id = packet->get_packet_id();
	TProcessID_t gate_id = (TProcessID_t)((client_id >> 40) & 0xFFFF);
	log_info("transfer client, gate id %u, client id %" I64_FMT "u, packet id %u", gate_id, client_id, packet_id);
	if (packet_id == PACKET_ID_TRANSFER_SERVER_BY_NAME) {
		transfer_server_by_name_packet* rpc_info = (transfer_server_by_name_packet*)packet;
		DRpcEntity.call(get_role_id_by_client_id(client_id), rpc_info->m_rpc_name, rpc_info->m_buffer);
	}
	else if (packet_id == PACKET_ID_TRANSFER_SERVER_BY_INDEX) {
		transfer_server_by_index_packet* rpc_info = (transfer_server_by_index_packet*)packet;
		DRpcEntity.call(get_role_id_by_client_id(client_id), rpc_info->m_rpc_index, rpc_info->m_buffer);
	}
	else {
		log_error("transfer client failed, not find packet id, gate id %u, packet id %u", gate_id, packet_id);
	}
}

void game_server::create_entity_globally(const std::string& entity_name)
{
	rpc_client* rpc = DRpcWrapper.get_random_client(get_server_id(), PROCESS_GAME_MANAGER);
	if (NULL == rpc) {
		return;
	}
	TEntityName_t name;
	memset(name.data(), 0, ENTITY_NAME_LEN);
	memcpy((void*)name.data(), entity_name.c_str(), entity_name.length());
	rpc->call_remote_func("create_entity", get_server_id(), name);
}

entity* game_server::create_entity_locally(const std::string& entity_name)
{
	entity* e = DEntityMgr.create_entity(entity_name);
	rpc_client* rpc = DRpcWrapper.get_random_client(get_server_id(), PROCESS_GAME_MANAGER);
	if (NULL == rpc) {
		// @TODO need delete entity
		return nullptr;
	}
	TEntityName_t name;
	memset(name.data(), 0, ENTITY_NAME_LEN);
	memcpy((void*)name.data(), entity_name.c_str(), entity_name.length());
	rpc->call_remote_func("register_entity", name, m_server_info.process_info);
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

TRoleID_t game_server::get_role_id_by_client_id(TSocketIndex_t client_id) const
{
	auto itr = m_client_id_2_role.find(client_id);
	if (itr != m_client_id_2_role.end()) {
		return (itr->second)->get_role_id();
	}
	return INVALID_ROLE_ID;
}
