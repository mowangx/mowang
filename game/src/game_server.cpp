
#include "game_server.h"
#include "log.h"
#include "socket.h"

#include "tbl_test.h"
#include "time_manager.h"

#include "game_enum.h"
#include "socket_manager.h"
#include "gate_handler.h"
#include "game_manager_handler.h"
#include "db_manager_handler.h"

#include "rpc_proxy.h"
#include "rpc_client.h"

game_server::game_server()
{
	m_write_packets.clear();
	m_server_id = INVALID_SERVER_ID;
	m_process_id = INVALID_PROCESS_ID;
	memset(m_listen_ip.data(), 0, IP_SIZE);
	m_listen_port = 0;
}

game_server::~game_server()
{
	
}

bool game_server::init(TProcessID_t process_id)
{
	if (!DTblTestMgr.load("../config/server_test.xml")) {
		log_error("load config failed");
		return false;
	}
	log_info("load config success");

	m_process_id = process_id;

	m_server_id = 100;
	
	char* ip = "127.0.0.1";
	memcpy(m_listen_ip.data(), ip, strlen(ip));

	m_listen_port = 10200 + m_process_id;

	return true;
}

void game_server::run()
{
	DRegisterRpc(this, game_server, game_rpc_func_1, 3);
	DRegisterRpc(this, game_server, game_rpc_func_2, 2);
	DRegisterRpc(this, game_server, on_query_servers, 3);
	DRegisterRpc(this, game_server, login_server, 3)

	DRegisterStubRpc(this, game_server, game_rpc_func_1, 3);
	DRegisterStubRpc(this, game_server, game_rpc_func_2, 2);

	gate_handler::Setup();
	game_manager_handler::Setup();
	db_manager_handler::Setup();
	TAppTime_t before_loop_time(0), after_loop_time(0);

	while (true) {
		before_loop_time = DTimeMgr.update();

		// 
		std::vector<TPacketRecvInfo_t*> read_packets;
		std::vector<socket_base*> wait_init_sockets;
		std::vector<socket_base*> wait_del_sockets;

		DNetMgr.read_packets(read_packets, wait_init_sockets, wait_del_sockets);

		for (auto socket : wait_del_sockets) {
			socket->get_packet_handler()->handle_close();
		}

		for (auto socket : wait_init_sockets) {
			socket->get_packet_handler()->handle_init();
		}

		for (auto packet_info : read_packets) {
			packet_info->socket->get_packet_handler()->handle(packet_info->packet);
		}

		DNetMgr.finish_read_packets(read_packets, wait_del_sockets);
		read_packets.clear();

		std::vector<TPacketSendInfo_t*> send_packets;
		DNetMgr.finish_write_packets(send_packets);
		for (auto packet_info : send_packets) {
			m_mem_pool.deallocate((char*)packet_info->packet);
			m_packet_pool.deallocate(packet_info);
		}
		send_packets.clear();

		DNetMgr.write_packets(m_write_packets);
		m_write_packets.clear();

		after_loop_time = DTimeMgr.update();
		if ((after_loop_time - before_loop_time) < PER_FRAME_TIME) {
			std::this_thread::sleep_for(std::chrono::milliseconds(PER_FRAME_TIME + before_loop_time - after_loop_time));
		}
	}
}

void game_server::get_process_info(game_process_info & process_info) const
{
	process_info.server_id = m_server_id;
	process_info.process_type = PROCESS_GAME;
	process_info.process_id = m_process_id;
}

void game_server::get_server_info(game_server_info& server_info) const
{
	get_process_info(server_info.process_info);
	server_info.port = m_listen_port;
	memcpy(server_info.ip.data(), m_listen_ip.data(), IP_SIZE);
}

TPacketSendInfo_t* game_server::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char* game_server::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void game_server::push_write_packets(TPacketSendInfo_t* packet_info)
{
	m_write_packets.push_back(packet_info);
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

void game_server::game_rpc_func_1(const dynamic_string& p1, uint16 p2, const std::array<char, 127>& p3)
{
	log_info("game rpc func 1, p1 = %s, p2 = %d, p3 = %s", p1.data(), p2, p3.data());
}

void game_server::game_rpc_func_2(uint8 p1, const std::array<char, 33>& p2)
{
	log_info("game rpc func 2, p1 = %d, p2 = %s", p1, p2.data());
}

void game_server::on_query_servers(TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers)
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

void game_server::login_server(TSocketIndex_t client_id, TPlatformID_t platform_id, TUserID_t user_id)
{
	// send msg to db manager to query role id from db by platform id and user id
	static TRoleID_t role_id = 1;
	m_client_id_2_role_id[client_id] = role_id;
	++role_id;
	log_info("login server, client id = '%"I64_FMT"u', platform id = %u, user id = %s", client_id, platform_id, user_id.data());
}

TRoleID_t game_server::get_role_id_by_client_id(TSocketIndex_t client_id) const
{
	auto itr = m_client_id_2_role_id.find(client_id);
	if (itr != m_client_id_2_role_id.end()) {
		return itr->second;
	}
	return INVALID_ROLE_ID;
}
