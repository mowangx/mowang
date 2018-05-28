
#include "rpc_wrapper.h"
#include "game_random.h"

rpc_wrapper::rpc_wrapper()
{
}

rpc_wrapper::~rpc_wrapper()
{
}

void rpc_wrapper::register_handler_info(rpc_client* client, const game_server_info& server_info)
{
	const game_process_info& process_info = server_info.process_info;
	uint64 key_id = get_key_id_by_process_id(process_info);
	auto id_itr = m_server_process_id_2_clients.find(key_id);
	if (id_itr != m_server_process_id_2_clients.end()) {
		id_itr->second->get_handler()->kick();
		log_info("kick server, server id = %u, process type = %u, process id = %u",
			process_info.server_id, process_info.process_type, process_info.process_id);
	}
	m_server_process_id_2_clients[key_id] = client;

	rpc_client_wrapper_info* wrapper_info = new rpc_client_wrapper_info(process_info.process_id, client);
	key_id = get_key_id_by_process_type(process_info.server_id, process_info.process_type);
	auto itr = m_server_process_type_2_clients.find(key_id);
	if (itr != m_server_process_type_2_clients.end()) {
		std::vector<rpc_client_wrapper_info*>& rpc_wrappers = itr->second;
		rpc_wrappers.push_back(wrapper_info);
	}
	else {
		std::vector<rpc_client_wrapper_info*> rpc_wrappers;
		rpc_wrappers.push_back(wrapper_info);
		m_server_process_type_2_clients[key_id] = rpc_wrappers;
	}
	
	m_server_manager.register_server(server_info);
	
	log_info("register handle info, server id = %d, process type = %d, process id = %d, listen ip = %s, port = %d",
		process_info.server_id, (TProcessType_t)process_info.process_type, process_info.process_id, server_info.ip.data(), server_info.port);
}

void rpc_wrapper::unregister_handler_info(TSocketIndex_t socket_index)
{
	for (auto itr = m_server_process_id_2_clients.begin(); itr != m_server_process_id_2_clients.end(); ++itr) {
		rpc_client* rpc = itr->second;
		if (NULL != rpc && NULL != rpc->get_handler() && rpc->get_handler()->get_socket_index() == socket_index) {
			m_server_process_id_2_clients.erase(itr);
			break;
		}
	}

	game_process_info process_info;
	for (auto itr = m_server_process_type_2_clients.begin(); itr != m_server_process_type_2_clients.end(); ++itr) {
		std::vector<rpc_client_wrapper_info*>& wrappers = itr->second;
		for (auto wrapper_itr = wrappers.begin(); wrapper_itr != wrappers.end(); ++wrapper_itr) {
			rpc_client_wrapper_info* wrapper_info = *wrapper_itr;
			if (wrapper_info->rpc->get_handler()->get_socket_index() == socket_index) {
				process_info.process_id = wrapper_info->process_id;
				parse_key_id_by_process_type(process_info, itr->first);
				delete wrapper_info;
				wrappers.erase(wrapper_itr);
				break;
			}
		}
	}

	m_server_manager.unregister_server(process_info);
}

void rpc_wrapper::register_stub_info(const std::string & stub_name, const game_process_info & process_info)
{
	log_info("register stub info, stub name = %s, process id = %d", stub_name.c_str(), process_info.process_id);
	m_stub_name_2_process_infos[stub_name] = process_info;
}

bool rpc_wrapper::get_server_info(const game_process_info& process_info, game_server_info & server_info) const
{
	return m_server_manager.get_server_info(process_info, server_info);
}

void rpc_wrapper::get_server_infos(TServerID_t server_id, TProcessType_t process_type, dynamic_array<game_server_info>& servers) const
{
	m_server_manager.get_server_infos(server_id, process_type, servers);
}

void rpc_wrapper::get_stub_infos(dynamic_array<game_stub_info>& stub_infos) const
{
	for (auto itr = m_stub_name_2_process_infos.begin(); itr != m_stub_name_2_process_infos.end(); ++itr) {
		game_stub_info stub_info;
		const std::string& stub_name = itr->first;
		memcpy((void*)stub_info.stub_name.data(), stub_name.c_str(), stub_name.length());
		stub_info.process_info = itr->second;
		stub_infos.push_back(stub_info);
	}
}

bool rpc_wrapper::get_server_simple_info_by_socket_index(game_process_info& process_info, TSocketIndex_t socket_index) const
{
	for (auto itr = m_server_process_id_2_clients.begin(); itr != m_server_process_id_2_clients.end(); ++itr) {
		const rpc_client* rpc = itr->second;
		if (rpc->get_handler()->get_socket_index() != socket_index) {
			continue;
		}

		parse_key_id_by_process_id(process_info, itr->first);
		return true;
	}

	for (auto itr = m_server_process_type_2_clients.begin(); itr != m_server_process_type_2_clients.end(); ++itr) {
		const std::vector<rpc_client_wrapper_info*>& wrappers = itr->second;
		for (auto wrapper_itr = wrappers.begin(); wrapper_itr != wrappers.end(); ++wrapper_itr) {
			const rpc_client_wrapper_info* wrapper_info = *wrapper_itr;
			if (wrapper_info->rpc->get_handler()->get_socket_index() != socket_index) {
				continue;
			}
			
			process_info.process_id = wrapper_info->process_id;
			parse_key_id_by_process_type(process_info, itr->first);
			return true;
		}
	}

	return false;
}

TSocketIndex_t rpc_wrapper::get_socket_index(const game_process_info& process_info) const
{
	rpc_client* rpc = get_client(process_info);
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

TProcessID_t rpc_wrapper::get_random_process_id(TServerID_t server_id, TProcessType_t process_type) const
{
	auto itr = m_server_process_type_2_clients.find(get_key_id_by_process_type(server_id, process_type));
	if (itr == m_server_process_type_2_clients.end()) {
		log_error("get random process id for not find server id! server id = %u, process type = %u", server_id, process_type);
		return INVALID_PROCESS_ID;
	}
	const std::vector<rpc_client_wrapper_info*>& rpc_wrappers = itr->second;
	if (rpc_wrappers.empty()) {
		log_error("get random process id for empty! server id = %u, process type = %u", server_id, process_type);
		return INVALID_PROCESS_ID;
	}
	int wrapper_index = DGameRandom.get_rand<int>(0, (int)(rpc_wrappers.size() - 1));
	return rpc_wrappers[wrapper_index]->process_id;
}

rpc_client* rpc_wrapper::get_client(const game_process_info& process_info) const
{
	auto itr = m_server_process_id_2_clients.find(get_key_id_by_process_id(process_info));
	if (itr == m_server_process_id_2_clients.end()) {
		log_error("rpc wrapper get client failed! server id = %u, process type = %u, process id = %u", process_info.server_id, process_info.process_type, process_info.process_id);
		return NULL;
	}
	return itr->second;
}

rpc_client* rpc_wrapper::get_random_client(TServerID_t server_id, TProcessType_t process_type) const
{
	auto itr = m_server_process_type_2_clients.find(get_key_id_by_process_type(server_id, process_type));
	if (itr == m_server_process_type_2_clients.end()) {
		log_error("rpc wrapper get random client failed for not find server id! server id = %u, process type = %u", server_id, process_type);
		return NULL;
	}
	const std::vector<rpc_client_wrapper_info*>& rpc_wrappers = itr->second;
	if (rpc_wrappers.empty()) {
		log_error("rcp wrapper get random client failed for empty! server id = %u, process_type", server_id, process_type);
		return NULL;
	}
	int wrapper_index = DGameRandom.get_rand<int>(0, (int)(rpc_wrappers.size() - 1));
	return rpc_wrappers[wrapper_index]->rpc;
}

rpc_client* rpc_wrapper::get_stub_client_and_prcoess_info(const std::string & stub_name, game_process_info& process_info)
{
	auto itr = m_stub_name_2_process_infos.find(stub_name);
	if (itr != m_stub_name_2_process_infos.end()) {
		process_info = itr->second;
		return get_random_client(process_info.server_id, PROCESS_GATE);
	}
	log_error("get stub client and process info failed! stub name = %s", stub_name.c_str());
	return NULL;
}

uint64 rpc_wrapper::get_key_id_by_process_id(const game_process_info& process_info) const
{
	uint64 key_id = process_info.server_id;
	key_id = (key_id << (sizeof(process_info.process_type) * 8)) + process_info.process_type;
	key_id = (key_id << (sizeof(process_info.process_id) * 8)) + process_info.process_id;
	return key_id;
}

uint64 rpc_wrapper::get_key_id_by_process_type(TServerID_t server_id, TProcessType_t process_type) const
{
	uint64 key_id = server_id;
	return ((key_id << (sizeof(process_type) * 8)) + process_type);
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
