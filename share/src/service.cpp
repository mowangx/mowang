
#include "service.h"

#include <algorithm>

//#include <boost/thread.hpp>
//#include <boost/chrono.hpp>

#include "debug.h"
#include "time_manager.h"
#include "tcp_manager.h"
#include "rpc_client.h"
#include "rpc_wrapper.h"
#include "timer.h"
#include "rpc_proxy.h"
#include "etcd_manager.h"

service::service(game_process_type process_type)
{
	m_reconnect_interval_time = 1;
	m_next_reconnect_time = INVALID_GAME_TIME;
	m_server_info.process_info.process_type = process_type;
	m_wait_kick_sockets.clear();
	m_wait_kick_ws_sockets.clear();
	m_write_packets.clear();
	m_disconnect_server_infos.clear();
	m_handlers.clear();
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
	TGameTime_t now_time = DTimeMgr.now_app_time();
	DGameRandom.reset(now_time);

	m_server_info.process_info.server_id = m_config.get_server_id();;
	m_server_info.process_info.process_id = process_id;
	m_opt_id = ((TOptID_t)m_server_info.process_info.server_id << 48) + ((TOptID_t)m_server_info.process_info.process_id << 40);

	const char* ip = "127.0.0.1";
	memcpy(m_server_info.ip.data(), ip, strlen(ip));
	m_server_info.port = m_config.get_listen_port();

	DTimer.init();
	DRpcWrapper.init(this);

	if (!DNetMgr.init(m_server_info.process_info.process_type, process_id)) {
		log_error("init socket manager failed");
		return false;
	}

	DRegisterServerRpc(this, service, on_http_response, 4);

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
	loop_connect_http_client();
	loop_run([this](TGameTime_t diff) -> bool {
		do_loop(diff);
		return true;
	});
}

void service::net_run()
{
	loop_run([this](TGameTime_t diff) -> bool {
		net_loop(diff);
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

void service::net_loop(TGameTime_t diff)
{
	DNetMgr.update(diff);
}

void service::loop_run(const std::function<bool(TGameTime_t)>& callback)
{
	TGameTime_t frame_time = m_config.get_frame_time();
	while (true) {
		if (!callback(0)) {
			break;
		}

		//boost::this_thread::no_interruption_point::sleep_for(boost::chrono::milliseconds(frame_time));
		std::this_thread::sleep_for(std::chrono::milliseconds(frame_time));
	}
}

bool service::connect_server(const char* ip, TPort_t port)
{
	return true;
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
		//if (connect_game_manager(server_info.ip.data(), server_info.port)) {
		//	del_server_info.push_back(server_info);
		//}
		//else {
		//	log_error("connect game manager failed, ip = %s, port = %u", server_info.ip.data(), server_info.port);
		//}
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

void service::loop_connect_http_client()
{
	const char* ip = m_config.get_http_client_listen_ip();
	TPort_t port = m_config.get_http_client_listen_port();
	while (!connect_server(ip, port)) {
		log_error("connect http client failed, ip = %s, port = %u", ip, port);
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

void service::push_write_packets(packet_send_info* packet_info)
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

void service::register_handler(game_handler* handler)
{
	log_info("register handler, socket index %" I64_FMT "u", handler->get_socket_index());
	m_handlers[handler->get_socket_index()] = handler;
	DRpcWrapper.register_handler_info(handler);
}

void service::unregister_handler(TSocketIndex_t socket_index)
{
	log_info("unregister handler, socket index %" I64_FMT "u", socket_index);
	on_disconnect(socket_index);

	auto itr = m_handlers.find(socket_index);
	if (itr != m_handlers.end()) {
		m_handlers.erase(itr);
	}
}

void service::add_process(const game_server_info& server_info)
{
	log_info("add_process, server id = %d, process type = %d, listen ip = %s, listen port = %d",
		server_info.process_info.server_id, server_info.process_info.process_type, server_info.ip.data(), server_info.port);
}

void service::register_server(TSocketIndex_t socket_index, const game_server_info& server_info)
{
	log_info("update handler server info, socket index %" I64_FMT "u", socket_index);
	auto itr = m_handlers.find(socket_index);
	if (itr != m_handlers.end()) {
		DRpcWrapper.update_handler_info(socket_index, server_info);
		on_connect(socket_index);
	}
	else {
		log_error("update handler server info find socket index failed! socket index %" I64_FMT "u", socket_index);
	}
	if (server_info.process_info.process_type == PROCESS_HTTP_CLIENT) {
		DEtcdMgr.init(this);
	}
}

void service::http_request(const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl, const std::function<void(int, const dynamic_string&)>& callback)
{
	m_opt_id += 1;
	rpc_client* rpc = DRpcWrapper.get_client_by_process_type(PROCESS_HTTP_CLIENT);
	if (NULL != rpc) {
		rpc->call_remote_func("http_request", m_opt_id, host, url, params, usessl);
	}
	m_http_response_callbacks[m_opt_id] = callback;
}

void service::on_http_response(TSocketIndex_t socket_index, TOptID_t opt_id, int status, const dynamic_string & result)
{
	auto itr = m_http_response_callbacks.find(opt_id);
	if (itr == m_http_response_callbacks.end()) {
		return;
	}
	const std::function<void(int, const dynamic_string&)>& callback = itr->second;
	callback(status, result);
}

void service::on_register_entities(TSocketIndex_t socket_index, const dynamic_string& name, TWaitIndex_t wait_index, const dynamic_array<etcd_process_packet_info>& processes, const dynamic_array<etcd_entity_packet_info>& entities)
{
	DEtcdMgr.on_register_entities(name, wait_index, processes, entities);
}

void service::on_unregister_process(TSocketIndex_t socket_index, const dynamic_string& name, TWaitIndex_t wait_index, const game_process_info& process_info)
{
	DEtcdMgr.on_unregister_process(name, wait_index, process_info);
}

void service::on_connect(TSocketIndex_t socket_index)
{
}

void service::on_disconnect(TSocketIndex_t socket_index)
{
}

TServerID_t service::get_server_id() const
{
	return m_server_info.process_info.server_id;
}

TProcessID_t service::get_game_id() const
{
	return m_server_info.process_info.process_id;
}
