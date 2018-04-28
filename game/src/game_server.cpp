
#include "game_server.h"

#include "log.h"
#include "tbl_test.h"
#include "game_enum.h"
#include "gate_handler.h"
#include "game_manager_handler.h"
#include "db_manager_handler.h"
#include "socket_manager.h"

#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

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

	if (!DTblTestMgr.load("../config/server_test.xml")) {
		log_error("load config failed");
		return false;
	}
	log_info("load config success");

	m_server_info.process_info.server_id = 100;
	char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = 10200 + process_id;

	gate_handler::Setup();
	game_manager_handler::Setup();
	db_manager_handler::Setup();

	DRegisterServerRpc(this, game_server, game_rpc_func, 2);
	DRegisterStubRpc(this, game_server, game_rpc_func, 2);

	DRegisterServerRpc(this, game_server, register_server, 2);
	DRegisterServerRpc(this, game_server, on_query_servers, 4);
	DRegisterServerRpc(this, game_server, login_server, 5);
	DRegisterServerRpc(this, game_server, game_rpc_func_1, 4);
	DRegisterServerRpc(this, game_server, game_rpc_func_2, 3);

	DRegisterStubRpc(this, game_server, game_rpc_func_1, 4);
	DRegisterStubRpc(this, game_server, game_rpc_func_2, 3);

	return true;
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

void game_server::register_server(TSocketIndex_t socket_index, const game_server_info& server_info)
{
	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		DRpcWrapper.register_handler_info(itr->second, server_info);
	}

	dynamic_string p1("xiedi");
	uint16 p2 = 65500;
	std::array<char, 127> p3;
	memset(p3.data(), 0, 127);
	memcpy(p3.data(), "hello world", 11);

	uint8 p2_1 = 99;
	std::array<char, 33> p2_2;
	memset(p2_2.data(), 0, 33);
	memcpy(p2_2.data(), "mowang", 6);

	game_process_info process_info;
	process_info.server_id = 100;
	process_info.process_type = PROCESS_GAME;
	process_info.process_id = 1;

	DRpcWrapper.call_stub(process_info, "game_server", "game_rpc_func_1", p1, p2, p3);

	DRpcWrapper.call_stub(process_info, "game_server", "game_rpc_func_2", p2_1, p2_2);
}

void game_server::login_server(TSocketIndex_t socket_index, TSocketIndex_t client_id, TProcessID_t gate_id, TPlatformID_t platform_id, TUserID_t user_id)
{
	// send msg to db manager to query role id from db by platform id and user id
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

	dynamic_string p1("xiedi");
	TServerID_t server_id = 100;
	std::array<char, 127> p3;
	memset(p3.data(), 0, 127);
	memcpy(p3.data(), "hello world", 11);
	DRpcWrapper.call_client(p->get_proxy_info(), "robot_rpc_func_1", client_id, p1, server_id, p3);

	log_info("login server, client id = '%"I64_FMT"u', platform id = %u, user id = %s", client_id, platform_id, user_id.data());
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

void game_server::on_query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers)
{
	log_info("on_query_servers, server id = %d, process type = %d, server size = %u", server_id, process_type, servers.size());
	for (int i = 0; i < servers.size(); ++i) {
		const game_server_info& server_info = servers[i];
		if (DNetMgr.start_connect<db_manager_handler>(server_info.ip.data(), server_info.port)) {
			log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
		else {
			log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
	}
}

void game_server::transfer_client(TSocketIndex_t client_id, packet_base * packet)
{
	TPacketID_t packet_id = packet->get_packet_id();
	if (packet_id == PACKET_ID_TRANSFER_SERVER_BY_NAME) {
		transfer_server_by_name_packet* rpc_info = (transfer_server_by_name_packet*)packet;
		DRpcRole.call(get_role_id_by_client_id(client_id), rpc_info->m_rpc_name, rpc_info->m_buffer);
	}
	else if (packet_id == PACKET_ID_TRANSFER_SERVER_BY_INDEX) {
		transfer_server_by_index_packet* rpc_info = (transfer_server_by_index_packet*)packet;
		DRpcRole.call(get_role_id_by_client_id(client_id), rpc_info->m_rpc_index, rpc_info->m_buffer);
	}
}

void game_server::create_entity(uint8 e_type)
{
	entity* e = NULL;
	if (e_type == ENTITY_ROLL_STUB) {
	}
	if (NULL != e) {
		e->init();
	}
}

TRoleID_t game_server::get_role_id_by_client_id(TSocketIndex_t client_id) const
{
	auto itr = m_client_id_2_role_id.find(client_id);
	if (itr != m_client_id_2_role_id.end()) {
		return itr->second;
	}
	return INVALID_ROLE_ID;
}
