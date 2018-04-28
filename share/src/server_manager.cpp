
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

void server_manager::get_server_info(const game_process_info& process_info, game_server_info & server_info) const
{
	auto server_itr = m_servers.find(process_info.server_id);
	if (server_itr == m_servers.end()) {
		return;
	}

	const std::vector<game_server_info>& cur_servers = server_itr->second;
	for (auto cur_server_info : cur_servers) {
		if (cur_server_info.process_info.process_type != process_info.process_type ||
			cur_server_info.process_info.process_id != process_info.process_id) {
			continue;
		}
		server_info = cur_server_info;
		return;
	}
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
	m_servers[server_info.process_info.server_id].push_back(server_info);
}

void server_manager::unregister_server(const game_process_info& process_info)
{
	auto server_itr = m_servers.find(process_info.server_id);
	if (server_itr == m_servers.end()) {
		return;
	}

	std::vector<game_server_info>& servers = server_itr->second;
	for (auto itr = servers.begin(); itr != servers.end(); ++itr) {
		const game_server_info& server_info = *itr;
		if (server_info.process_info.process_type == process_info.process_type && 
			server_info.process_info.process_id == process_info.process_id) {
			servers.erase(itr);
			break;
		}
	}
}

void server_manager::clean_up()
{
	m_servers.clear();
}