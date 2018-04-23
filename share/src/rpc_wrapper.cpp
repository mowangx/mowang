
#include "rpc_wrapper.h"
#include "game_random.h"

rpc_wrapper::rpc_wrapper()
{
}

rpc_wrapper::~rpc_wrapper()
{
}

void rpc_wrapper::register_handle_info(rpc_client * client, server_info_packet * packet)
{
	const game_server_info& server_info = packet->m_server_info;
	const game_process_info& process_info = server_info.process_info;

	m_clients[get_client_key_id(process_info.server_id, process_info.process_id)] = client;

	rpc_client_wrapper_info* wrapper_info = new rpc_client_wrapper_info();
	wrapper_info->rpc = client;
	wrapper_info->process_id = process_info.process_id;
	auto itr = m_client_wrappers.find(process_info.server_id);
	if (itr != m_client_wrappers.end()) {
		itr->second.push_back(wrapper_info);
	}
	else {
		std::vector<rpc_client_wrapper_info*> rpc_wrappers;
		rpc_wrappers.push_back(wrapper_info);
		m_client_wrappers[process_info.server_id] = rpc_wrappers;
	}
	
	m_gates.register_server(server_info);
	
	log_info("register handle info, server id = %d, process type = %d, process id = %d, listen ip = %s, port = %d",
		process_info.server_id, (TProcessType_t)process_info.process_type, process_info.process_id, server_info.ip.data(), server_info.port);
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

rpc_client * rpc_wrapper::get_client(TServerID_t server_id, TProcessID_t process_id) const
{
	uint32 key_id = get_client_key_id(server_id, process_id);
	auto itr = m_clients.find(key_id);
	if (itr == m_clients.end()) {
		return NULL;
	}
	return itr->second;
}

rpc_client* rpc_wrapper::get_random_client(TServerID_t server_id) const
{
	auto itr = m_client_wrappers.find(server_id);
	if (itr == m_client_wrappers.end()) {
		return NULL;
	}
	const std::vector<rpc_client_wrapper_info*>& rpc_wrappers = itr->second;
	int wrapper_index = DGameRandom.get_rand<int>(0, (int)(rpc_wrappers.size() - 1));
	return rpc_wrappers[wrapper_index]->rpc;
}

uint32 rpc_wrapper::get_client_key_id(TServerID_t server_id, TProcessID_t process_id) const
{
	uint32 key_id = server_id;
	return ((key_id << sizeof(process_id) * 8) + process_id);
}
