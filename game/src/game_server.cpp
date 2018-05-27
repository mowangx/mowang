
#include "game_server.h"
#include "tbl_test.h"
#include "gate_handler.h"
#include "game_manager_handler.h"
#include "db_manager_handler.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "socket_manager.h"
#include "lbs_stub.h"
#include "roll_stub.h"
#include "timer.h"

#include "sequence.h"

game_server::game_server() :service(PROCESS_GAME)
{
	m_write_packets.clear();
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


	m_db_opt_id = ((TDbOptID_t)m_server_info.process_info.server_id << 48) + ((TDbOptID_t)m_server_info.process_info.process_id << 40);

	gate_handler::Setup();
	game_manager_handler::Setup();
	db_manager_handler::Setup();

	DRegisterServerRpc(this, game_server, register_server, 2);
	DRegisterServerRpc(this, game_server, on_register_servers, 4);
	DRegisterServerRpc(this, game_server, login_server, 5);
	DRegisterServerRpc(this, game_server, logout_server, 2);
	DRegisterServerRpc(this, game_server, create_entity, 2);
	DRegisterServerRpc(this, game_server, on_register_entity, 2);
	DRegisterServerRpc(this, game_server, on_opt_db_with_status, 3);
	DRegisterServerRpc(this, game_server, on_opt_db_with_result, 4);

	DRegisterStubRpc(this, game_server, remove_entity, 1);

	return true;
}

void game_server::work_run()
{
	connect_game_manager_loop(m_config.get_game_manager_listen_ip(), m_config.get_game_manager_listen_port());
	TBaseType_t::work_run();
}

void game_server::net_run()
{
	if (!DNetMgr.start_listen<gate_handler>(m_server_info.port)) {
		return;
	}

	log_info("init socket manager success");

	while (true) {
		DNetMgr.update(0);
		//DNetMgr.test_kick();
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void game_server::do_loop(TGameTime_t diff)
{
	TBaseType_t::do_loop(diff);
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

void game_server::db_query(const char* table, const char* query, const char* fields, const std::function<void(bool, const dynamic_string_array2&)>& callback)
{
	db_opt_with_result(DB_OPT_QUERY, table, query, fields, callback);
}

void game_server::db_opt_with_status(uint8 opt_type, const char* table, const char* query, const char* fields, const std::function<void(bool)>& callback)
{
	db_opt(opt_type, table, query, fields);
	m_db_status_callbacks[m_db_opt_id] = callback;
}

void game_server::db_opt_with_result(uint8 opt_type, const char * table, const char * query, const char * fields, const std::function<void(bool, const dynamic_string_array2&)>& callback)
{
	db_opt(opt_type, table, query, fields);
	m_db_result_callbacks[m_db_opt_id] = callback;
}

void game_server::db_opt(uint8 opt_type, const char * table, const char * query, const char * fields)
{
	m_db_opt_id += 1;
	rpc_client* rpc = DRpcWrapper.get_random_client(m_server_info.process_info.server_id, PROCESS_DB);
	if (NULL != rpc) {
		dynamic_string tmp_table(table);
		dynamic_string tmp_query(query);
		dynamic_string tmp_fields(fields);
		rpc->call_remote_func("add_executor", opt_type, m_db_opt_id, tmp_table, tmp_query, tmp_fields);
	}
}

void game_server::login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, TPlatformID_t platform_id, TUserID_t user_id, TSocketIndex_t test_client_id)
{
	// send msg to db manager to query role id from db by platform id and user id
	TProcessID_t gate_id = (TProcessID_t)((client_id >> 40) & 0xFFFF);
	static TRoleID_t entity_id = ((TRoleID_t)m_server_info.process_info.server_id << 48) + ((TRoleID_t)m_server_info.process_info.process_id << 40);
	++entity_id;
	role* p = new role();
	p->set_server_id(100);
	p->set_game_id(m_server_info.process_info.process_id);
	p->set_client_id(client_id);
	p->set_gate_id(gate_id);
	p->set_entity_id(entity_id);
	p->set_test_client_id(test_client_id);
	p->init();
	m_client_id_2_role[client_id] = p;
	p->login(platform_id, user_id);

	log_info("login server, client id = %" I64_FMT "u, gate id = %u, entity id = %" I64_FMT "u, platform id = %u, user id = %s", 
		client_id, gate_id, entity_id, platform_id, user_id.data());
}

void game_server::logout_server(TSocketIndex_t socket_index, TSocketIndex_t client_id)
{
	log_info("logout server, client id = %"I64_FMT"u", client_id);
	remove_entity_core(client_id);
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

		if (DNetMgr.start_connect<db_manager_handler>(server_info.ip.data(), server_info.port)) {
			log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
		else {
			log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
	}
	

	if (m_server_info.process_info.process_id == 1) {
		create_entity_globally("roll_stub");
		create_entity_globally("lbs_stub");
	}
}

void game_server::create_entity(TSocketIndex_t socket_index, const TStubName_t& stub_name)
{
	create_entity_locally(stub_name.data());
}

void game_server::remove_entity(TSocketIndex_t client_id)
{
	if (!remove_entity_core(client_id)) {
		log_error("remove entity failed for not find client id, client id = %" I64_FMT "u", client_id);
	}
}

void game_server::on_opt_db_with_status(TSocketIndex_t socket_index, TDbOptID_t opt_id, bool status)
{
	auto itr = m_db_status_callbacks.find(opt_id);
	if (itr == m_db_status_callbacks.end()) {
		return;
	}
	const std::function<void(bool)>& callback = itr->second;
	callback(status);
}

void game_server::on_opt_db_with_result(TSocketIndex_t socket_index, TDbOptID_t opt_id, bool status, const dynamic_string_array2& data)
{
	auto itr = m_db_result_callbacks.find(opt_id);
	if (itr == m_db_result_callbacks.end()) {
		return;
	}
	const std::function<void(bool, const dynamic_string_array2&)>& callback = itr->second;
	callback(status, data);
}

void game_server::on_connect(TSocketIndex_t socket_index)
{
	TBaseType_t::on_connect(socket_index);
	game_process_info process_info;
	if (DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index)) {
		if (process_info.process_type == PROCESS_DB) {
			DSequence.save();
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
		log_info("remove entity success! client id = %" I64_FMT "u", client_id);
		role* p = itr->second;
		m_client_id_2_role.erase(itr);

		DTimer.add_timer(10, false, p, [](void* param, TTimerID_t timer_id) {
			role* r = (role*)param;
			delete r;
		});

		return true;
	}
	return false;
}

void game_server::transfer_client(TSocketIndex_t client_id, packet_base* packet)
{
	TPacketID_t packet_id = packet->get_packet_id();
	TProcessID_t gate_id = (TProcessID_t)((client_id >> 40) & 0xFFFF);
	log_info("transfer client, gate id = %u, client id = %"I64_FMT"u, packet id = %u", gate_id, client_id, packet_id);
	if (packet_id == PACKET_ID_TRANSFER_SERVER_BY_NAME) {
		transfer_server_by_name_packet* rpc_info = (transfer_server_by_name_packet*)packet;
		DRpcRole.call(get_role_id_by_client_id(client_id), rpc_info->m_rpc_name, rpc_info->m_buffer);
	}
	else if (packet_id == PACKET_ID_TRANSFER_SERVER_BY_INDEX) {
		transfer_server_by_index_packet* rpc_info = (transfer_server_by_index_packet*)packet;
		DRpcRole.call(get_role_id_by_client_id(client_id), rpc_info->m_rpc_index, rpc_info->m_buffer);
	}
	else {
		log_error("transfer client failed, not find packet id, gate id = %u, packet id = %u", gate_id, packet_id);
	}
}

void game_server::create_entity_globally(const std::string& stub_name)
{
	TServerID_t server_id = 100;
	rpc_client* rpc = DRpcWrapper.get_random_client(server_id, PROCESS_GAME_MANAGER);
	if (NULL == rpc) {
		return;
	}
	TStubName_t name;
	memset(name.data(), 0, STUB_NAME_LEN);
	memcpy((void*)name.data(), stub_name.c_str(), stub_name.length());
	rpc->call_remote_func("create_entity", server_id, name);
}

void game_server::create_entity_locally(const std::string& stub_name)
{
	entity* e = NULL;
	if (strcmp(stub_name.data(), "roll_stub") == 0) {
		e = new roll_stub();
	}
	else if (strcmp(stub_name.data(), "lbs_stub") == 0) {
		e = new lbs_stub();
	}
	if (NULL != e) {
		e->init();
	}
	TServerID_t server_id = 100;
	rpc_client* rpc = DRpcWrapper.get_random_client(server_id, PROCESS_GAME_MANAGER);
	if (NULL == rpc) {
		return;
	}
	TStubName_t name;
	memset(name.data(), 0, STUB_NAME_LEN);
	memcpy((void*)name.data(), stub_name.c_str(), stub_name.length());
	rpc->call_remote_func("register_entity", name, m_server_info.process_info);
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
