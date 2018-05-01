
#include "server_manager.h"

#include <algorithm>

server_manager::server_manager()
{
	clean_up();
}

server_manager::~server_manager()
{
	clean_up();
}

bool server_manager::get_server_info(const game_process_info& process_info, game_server_info & server_info) const
{
	auto server_itr = m_servers.find(process_info.server_id);
	if (server_itr == m_servers.end()) {
		return false;
	}

	const std::vector<game_server_info>& cur_servers = server_itr->second;
	for (auto cur_server_info : cur_servers) {
		if (cur_server_info.process_info.process_type != process_info.process_type ||
			cur_server_info.process_info.process_id != process_info.process_id) {
			continue;
		}
		server_info = cur_server_info;
		return true;
	}
	
	return false;
}

void server_manager::get_server_infos(TServerID_t server_id, TProcessType_t process_type, dynamic_array<game_server_info>& servers) const
{
	auto server_itr = m_servers.find(server_id);
	if (server_itr == m_servers.end()) {
		return;
	}

	const std::vector<game_server_info>& cur_servers = server_itr->second;
	for (auto server_info : cur_servers) {
		if (server_info.process_info.process_type == process_type) {
			servers.push_back(server_info);
		}
	}
}

void server_manager::register_server(const game_server_info& server_info)
{
	log_info("register server, server id = %u, process type = %d, process id = %u",
		server_info.process_info.server_id, server_info.process_info.process_type, server_info.process_info.process_id);
	auto itr = m_servers.find(server_info.process_info.server_id);
	if (itr == m_servers.end()) {
		std::vector<game_server_info> servers;
		m_servers[server_info.process_info.server_id] = servers;
		itr = m_servers.find(server_info.process_info.server_id);
	}
	itr->second.push_back(server_info);
}

void server_manager::unregister_server(const game_process_info& process_info)
{
	log_info("unregister server, server id = %u, process type = %d, process id = %u",
		process_info.server_id, process_info.process_type, process_info.process_id);
	auto server_itr = m_servers.find(process_info.server_id);
	if (server_itr == m_servers.end()) {
		log_info("unregister server failed, not find server id, server id = %u", process_info.server_id);
		return;
	}

	std::vector<game_server_info>& servers = server_itr->second;
	for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
		const game_server_info& server_info = *itr;
		if (server_info.process_info.process_type == process_info.process_type && 
			server_info.process_info.process_id == process_info.process_id) {
			servers.erase(itr);
			log_info("unregister server success");
			break;
		}
	}
}

void server_manager::clean_up()
{
	m_servers.clear();
}