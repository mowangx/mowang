
#ifndef _SERVER_MANAGER_H_
#define _SERVER_MANAGER_H_

#include <unordered_map>

#include "dynamic_array.h"
#include "packet_struct.h"

class server_manager
{
public:
	server_manager();
	~server_manager();

public:
	void get_server_info(TServerID_t server_id, TProcessID_t process_id, game_server_info& server_info) const;
	void get_server_infos(TServerID_t server_id, TProcessType_t process_type, dynamic_array<game_server_info>& servers) const;

public:
	void register_server(const game_server_info& server_info);
	void unregister_server(TServerID_t server_id, TProcessType_t process_type, TProcessID_t process_id);

private:
	void clean_up();

private:
	std::unordered_map<uint32, std::vector<game_server_info>> m_servers;
};

#endif // !_SERVER_MANAGER_H_

