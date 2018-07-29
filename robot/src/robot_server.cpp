

#include "robot_server.h"
#include "gate_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "game_random.h"
#include "tcp_manager.h"
#include "ws_manager.h"

robot_server::robot_server() : service(PROCESS_ROBOT)
{
}

robot_server::~robot_server()
{
	
}

bool robot_server::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	gate_handler::Setup();

	DRegisterClientRpc(this, robot_server, logout, 2);

	return true;
}

void robot_server::init_threads()
{
	if (!DWSNetMgr.init(m_server_info.process_info.process_type, m_server_info.process_info.process_id)) {
		log_error("init websocket manager failed");
		return;
	}

	std::thread log_thread(std::bind(&service::log_run, this));
	std::thread net_thread(std::bind(&robot_server::net_run, this));
	std::thread ws_thread(std::bind(&robot_server::websocket_run, this));
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> ws_client_thread = DWSNetMgr.init_client();

	work_run();

	log_thread.join();
	net_thread.join();
	ws_thread.join();
	ws_client_thread->join();
}

void robot_server::do_loop(TGameTime_t diff)
{
	TBaseType_t::do_loop(diff);

	do_ws_loop(diff);
}

void robot_server::net_run()
{
	log_info("init socket manager success");
	while (true) {
		DNetMgr.update(0);
		//DNetMgr.test_kick();

		if (DNetMgr.socket_num() < 2) {
			if (!DNetMgr.start_connect<gate_handler>("127.0.0.1", DGameRandom.get_rand<int>(10010, 10012))) {
				log_info("connect server failed");
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}

void robot_server::websocket_run()
{
	DWSNetMgr.start_connect("ws://127.0.0.1:10101");
	log_info("init websocket manager success");

	bool need_send = true;

	while (true) {
		DWSNetMgr.update_client(0);
		std::this_thread::sleep_for(std::chrono::milliseconds(2));

		if (!need_send) {
			continue;
		}

		std::vector<web_socket_wrapper_base*> sockets;
		DWSNetMgr.test_get_sockets(sockets);
		std::string s = "{\"cmd\":\"hello\", \"params\":\"world\"}";
		for (auto socket : sockets) {
			packet_send_info* packet_info = allocate_packet_info();
			packet_info->socket_index = socket->get_socket_index();
			packet_info->buffer_info.len = s.length();
			packet_info->buffer_info.buffer = allocate_memory(packet_info->buffer_info.len);
			memcpy(packet_info->buffer_info.buffer, s.c_str(), packet_info->buffer_info.len);
			push_ws_write_packets(packet_info);
			log_debug("robot send msg!!!! %d, %s", packet_info->buffer_info.len, packet_info->buffer_info.buffer);
			need_send = false;
		}
	}
}

void robot_server::do_ws_loop(TGameTime_t diff)
{
	std::vector<ws_packet_recv_info*> read_packets;
	std::vector<packet_send_info*> finish_write_packets;
	std::vector<web_socket_wrapper_base*> add_sockets;
	std::vector<web_socket_wrapper_base*> del_sockets;

	DWSNetMgr.swap_net_2_logic(read_packets, finish_write_packets, add_sockets, del_sockets);

	for (auto packet_info : finish_write_packets) {
		m_mem_pool.deallocate((char*)packet_info->buffer_info.buffer);
		m_packet_pool.deallocate(packet_info);
	}

	for (auto packet_info : read_packets) {
		process_ws_packet(packet_info);
	}

	//for (auto socket : del_sockets) {
	//	logout_server(socket->get_socket_index());
	//}

	DWSNetMgr.swap_login_2_net(m_write_ws_packets, read_packets, m_wait_kick_ws_sockets, del_sockets);

	m_write_ws_packets.clear();
	m_wait_kick_ws_sockets.clear();
}

void robot_server::process_ws_packet(ws_packet_recv_info * packet_info)
{
	log_info("recv buff info %s, len %d", packet_info->buffer_info.buffer, packet_info->buffer_info.len);
}

void robot_server::logout(uint8 reason, TSocketIndex_t client_id)
{
	log_info("logout! reason = %u, client id = %" I64_FMT "u", reason, client_id);
	//kick_socket(client_id);
}
