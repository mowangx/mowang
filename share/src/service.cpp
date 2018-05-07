
#include "service.h"
#include "time_manager.h"
#include "socket_manager.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "timer.h"

service::service(game_process_type process_type)
{
	m_reconnect_interval_time = 1;
	m_next_reconnect_time = INVALID_GAME_TIME;
	m_wait_kick_sockets.clear();
	m_write_packets.clear();
	m_disconnect_server_infos.clear();
	m_clients.clear();
	m_server_info.process_info.process_type = process_type;
}

service::~service()
{

}

bool service::init(TProcessID_t process_id)
{
	m_server_info.process_info.process_id = process_id;
	DTimer.init();
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
	DTimer.update(diff);

	std::vector<TPacketRecvInfo_t*> read_packets;
	std::vector<TPacketSendInfo_t*> finish_write_packets;
	std::vector<socket_base*> add_sockets;
	std::vector<socket_base*> del_sockets;

	DNetMgr.swap_net_2_logic(read_packets, finish_write_packets, add_sockets, del_sockets);

	for (auto packet_info : finish_write_packets) {
		m_mem_pool.deallocate((char*)packet_info->packet);
		m_packet_pool.deallocate(packet_info);
	}

	for (auto socket : add_sockets) {
		socket->get_packet_handler()->handle_init();
	}

	for (auto packet_info : read_packets) {
		packet_info->socket->get_packet_handler()->handle(packet_info->packet);
	}

	for (auto socket : del_sockets) {
		socket->get_packet_handler()->handle_close();
	}

	DNetMgr.swap_login_2_net(m_write_packets, read_packets, m_wait_kick_sockets, del_sockets);

	m_write_packets.clear();

	try_reconnect_server();
}

bool service::connect_game_manager(const char * ip, TPort_t port)
{
	return true;
}

void service::connect_game_manager_loop(const char * ip, TPort_t port)
{
	while (!connect_game_manager(ip, port)) {
		log_error("connect game manager failed, ip = %s, port = %u", ip, port);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

void service::try_reconnect_server()
{
	if (m_disconnect_server_infos.empty() || DTimeMgr.now_sys_time() < m_next_reconnect_time) {
		return;
	}

	if (m_reconnect_interval_time < 2) {
		m_reconnect_interval_time = 1 + m_server_info.process_info.process_id;
	}
	m_reconnect_interval_time <<= 1;
	if (m_reconnect_interval_time > 128) {
		m_reconnect_interval_time = 128;
	}
	m_next_reconnect_time = DTimeMgr.now_sys_time() + m_reconnect_interval_time;

	std::vector<game_server_info> del_server_info;

	for (auto itr = m_disconnect_server_infos.begin(); itr != m_disconnect_server_infos.end(); ++itr) {
		const game_server_info& server_info = *itr;
		if (connect_game_manager(server_info.ip.data(), server_info.port)) {
			del_server_info.push_back(server_info);
		}
		else {
			log_error("connect game manager failed, ip = %s, port = %u", server_info.ip.data(), server_info.port);
		}
	}

	if (del_server_info.empty()) {
		return;
	}
	
	for (auto del_itr = del_server_info.begin(); del_itr != del_server_info.end(); ++del_itr) {
		auto itr = std::find(m_disconnect_server_infos.begin(), m_disconnect_server_infos.end(), *del_itr);
		if (itr != m_disconnect_server_infos.end()) {
			m_disconnect_server_infos.erase(itr);
		}
	}

	if (m_disconnect_server_infos.empty()) {
		m_reconnect_interval_time = 1 + m_server_info.process_info.process_id;
	}
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

void service::kick_socket(TSocketIndex_t socket_index)
{
	m_wait_kick_sockets.push_back(socket_index);
}

void service::register_client(rpc_client * client)
{
	log_info("register client, socket index = '%"I64_FMT"u'", client->get_handler()->get_socket_index());
	m_clients[client->get_handler()->get_socket_index()] = client;
}

void service::unregister_client(TSocketIndex_t socket_index)
{
	log_info("unregister client, socket index = '%"I64_FMT"u'", socket_index);
	on_disconnect(socket_index);

	game_process_info process_info;
	DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index);
	if (process_info.process_type == PROCESS_GAME_MANAGER) {
		game_server_info server_info;
		DRpcWrapper.get_server_info(process_info, server_info);
		m_disconnect_server_infos.push_back(server_info);
	}

	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		m_clients.erase(itr);
	}
}

void service::register_server(TSocketIndex_t socket_index, const game_server_info & server_info)
{
	log_info("register server, socket index = '%"I64_FMT"u'", socket_index);
	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		DRpcWrapper.register_handler_info(itr->second, server_info);
		on_connect(socket_index);
	}
	else {
		log_error("register server find socket index failed! socket index = '%"I64_FMT"u'", socket_index);
	}
}

void service::on_connect(TSocketIndex_t socket_index)
{
}

void service::on_disconnect(TSocketIndex_t socket_index)
{
}

void service::on_register_entity(TSocketIndex_t socket_index, const dynamic_string& stub_name, const game_process_info& process_info)
{
	std::string tmp_stub_name = stub_name.data();
	DRpcWrapper.register_stub_info(tmp_stub_name, process_info);
}

rpc_client* service::get_client(TSocketIndex_t socket_index)
{
	auto itr = m_clients.find(socket_index);
	return itr != m_clients.end() ? itr->second : NULL;
}