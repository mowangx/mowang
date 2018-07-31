
#include "ws_service.h"

#include "ws_manager.h"

ws_service::ws_service(game_process_type process_type) : service(process_type)
{
	m_write_ws_packets.clear();
	m_cmd_2_parse_func.clear();
}

ws_service::~ws_service()
{
	m_cmd_2_parse_func.clear();
}

bool ws_service::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		log_error("init service failed!");
		return false;
	}

	if (!DWSNetMgr.init(m_server_info.process_info.process_type, process_id)) {
		log_error("init websocket manager failed");
		return false;
	}

	init_ws_process_func();
	return true;
}

void ws_service::init_ws_process_func()
{
}

void ws_service::init_threads()
{
	std::thread log_thread(std::bind(&ws_service::log_run, this));
	std::thread net_thread(std::bind(&ws_service::net_run, this));
	std::thread ws_thread(std::bind(&ws_service::ws_run, this));

	work_run();

	log_thread.join();
	net_thread.join();
	ws_thread.join();
}

void ws_service::ws_run()
{
}

void ws_service::do_loop(TGameTime_t diff)
{
	TBaseType_t::do_loop(diff);
	do_ws_loop(diff);
}

void ws_service::do_ws_loop(TGameTime_t diff)
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

	process_ws_init_sockets(add_sockets);

	process_ws_packets(read_packets);

	process_ws_close_sockets(del_sockets);

	DWSNetMgr.swap_login_2_net(m_write_ws_packets, read_packets, m_wait_kick_ws_sockets, del_sockets);

	m_write_ws_packets.clear();
	m_wait_kick_ws_sockets.clear();
}

void ws_service::push_ws_write_packets(TSocketIndex_t socket_index, const std::string& packet)
{
	std::string msg = "";
	msg.push_back(START_CHARACTER);
	msg += packet;
	msg.push_back(END_CHARACTER);
	packet_send_info* packet_info = allocate_packet_info();
	packet_info->socket_index = socket_index;
	packet_info->buffer_info.len = (uint32)msg.length();
	packet_info->buffer_info.buffer = allocate_memory(packet_info->buffer_info.len);
	memcpy(packet_info->buffer_info.buffer, msg.c_str(), packet_info->buffer_info.len);
	m_write_ws_packets.push_back(packet_info);
}

void ws_service::process_ws_init_sockets(std::vector<web_socket_wrapper_base*>& sockets)
{
}

void ws_service::process_ws_close_sockets(std::vector<web_socket_wrapper_base*>& sockets)
{
}

void ws_service::process_ws_packets(std::vector<ws_packet_recv_info*>& packets)
{
	boost::property_tree::ptree json;
	for (auto packet_info : packets) {
		std::string str(packet_info->buffer_info.buffer, packet_info->buffer_info.len);
		std::stringstream json_stream(str);
		log_debug("parse ws packet! socket index %" I64_FMT "u, %s", packet_info->socket->get_socket_index(), json_stream.str().c_str());
		json.clear();
		boost::property_tree::read_json(json_stream, json);
		std::string cmd = json.get<std::string>("cmd");
		auto itr = m_cmd_2_parse_func.find(cmd);
		if (itr != m_cmd_2_parse_func.end()) {
			itr->second(packet_info->socket->get_socket_index(), &json);
		}
		else {
			log_info("parse ws packet but not find cmd! %s", cmd.c_str());
		}
	}
}
