
#include "service.h"

#include <algorithm>

#include "debug.h"
#include "time_manager.h"
#include "tcp_manager.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "timer.h"

service::service(game_process_type process_type)
{
	m_reconnect_interval_time = 1;
	m_next_reconnect_time = INVALID_GAME_TIME;
	m_server_info.process_info.process_type = process_type;
	m_wait_kick_sockets.clear();
	m_wait_kick_ws_sockets.clear();
	m_write_packets.clear();
	m_disconnect_server_infos.clear();
	m_clients.clear();
}

service::~service()
{

}

void service::start(const std::string & module_name, const char * process_id)
{
	TProcessID_t pid = atoi(process_id);

	DLogMgr.init(module_name + process_id);
	gxSetDumpHandler(module_name);

	std::string config_module_name = module_name + process_id;
	if (!m_config.load("../config/game.ini", config_module_name, std::bind(&service::load_config, this, std::placeholders::_1, std::placeholders::_2))) {
		log_error("load ini config failed");
	}
	log_info("load ini config success");

	if (!init(pid)) {
		log_error("Init service failed");
		return;
	}

	log_info("Init service success");

	init_threads();
}

bool service::load_config(ini_file& ini, const std::string& module_name)
{
	return true;
}

bool service::init(TProcessID_t process_id)
{
	DGameRandom.reset(process_id);

	m_server_info.process_info.server_id = m_config.get_server_id();;
	m_server_info.process_info.process_id = process_id;

	const char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = m_config.get_listen_port();

	DTimer.init();

	if (!DNetMgr.init(m_server_info.process_info.process_type, process_id)) {
		log_error("init socket manager failed");
		return false;
	}

	return true;
}

void service::init_threads()
{
	std::thread log_thread(std::bind(&service::log_run, this));
	std::thread net_thread(std::bind(&service::net_run, this));
	//std::thread net_thread(std::bind(&service::net_run, this, std::ref(pid)));
	work_run();

	log_thread.join();
	net_thread.join();
}

void service::work_run()
{
	loop_run([this](TGameTime_t diff) -> bool {
		do_loop(diff);
		return true;
	});
}

void service::net_run()
{
	loop_run([this](TGameTime_t diff) -> bool {
		DNetMgr.update(diff);
		return true;
	});
}

void service::log_run()
{
	loop_run([this](TGameTime_t diff) -> bool {
		DLogMgr.flush();
		return true;
	});
}

void service::do_loop(TGameTime_t diff)
{
	// 
	DTimeMgr.update();
	DTimer.update(diff);

	std::vector<packet_recv_info*> read_packets;
	std::vector<packet_send_info*> finish_write_packets;
	std::vector<socket_base*> add_sockets;
	std::vector<socket_base*> del_sockets;

	DNetMgr.swap_net_2_logic(read_packets, finish_write_packets, add_sockets, del_sockets);

	for (auto packet_info : finish_write_packets) {
		m_mem_pool.deallocate((char*)packet_info->buffer_info.buffer);
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
	m_wait_kick_sockets.clear();

	try_reconnect_server();
}

void service::loop_run(const std::function<bool(TGameTime_t)>& callback)
{
	TGameTime_t frame_time = m_config.get_frame_time();
	while (true) {
		if (!callback(0)) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(frame_time));
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

const game_server_info & service::get_server_info() const
{
	return m_server_info;
}

packet_send_info * service::allocate_packet_info()
{
	return m_packet_pool.allocate();
}

char * service::allocate_memory(int n)
{
	return m_mem_pool.allocate(n);
}

void service::push_write_packets(packet_send_info * packet_info)
{
	m_write_packets.push_back(packet_info);
}

void service::kick_socket(TSocketIndex_t socket_index)
{
	m_wait_kick_sockets.push_back(socket_index);
}

void service::kick_ws_socket(TSocketIndex_t socket_index)
{
	m_wait_kick_ws_sockets.push_back(socket_index);
}

void service::register_client(rpc_client * client)
{
	log_info("register client, socket index %" I64_FMT "u", client->get_handler()->get_socket_index());
	m_clients[client->get_handler()->get_socket_index()] = client;
}

void service::unregister_client(TSocketIndex_t socket_index)
{
	log_info("unregister client, socket index %" I64_FMT "u", socket_index);
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
	log_info("register server, socket index %" I64_FMT "u", socket_index);
	auto itr = m_clients.find(socket_index);
	if (itr != m_clients.end()) {
		DRpcWrapper.register_handler_info(itr->second, server_info);
		on_connect(socket_index);
	}
	else {
		log_error("register server find socket index failed! socket index %" I64_FMT "u", socket_index);
	}
}

void service::on_connect(TSocketIndex_t socket_index)
{
}

void service::on_disconnect(TSocketIndex_t socket_index)
{
}

void service::on_register_entity(TSocketIndex_t socket_index, const dynamic_array<game_stub_info>& stub_infos)
{
	for (int i = 0; i < stub_infos.size(); ++i) {
		DRpcWrapper.register_stub_info(stub_infos[i].stub_name.data(), stub_infos[i].process_info);
	}
}

rpc_client* service::get_client(TSocketIndex_t socket_index)
{
	auto itr = m_clients.find(socket_index);
	return itr != m_clients.end() ? itr->second : NULL;
}