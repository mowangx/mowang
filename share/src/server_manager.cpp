
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

void server_manager::get_servers(TServerID_t server_id, dynamic_array<game_server_info>& servers)
{
	auto server_itr = m_servers.find(server_id);
	if (server_itr == m_servers.end()) {
		return;
	}
	std::vector<game_server_info>& cur_servers = server_itr->second;
	for (auto server_info : cur_servers) {
		servers.push_back(server_info);
	}
}

void server_manager::register_server(const game_server_info& server_info)
{
	m_servers[server_info.process_info.server_id].push_back(server_info);
}

void server_manager::unregister_server(const game_server_info& server_info)
{
	auto server_itr = m_servers.find(server_info.process_info.server_id);
	if (server_itr == m_servers.end()) {
		return;
	}
	std::vector<game_server_info>& servers = server_itr->second;
	auto itr = std::find(servers.begin(), servers.end(), server_info);
	if (itr != servers.end()) {
		servers.erase(itr);
	}
}

void server_manager::clean_up()
{
	m_servers.clear();
}