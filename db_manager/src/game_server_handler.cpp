
#include "game_server_handler.h"
#include "log.h"
#include "db_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

game_server_handler::game_server_handler() : packet_handler<game_server_handler>()
{
	m_rpc_client = new rpc_client(this);
}

game_server_handler::~game_server_handler()
{
	if (NULL != m_rpc_client) {
		delete m_rpc_client;
		m_rpc_client = NULL;
	}
}

void game_server_handler::Setup()
{
	TBaseType_t::Setup();
}

TPacketInfo_t* game_server_handler::create_packet_info()
{
	return DDbServer.allocate_packet_info();
}

char* game_server_handler::create_packet(int n)
{
	return DDbServer.allocate_memory(n);
}

void game_server_handler::write_packet(TPacketInfo_t* packet_info)
{
	DDbServer.push_write_packets(packet_info);
}

void game_server_handler::handle_init()
{
	dynamic_string p1("xiedi");
	uint16 p2 = 65500;
	std::array<char, 127> p3;
	memset(p3.data(), 0, 127);
	memcpy(p3.data(), "hello world", 11);
	m_rpc_client->call_remote_func("game_rpc_func_1", p1, p2, p3);

	uint8 p2_1 = 99;
	std::array<char, 33> p2_2;
	memset(p2_2.data(), 0, 33);
	memcpy(p2_2.data(), "mowang", 6);
	m_rpc_client->call_remote_func("game_rpc_func_2", p2_1, p2_2);
}

void game_server_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}