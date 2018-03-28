
#include "db_manager_handler.h"
#include "log.h"
#include "game_server.h"
#include "socket.h"
#include "rpc_proxy.h"
#include "rpc_client.h"

db_manager_handler::db_manager_handler() : packet_handler<db_manager_handler>()
{
	m_rpc_client = new rpc_client(this);
}

db_manager_handler::~db_manager_handler()
{

}

void db_manager_handler::Setup()
{
	register_handler(PACKET_ID_RPC_BY_INDEX, (packet_handler_func)&db_manager_handler::handle_rpc_by_index);
	register_handler(PACKET_ID_RPC_BY_NAME, (packet_handler_func)&db_manager_handler::handle_rpc_by_name);
}

TPacketInfo_t* db_manager_handler::create_packet_info()
{
	return DGameSerger.allocate_packet_info();
}

char* db_manager_handler::create_packet(int n)
{
	return DGameSerger.allocate_memory(n);
}

void db_manager_handler::write_packet(TPacketInfo_t* packet_info)
{
	DGameSerger.push_write_packets(packet_info);
}

void db_manager_handler::handle_close()
{
	log_info("'%"I64_FMT"u', handle close", get_socket_index());
	TBaseType_t::handle_close();
}

bool db_manager_handler::handle_rpc_by_index(packet_base* packet)
{
	rpc_by_index_packet* rpc_info = (rpc_by_index_packet*)packet;
	DRpcProxy.call(rpc_info->rpc_index, rpc_info->buffer);
	return true;
}

bool db_manager_handler::handle_rpc_by_name(packet_base* packet)
{
	rpc_by_name_packet* rpc_info = (rpc_by_name_packet*)packet;
	DRpcProxy.call(rpc_info->rpc_name, rpc_info->buffer);
	return true;
}