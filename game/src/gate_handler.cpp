
#include "gate_handler.h"
#include "log.h"
#include "game_server.h"
#include "rpc_proxy.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

gate_handler::gate_handler() : packet_handler<gate_handler>()
{
	m_rpc_client = new rpc_client(this);
}

gate_handler::~gate_handler()
{
	if (NULL != m_rpc_client) {
		delete m_rpc_client;
		m_rpc_client = NULL;
	}
}

void gate_handler::Setup()
{
	TBaseType_t::Setup();
	register_handler((TPacketID_t)PACKET_ID_SERVER_INFO, (packet_handler_func)&gate_handler::handle_server_info);
	register_handler((TPacketID_t)PACKET_ID_TRANSFER_CLIENT, (packet_handler_func)&gate_handler::handle_transfer_client);
}

TPacketSendInfo_t* gate_handler::create_packet_info()
{
	return DGameServer.allocate_packet_info();
}

char* gate_handler::create_packet(int n)
{
	return DGameServer.allocate_memory(n);
}

void gate_handler::write_packet(TPacketSendInfo_t* packet_info)
{
	DGameServer.push_write_packets(packet_info);
}

void gate_handler::handle_init()
{
	log_info("'%"I64_FMT"u', handle init", get_socket_index());
	server_info_packet server_info;
	DGameServer.get_server_info(server_info.m_server_info);
	server_info.m_len = sizeof(server_info);
	send_packet(&server_info);
}

void gate_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}

bool gate_handler::handle_server_info(packet_base * packet)
{
	DRpcWrapper.register_handle_info(m_rpc_client, (server_info_packet*)packet);

	dynamic_string p1("xiedi");
	uint16 p2 = 65500;
	std::array<char, 127> p3;
	memset(p3.data(), 0, 127);
	memcpy(p3.data(), "hello world", 11);

	uint8 p2_1 = 99;
	std::array<char, 33> p2_2;
	memset(p2_2.data(), 0, 33);
	memcpy(p2_2.data(), "mowang", 6);

	mailbox_info mailbox;
	mailbox.server_id = 100;
	mailbox.game_id = 1;

	DRpcWrapper.call_stub(mailbox, "game_server", "game_rpc_func_1", p1, p2, p3);

	DRpcWrapper.call_stub(mailbox, "game_server", "game_rpc_func_2", p2_1, p2_2);
	return true;
}

bool gate_handler::handle_transfer_client(packet_base * packet)
{
	transfer_client_packet* transfer_info = (transfer_client_packet*)packet;
	DGameServer.transfer_client(transfer_info->m_client_id, (packet_base*)transfer_info->m_buffer);
	return true;
}
