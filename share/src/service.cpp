
#include "service.h"
#include "time_manager.h"
#include "socket_manager.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"

service::service(game_process_type process_type)
{
	m_disconnect_server_infos.clear();
	m_write_packets.clear();
	m_clients.clear();
	m_server_info.process_info.process_type = process_type;
}

service::~service()
{

}

bool service::init(TProcessID_t process_id)
{
	m_server_info.process_info.process_id = process_id;
	return true;
}

void service::run()
{
	TAppTime_t before_loop_time(0), after_loop_time(0);
	while (true) {
		before_loop_time = DTimeMgr.update();
		do_loop(0);
		after_loop_time = DTimeMgr.update();
		if ((after_loop_time - before_loop_time) < PER_FRAME_TIME) {
			std::this_thread::sleep_for(std::chrono::milliseconds(PER_FRAME_TIME + before_loop_time - after_loop_time));
		}
	}
}

void service::do_loop(TGameTime_t diff)
{
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
}

const game_server_info & service::get_server_info() const
{
	return m_server_info;
}

TPacketSendInfo_t * service::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char * service::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void service::push_write_packets(TPacketSendInfo_t * packet_info)
{
	m_write_packets.push_back(packet_info);
}

void service::register_client(rpc_client * client)
{
	m_clients[client->get_handler()->get_socket_index()] = client;
}

void service::unregister_client(TSocketIndex_t socket_index)
{
	TServerID_t server_id = INVALID_SERVER_ID;
	TProcessType_t process_type = INVALID_PROCESS_TYPE;
	TProcessID_t process_id = INVALID_PROCESS_ID;
	DRpcWrapper.get_server_simple_info_by_socket_index(server_id, process_type, process_id, socket_index);
	if (process_type == PROCESS_GAME_MANAGER) {
		game_server_info server_info;
		DRpcWrapper.get_server_info(server_id, process_id, server_info);
		m_disconnect_server_infos.push_back(server_info);
	}

	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		m_clients.erase(itr);
	}
}

void service::register_server(TSocketIndex_t socket_index, const game_server_info & server_info)
{
	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		DRpcWrapper.register_handler_info(itr->second, server_info);
	}
}

rpc_client* service::get_client(TSocketIndex_t socket_index)
{
	auto itr = m_clients.find(socket_index);
	return itr != m_clients.end() ? itr->second : NULL;
}