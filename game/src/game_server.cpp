
#include "game_server.h"
#include "tbl_test.h"
#include "gate_handler.h"
#include "game_manager_handler.h"
#include "db_manager_handler.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "socket_manager.h"
#include "roll_stub.h"

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

	m_server_info.process_info.server_id = 100;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10200 + process_id;

	gate_handler::Setup();
	game_manager_handler::Setup();
	db_manager_handler::Setup();

	DRegisterStubRpc(this, game_server, game_rpc_func, 2);
	DRegisterStubRpc(this, game_server, game_rpc_func_1, 4);
	DRegisterStubRpc(this, game_server, game_rpc_func_2, 3);

	DRegisterServerRpc(this, game_server, register_server, 2);
	DRegisterServerRpc(this, game_server, on_register_servers, 4);
	DRegisterServerRpc(this, game_server, login_server, 5);
	DRegisterServerRpc(this, game_server, create_entity, 2);
	DRegisterServerRpc(this, game_server, on_register_entity, 3);

	connect_game_manager_loop("127.0.0.1", 10000);

	return true;
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

void game_server::login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, TPlatformID_t platform_id, TUserID_t user_id, TSocketIndex_t test_client_id)
{
	// send msg to db manager to query role id from db by platform id and user id
	TProcessID_t gate_id = (TProcessID_t)(client_id >> 48);
	static TRoleID_t role_id = 1;
	++role_id;
	role* p = new role();
	p->set_server_id(100);
	p->set_game_id(m_server_info.process_info.process_id);
	p->set_client_id(client_id);
	p->set_gate_id(gate_id);
	p->set_role_id(role_id);
	p->init();
	m_client_id_2_role_id[client_id] = role_id;
	m_roles.push_back(p);

	DRpcWrapper.call_stub("roll_stub", "register_role", role_id, p->get_mailbox_info());

	dynamic_string p1("xiedi");
	std::array<char, 127> p3;
	memset(p3.data(), 0, 127);
	memcpy(p3.data(), "hello world", 11);
	DRpcWrapper.call_client(p->get_proxy_info(), "robot_rpc_func_1", gate_id, client_id, p1, role_id, p3, test_client_id);

	log_info("login server, client id = %"I64_FMT"u, gate id = %u, role id = %"I64_FMT"u, platform id = %u, user id = %s", client_id, gate_id, role_id, platform_id, user_id.data());
}

void game_server::game_rpc_func(TSocketIndex_t socket_index, TServerID_t server_id)
{
	log_info("game rpc func, server id = %u", server_id);
}

void game_server::game_rpc_func_1(TSocketIndex_t socket_index, const dynamic_string& p1, uint16 p2, const std::array<char, 127>& p3)
{
	log_info("game rpc func 1, p1 = %s, p2 = %d, p3 = %s", p1.data(), p2, p3.data());
}

void game_server::game_rpc_func_2(TSocketIndex_t socket_index, uint8 p1, const std::array<char, 33>& p2)
{
	log_info("game rpc func 2, p1 = %d, p2 = %s", p1, p2.data());
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
		create_entity_globally("space_stub");
	}
}

void game_server::create_entity(TSocketIndex_t socket_index, const dynamic_string & stub_name)
{
	create_entity_locally(stub_name);
}

void game_server::transfer_client(TSocketIndex_t client_id, packet_base* packet)
{
	TPacketID_t packet_id = packet->get_packet_id();
	TProcessID_t gate_id = (TProcessID_t)(client_id >> 48);
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

void game_server::create_entity_globally(const dynamic_string& stub_name)
{
	TServerID_t server_id = 100;
	rpc_client* rpc = DRpcWrapper.get_random_client(server_id, PROCESS_GAME_MANAGER);
	if (NULL == rpc) {
		return;
	}
	rpc->call_remote_func("create_entity", server_id, stub_name);
}

void game_server::create_entity_locally(const dynamic_string& stub_name)
{
	entity* e = NULL;
	if (strcmp(stub_name.data(), "roll_stub") == 0) {
		e = new roll_stub();
	}
	if (NULL != e) {
		e->init();
	}
	TServerID_t server_id = 100;
	rpc_client* rpc = DRpcWrapper.get_random_client(server_id, PROCESS_GAME_MANAGER);
	if (NULL == rpc) {
		return;
	}
	rpc->call_remote_func("register_entity", stub_name, m_server_info.process_info);
}

TRoleID_t game_server::get_role_id_by_client_id(TSocketIndex_t client_id) const
{
	auto itr = m_client_id_2_role_id.find(client_id);
	if (itr != m_client_id_2_role_id.end()) {
		return itr->second;
	}
	return INVALID_ROLE_ID;
}
