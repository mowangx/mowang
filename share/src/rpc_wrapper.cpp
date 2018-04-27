
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

	m_server_process_id_2_clients[get_key_id_by_process_id(process_info.server_id, process_info.process_id)] = client;

	rpc_client_wrapper_info* wrapper_info = new rpc_client_wrapper_info();
	wrapper_info->rpc = client;
	wrapper_info->process_id = process_info.process_id;
	auto itr = m_server_process_type_2_clients.find(get_key_id_by_process_type(process_info.server_id, process_info.process_type));
	if (itr != m_server_process_type_2_clients.end()) {
		itr->second.push_back(wrapper_info);
	}
	else {
		std::vector<rpc_client_wrapper_info*> rpc_wrappers;
		rpc_wrappers.push_back(wrapper_info);
		m_server_process_type_2_clients[get_key_id_by_process_type(process_info.server_id, process_info.process_type)] = rpc_wrappers;
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

	TServerID_t server_id = INVALID_SERVER_ID;
	TProcessType_t process_type = INVALID_PROCESS_TYPE;
	TProcessID_t process_id = INVALID_PROCESS_ID;

	for (auto itr = m_server_process_type_2_clients.begin(); itr != m_server_process_type_2_clients.end(); ++itr) {
		std::vector<rpc_client_wrapper_info*>& wrappers = itr->second;
		for (auto wrapper_itr = wrappers.begin(); wrapper_itr != wrappers.end(); ++wrapper_itr) {
			rpc_client_wrapper_info* wrapper_info = *wrapper_itr;
			if (NULL != wrapper_info && NULL != wrapper_info->rpc && NULL != wrapper_info->rpc->get_handler() && 
				wrapper_info->rpc->get_handler()->get_socket_index() == socket_index) {
				uint32 key_id = itr->first;
				server_id = (TServerID_t)(key_id >> (sizeof(TProcessType_t) * 8));
				process_type = (TProcessType_t)(key_id & 0xFF);
				process_id = wrapper_info->process_id;
				wrappers.erase(wrapper_itr);
				break;
			}
		}
	}

	m_server_manager.unregister_server(server_id, process_type, process_id);
}

void rpc_wrapper::get_server_infos(TServerID_t server_id, TProcessType_t process_type, dynamic_array<game_server_info>& servers)
{
	m_server_manager.get_servers(server_id, process_type, servers);
}

TSocketIndex_t rpc_wrapper::get_socket_index(TServerID_t server_id, TProcessID_t process_id) const
{
	rpc_client* rpc = get_client(server_id, process_id);
	if (NULL == rpc) {
		return INVALID_SOCKET_INDEX;
	}
	const game_handler* handler = rpc->get_handler();
	if (NULL == handler) {
		return INVALID_SOCKET_INDEX;
	}
	return handler->get_socket_index();
}

TProcessID_t rpc_wrapper::get_random_process_id(TServerID_t server_id, TProcessType_t process_type) const
{
	auto itr = m_server_process_type_2_clients.find(get_key_id_by_process_type(server_id, process_type));
	if (itr == m_server_process_type_2_clients.end()) {
		return NULL;
	}
	const std::vector<rpc_client_wrapper_info*>& rpc_wrappers = itr->second;
	int wrapper_index = DGameRandom.get_rand<int>(0, (int)(rpc_wrappers.size() - 1));
	return rpc_wrappers[wrapper_index]->process_id;
}

rpc_client * rpc_wrapper::get_client(TServerID_t server_id, TProcessID_t process_id) const
{
	auto itr = m_server_process_id_2_clients.find(get_key_id_by_process_id(server_id, process_id));
	if (itr == m_server_process_id_2_clients.end()) {
		return NULL;
	}
	return itr->second;
}

rpc_client* rpc_wrapper::get_random_client(TServerID_t server_id, TProcessType_t process_type) const
{
	auto itr = m_server_process_type_2_clients.find(get_key_id_by_process_type(server_id, process_type));
	if (itr == m_server_process_type_2_clients.end()) {
		return NULL;
	}
	const std::vector<rpc_client_wrapper_info*>& rpc_wrappers = itr->second;
	int wrapper_index = DGameRandom.get_rand<int>(0, (int)(rpc_wrappers.size() - 1));
	return rpc_wrappers[wrapper_index]->rpc;
}

uint32 rpc_wrapper::get_key_id_by_process_id(TServerID_t server_id, TProcessID_t process_id) const
{
	uint32 key_id = server_id;
	return ((key_id << sizeof(process_id) * 8) + process_id);
}

uint32 rpc_wrapper::get_key_id_by_process_type(TServerID_t server_id, TProcessType_t process_type) const
{
	uint32 key_id = server_id;
	return ((key_id << sizeof(process_type) * 8) + process_type);
}
