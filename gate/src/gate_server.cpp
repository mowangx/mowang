
#include "gate_server.h"
#include "gate_packet_handler.h"
#include "client_handler.h"
#include "rpc_client.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "tcp_manager.h"
#include "ws_manager.h"
#include "time_manager.h"
#include "timer.h"

gate_server::gate_server() : ws_service(PROCESS_GATE)
{
	m_ws_list_port = 0;
	m_delay_kick_sockets.clear();
	m_client_2_process.clear();
}

gate_server::~gate_server()
{

}

bool gate_server::load_config(ini_file& ini, const std::string & module_name)
{
	if (!ini.read_type_if_exist(module_name.c_str(), "ws_port", m_ws_list_port)) {
		log_error("load config failed for not find ws port in module %s!", module_name.c_str());
		return false;
	}

	return true;
}

bool gate_server::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	DRegisterServerRpc(this, gate_server, register_server, 2);
	DRegisterServerRpc(this, gate_server, login_server, 4);
	DRegisterServerRpc(this, gate_server, update_process_info, 3);
	DRegisterServerRpc(this, gate_server, kick_socket_delay, 2);
	DRegisterServerRpc(this, gate_server, on_register_entities, 5);
	DRegisterServerRpc(this, gate_server, on_unregister_process, 4);

	gate_packet_handler::Setup();
	client_handler::Setup();

	if (!DNetMgr.start_listen<client_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	log_info("init socket manager success");

	DWSNetMgr.start_listen(m_ws_list_port);

	log_info("init websocket manager success");

	return true;
}

void gate_server::init_ws_process_func()
{
	m_cmd_2_parse_func["login"] = std::bind(&gate_server::process_login, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["test"] = std::bind(&gate_server::process_test, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["ready_start"] = std::bind(&gate_server::process_ready_start, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["create_room"] = std::bind(&gate_server::process_create_room, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["enter_room"] = std::bind(&gate_server::process_enter_room, this, std::placeholders::_1, std::placeholders::_2);
	m_cmd_2_parse_func["pop_cards"] = std::bind(&gate_server::process_pop_cards, this, std::placeholders::_1, std::placeholders::_2);
}

void gate_server::do_loop(TGameTime_t diff)
{
	TBaseType_t::do_loop(diff);

	if (m_delay_kick_sockets.empty()) {
		return;
	}

	TGameTime_t cur_time = DTimeMgr.now_sys_time();
	std::vector<socket_kick_info> del_kick_infos;

	for (auto itr = m_delay_kick_sockets.begin(); itr != m_delay_kick_sockets.end(); ++itr) {
		const socket_kick_info& kick_info = *itr;
		if (kick_info.kick_time < cur_time) {
			kick_socket(kick_info.socket_index);
			del_kick_infos.push_back(kick_info);
		}
	}

	if (del_kick_infos.empty()) {
		return;
	}

	for (auto del_itr = del_kick_infos.begin(); del_itr != del_kick_infos.end(); ++del_itr) {
		auto itr = std::find(m_delay_kick_sockets.begin(), m_delay_kick_sockets.end(), *del_itr);
		if (itr != m_delay_kick_sockets.end()) {
			m_delay_kick_sockets.erase(itr);
		}
	}
}

void gate_server::on_disconnect(TSocketIndex_t socket_index)
{
	TBaseType_t::on_disconnect(socket_index);
	//game_process_info process_info;
	//if (!DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index) ||
	//	process_info.process_type != PROCESS_GAME) {
	//	return;
	//}

	//for (auto itr = m_client_2_process.begin(); itr != m_client_2_process.end(); ++itr) {
	//	const game_process_info& tmp_process_info = itr->second;
	//	if (tmp_process_info.process_id == process_info.process_id &&
	//		tmp_process_info.server_id == process_info.server_id) {
	//		kick_socket(itr->first);
	//	}
	//}
}

bool gate_server::connect_server(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<gate_packet_handler>(ip, port);
}

void gate_server::add_process(const game_server_info& server_info)
{
	log_info("on_register_servers, server id = %d, process type = %d, listen ip = %s, listen port = %d",
		server_info.process_info.server_id, server_info.process_info.process_type, server_info.ip.data(), server_info.port);
	if (server_info.process_info.process_type == PROCESS_GAME) {
		if (DNetMgr.start_connect<gate_packet_handler>(server_info.ip.data(), server_info.port)) {
			log_info("connect sucess, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
		else {
			log_info("connect failed, ip = %s, port = %d", server_info.ip.data(), server_info.port);
		}
	}
}

void gate_server::login_server(TSocketIndex_t socket_index, TPlatformID_t platform_id, TServerID_t server_id, const account_info& account)
{
	game_process_info process_info;
	process_info.server_id = server_id;
	process_info.process_type = PROCESS_GAME;
	process_info.process_id = DRpcWrapper.get_random_process_id(PROCESS_GAME);
	m_client_2_process[socket_index] = process_info;
	log_info("login, client id %" I64_FMT "u, token %s, game id %u", socket_index, account.token.data(), process_info.process_id);
	rpc_client* rpc = DRpcWrapper.get_client_by_process_id(PROCESS_GAME, process_info.process_id);
	if (NULL != rpc) {
		rpc->call_remote_func("login_server", socket_index, platform_id, account);
	}
	else {
		log_error("login failed for rpc is NULL! server id %u, process id %u, client id %" I64_FMT "u", 
			process_info.server_id, process_info.process_id, socket_index);
	}
}

void gate_server::logout_server(TSocketIndex_t socket_index)
{
	log_info("logout, client id %" I64_FMT "u", socket_index);
	auto itr = m_client_2_process.find(socket_index);
	if (itr != m_client_2_process.end()) {
		const game_process_info& process_info = itr->second;
		rpc_client* rpc = DRpcWrapper.get_client_by_process_id(PROCESS_GAME, process_info.process_id);
		if (NULL != rpc) {
			rpc->call_remote_func("logout_server", socket_index);
		}
		else {
			log_error("logout failed for rpc is NULL! server id %u, process id %u, client id %" I64_FMT "u",
				process_info.server_id, process_info.process_id, socket_index);
		}
	}
}

void gate_server::transfer_server(TSocketIndex_t socket_index, packet_base* packet)
{
	packet_send_info* packet_info = allocate_packet_info();
	packet_info->socket_index = get_server_socket_index(socket_index);
	TPacketLen_t len = (TPacketLen_t)(sizeof(transfer_client_packet) - 65000 + packet->get_packet_len());
	transfer_client_packet* transfer_packet = (transfer_client_packet*)allocate_memory(len);
	packet_info->buffer_info.len = len;
	packet_info->buffer_info.buffer = (char*)transfer_packet;
	transfer_packet->m_len = len;
	transfer_packet->m_id = PACKET_ID_TRANSFER_CLIENT;
	transfer_packet->m_client_id = socket_index;
	memcpy(transfer_packet->m_buffer, packet, packet->get_packet_len());
	push_write_packets(packet_info);
	log_info("transfer client packet to server! client id %" I64_FMT "u, socket index %" I64_FMT "u", packet_info->socket_index, socket_index);
}

void gate_server::update_process_info(TSocketIndex_t socket_index, TSocketIndex_t client_id, const game_process_info & process_info)
{
	auto itr = m_client_2_process.find(client_id);
	if (itr != m_client_2_process.end()) {
		log_info("update process info success! client id %" I64_FMT "u", client_id);
		itr->second = process_info;
	}
	else {
		log_error("update process info failed for not find client id! client id %" I64_FMT "u", client_id);
	}
}

void gate_server::kick_socket_delay(TSocketIndex_t socket_index, TSocketIndex_t client_id)
{
	socket_kick_info kick_info;
	kick_info.socket_index = client_id;
	kick_info.kick_time = DTimeMgr.now_sys_time() + DELAY_KICK_SOCKET_TIME;
	m_delay_kick_sockets.push_back(kick_info);
}

void gate_server::process_ws_close_sockets(std::vector<web_socket_wrapper_base*>& sockets)
{
	for (auto socket : sockets) {
		logout_server(socket->get_socket_index());
	}
}

void gate_server::process_login(TSocketIndex_t socket_index, boost::property_tree::ptree * json)
{
	TPlatformID_t platform_id = json->get<TPlatformID_t>("platform_id", 1);
	TServerID_t server_id = json->get<TServerID_t>("server_id", m_server_info.process_info.server_id);
	account_info account;
	memset(account.token.data(), 0, TOKEN_LEN);
	std::string cur_token = json->get<std::string>("code", "");
	memcpy(account.token.data(), cur_token.c_str(), cur_token.length());
	memset(account.role_name.data(), 0, ROLE_NAME_LEN);
	std::string cur_role_name = json->get<std::string>("name", "");
	memcpy(account.role_name.data(), cur_role_name.c_str(), cur_role_name.length());
	account.sex = json->get<TSex_t>("sex", 1);
	
	login_server(socket_index, platform_id, server_id, account);
	log_debug("parse login!!! socket index %" I64_FMT "u,  platform id %d, server_id %d", socket_index, platform_id, server_id);
}

void gate_server::process_test(TSocketIndex_t socket_index, boost::property_tree::ptree* json)
{
	//transfer_server_by_name_packet packet;
	//std::string func_name = "test";
	//int buffer_index = 0;
	//memcpy(packet.m_rpc_name, func_name.c_str(), func_name.length());
	//fill_packet(packet.m_buffer, buffer_index, json->get<uint8>("param_1", 0));
	//fill_packet(packet.m_buffer, buffer_index, json->get<uint16>("param_2", 0));
	//packet.m_len = TPacketLen_t(sizeof(packet) - sizeof(packet.m_buffer) + buffer_index);
	//transfer_server(socket_index, &packet);
	//log_debug("parse test!!! socket index %" I64_FMT "u,  param_1 %d, param_2 %d", socket_index, json->get<uint8>("param_1", 0), json->get<uint16>("param_2", 0));
}

void gate_server::process_ready_start(TSocketIndex_t socket_index, boost::property_tree::ptree * json)
{
	//transfer_server_by_name_packet packet;
	//std::string func_name = "ready_start";
	//int buffer_index = 0;
	//memcpy(packet.m_rpc_name, func_name.c_str(), func_name.length());
	//packet.m_len = TPacketLen_t(sizeof(packet) - sizeof(packet.m_buffer) + buffer_index);
	//transfer_server(socket_index, &packet);
}

void gate_server::process_create_room(TSocketIndex_t socket_index, boost::property_tree::ptree * json)
{
	//std::string tmp_pwd = json->get<std::string>("pwd", "");
	//dynamic_string pwd(tmp_pwd.c_str(), tmp_pwd.length());
	//transfer_server_by_name_packet packet;
	//std::string func_name = "create_room";
	//int buffer_index = 0;
	//memcpy(packet.m_rpc_name, func_name.c_str(), func_name.length());
	//fill_packet(packet.m_buffer, buffer_index, pwd);
	//packet.m_len = TPacketLen_t(sizeof(packet) - sizeof(packet.m_buffer) + buffer_index);
	//transfer_server(socket_index, &packet);
}

void gate_server::process_enter_room(TSocketIndex_t socket_index, boost::property_tree::ptree * json)
{
	//TRoomID_t room_id = json->get<TRoomID_t>("room_id", 0);
	//std::string tmp_pwd = json->get<std::string>("pwd", "");
	//dynamic_string pwd(tmp_pwd.c_str(), tmp_pwd.length());
	//transfer_server_by_name_packet packet;
	//std::string func_name = "enter_room";
	//int buffer_index = 0;
	//memcpy(packet.m_rpc_name, func_name.c_str(), func_name.length());
	//fill_packet(packet.m_buffer, buffer_index, room_id);
	//fill_packet(packet.m_buffer, buffer_index, pwd);
	//packet.m_len = TPacketLen_t(sizeof(packet) - sizeof(packet.m_buffer) + buffer_index);
	//transfer_server(socket_index, &packet);
}

void gate_server::process_pop_cards(TSocketIndex_t socket_index, boost::property_tree::ptree * json)
{
	//dynamic_array<TCardIndex_t> cards;
	//boost::property_tree::ptree json_cards = json->get_child("cards");
	//for (auto itr=json_cards.begin(); itr != json_cards.end(); ++itr)
	//{
	//	TCardIndex_t card_id = itr->second.get_value<TCardIndex_t>();
	//	cards.push_back(card_id);
	//}

	//transfer_server_by_name_packet packet;
	//std::string func_name = "pop_cards";
	//int buffer_index = 0;
	//memcpy(packet.m_rpc_name, func_name.c_str(), func_name.length());
	//fill_packet(packet.m_buffer, buffer_index, cards);
	//packet.m_len = TPacketLen_t(sizeof(packet) - sizeof(packet.m_buffer) + buffer_index);
	//transfer_server(socket_index, &packet);
}

TSocketIndex_t gate_server::get_server_socket_index(TSocketIndex_t socket_index) const
{
	auto itr = m_client_2_process.find(socket_index);
	if (itr != m_client_2_process.end()) {
		return DRpcWrapper.get_socket_index(itr->second);
	}
	return INVALID_SOCKET_INDEX;
}
