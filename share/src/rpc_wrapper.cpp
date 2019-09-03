
#include "rpc_wrapper.h"
#include "string_common.h"
#include "game_random.h"
#include "tcp_manager.h"
#include "service.h"

rpc_wrapper::rpc_wrapper()
{
	m_service = nullptr;
}

rpc_wrapper::~rpc_wrapper()
{
}

void rpc_wrapper::init(service* s)
{
	m_service = s;
}

void rpc_wrapper::register_handler_info(game_handler* handler)
{
	TSocketIndex_t socket_index = handler->get_socket_index();
	socket_base* socket = DNetMgr.get_socket(socket_index);
	std::string key = gx_to_string("%s%d", socket->get_remote_host_ip_str().data(), socket->get_remote_port());
	auto itr = m_directly_games.find(key);
	rpc_client* client = nullptr;
	if (itr == m_directly_games.end()) {
		client = new rpc_client(handler);
	}
	else {
		client = itr->second->rpc;
		client->set_handler(handler);
	}
	m_socket_index_2_client[handler->get_socket_index()] = client;
}

void rpc_wrapper::update_handler_info(TSocketIndex_t socket_index, const game_server_info& server_info)
{
	auto itr = m_socket_index_2_client.find(socket_index);
	if (itr == m_socket_index_2_client.end()) {
		log_error("update handler info failed! %" I64_FMT "u", socket_index);
		return;
	}
	rpc_client* client = itr->second;
	const game_process_info& process_info = server_info.process_info;
	uint32 key_id = get_key_id_by_process_id(process_info.process_type, process_info.process_id);
	m_process_id_2_client[key_id] = client;
	if (m_service->get_server_info().process_info.process_type == PROCESS_GAME) {
		if (process_info.process_type == PROCESS_GAME) {
			std::string key = gx_to_string("%s%d", server_info.ip.data(), server_info.port);
			auto game_itr = m_directly_games.find(key);
			rpc_client_wrapper_info* rpc_wrapper = nullptr;
			if (game_itr == m_directly_games.end()) {
				rpc_wrapper = new rpc_client_wrapper_info();
				rpc_wrapper->rpc = client;
				m_directly_games[key] = rpc_wrapper;
			}
			else {
				rpc_wrapper = game_itr->second;
				rpc_wrapper->rpc->process_cache_packets();
			}
			rpc_wrapper->send_time = DTimeMgr.now_sys_time();
		}
	}
	log_info("update handler info! socket index %" I64_FMT "u, server id %u, process type %u, process id %u, ip %s, port %u",
		socket_index, server_info.process_info.server_id, server_info.process_info.process_type, server_info.process_info.process_id,
		server_info.ip.data(), server_info.port);
}

void rpc_wrapper::unregister_handler_info(TSocketIndex_t socket_index)
{
	for (auto itr = m_directly_games.begin(); itr != m_directly_games.end(); ++itr) {
		rpc_client_wrapper_info* rpc_wrapper = itr->second;
		rpc_client* rpc = rpc_wrapper->rpc;
		if (NULL != rpc && NULL != rpc->get_handler() && rpc->get_handler()->get_socket_index() == socket_index) {
			delete rpc;
			delete rpc_wrapper;
			m_directly_games.erase(itr);
			return;
		}
	}
	for (auto itr = m_process_id_2_client.begin(); itr != m_process_id_2_client.end(); ++itr) {
		rpc_client* rpc = itr->second;
		if (NULL != rpc && NULL != rpc->get_handler() && rpc->get_handler()->get_socket_index() == socket_index) {
			delete rpc;
			m_process_id_2_client.erase(itr);
			return;
		}
	}
}

TSocketIndex_t rpc_wrapper::get_socket_index(const game_process_info& process_info) const
{
	rpc_client* rpc = get_client_by_process_id(process_info.process_type, process_info.process_id);
	if (NULL == rpc) {
		log_error("get socket index failed for rpc is NULL! server id = %u, process type = %u, process id = %u", 
			process_info.server_id, process_info.process_type, process_info.process_id);
		return INVALID_SOCKET_INDEX;
	}
	const game_handler* handler = rpc->get_handler();
	if (NULL == handler) {
		log_error("get socket index failed for handler is NULL! server id = %u, process type = %u, process id = %u",
			process_info.server_id, process_info.process_type, process_info.process_id);
		return INVALID_SOCKET_INDEX;
	}
	return handler->get_socket_index();
}

rpc_client* rpc_wrapper::get_client_by_socket_index(TSocketIndex_t socket_index) const
{
	auto itr = m_socket_index_2_client.find(socket_index);
	if (itr == m_socket_index_2_client.end()) {
		log_error("rpc wrapper get client failed! socket index = %" I64_FMT "u", socket_index);
		return nullptr;
	}
	return itr->second;
}

rpc_client* rpc_wrapper::get_client_by_process_type(TProcessType_t process_type) const
{
	std::vector<rpc_client*> clients;
	for (auto itr = m_process_id_2_client.begin(); itr != m_process_id_2_client.end(); ++itr) {
		uint32 key_id = itr->first;
		TProcessType_t cur_process_type = (TProcessType_t)(key_id >> (sizeof(TProcessID_t) * 8));
		if (cur_process_type == process_type) {
			clients.push_back(itr->second);
		}
	}
	if (clients.empty()) {
		log_error("rpc wrapper get client failed! process type = %u", process_type);
		return nullptr;
	}
	int idx = DGameRandom.get_rand<int>(0, (int)(clients.size() - 1));
	return clients[idx];
}

rpc_client* rpc_wrapper::get_client_by_process_id(TProcessType_t process_type, TProcessID_t process_id) const
{
	uint32 key_id = get_key_id_by_process_id(process_type, process_id);
	auto itr = m_process_id_2_client.find(key_id);
	if (itr == m_process_id_2_client.end()) {
		log_error("rpc wrapper get client failed! process type = %u, process id = %u", process_type, process_id);
		return nullptr;
	}
	return itr->second;
}

rpc_client* rpc_wrapper::get_client_by_address(const TIP_t& ip, TPort_t port)
{
	rpc_client_wrapper_info* rpc_wrapper = NULL;
	std::string key = gx_to_string("%s%d", ip.data(), port);
	auto itr = m_directly_games.find(key);
	if (itr != m_directly_games.end()) {
		rpc_wrapper = itr->second;
	}
	else {
		rpc_wrapper = new rpc_client_wrapper_info();
		rpc_wrapper->rpc = new rpc_client();
		m_directly_games[key] = rpc_wrapper;
		m_service->connect_server(ip.data(), port);
	}
	rpc_wrapper->send_time = DTimeMgr.now_sys_time();
	return rpc_wrapper->rpc;
}

TProcessID_t rpc_wrapper::get_random_process_id(TProcessType_t process_type) const
{
	std::vector<TProcessID_t> process_ids;
	for (auto itr = m_process_id_2_client.begin(); itr != m_process_id_2_client.end(); ++itr) {
		uint32 key_id = itr->first;
		TProcessType_t cur_process_type = (TProcessType_t)(key_id >> (sizeof(TProcessID_t) * 8));
		if (cur_process_type != process_type) {
			continue;
		}
		process_ids.push_back((TProcessID_t)(key_id & 0xFFFF));
	}
	if (process_ids.empty()) {
		return INVALID_PROCESS_ID;
	}
	int idx = DGameRandom.get_rand<int>(0, (int)(process_ids.size() - 1));
	return process_ids[idx];
}

uint32 rpc_wrapper::get_key_id_by_process_id(TProcessType_t process_type, TProcessID_t process_id) const
{
	uint32 key_id = process_type;
	key_id = (key_id << (sizeof(process_id) * 8)) + process_id;
	return key_id;
}

void rpc_wrapper::parse_key_id_by_process_id(game_process_info& process_info, uint64 key_id) const
{
	process_info.process_id = (TProcessID_t)(key_id & 0xFFFF);
	key_id = (key_id >> (sizeof(TProcessID_t) * 8));
	process_info.process_type = (TProcessType_t)(key_id & 0xFF);
	key_id = (key_id >> (sizeof(TProcessType_t) * 8));
	process_info.server_id = (TServerID_t)(key_id & 0xFFFF);
}

void rpc_wrapper::parse_key_id_by_process_type(game_process_info & process_info, uint64 key_id) const
{
	process_info.process_type = (TProcessType_t)(key_id & 0xFF);
	key_id = (key_id >> (sizeof(TProcessType_t) * 8));
	process_info.server_id = (TServerID_t)(key_id & 0xFFFF);
}
