
#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include "service.h"
#include "singleton.h"
#include "dynamic_array.h"

class gate_server : public service, public singleton<gate_server>
{
	typedef service TBaseType_t;

public:
	gate_server();
	~gate_server();

public:
	bool init(TProcessID_t process_id);

public:
	void register_server(TSocketIndex_t socket_index, const game_server_info& server_info);
	void on_query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);
	void login_server(TSocketIndex_t socket_index, TPlatformID_t platform_id, TServerID_t server_id, const TUserID_t& user_id);
	void on_game_lose(TServerID_t server_id, TProcessID_t game_id);
	void on_game_connect(const game_server_info& server_info);

public:
	void transfer_role(TServerID_t server_id, TProcessID_t game_id, TRoleID_t role_id, packet_base* packet);
	void transfer_stub(TServerID_t server_id, TProcessID_t game_id, packet_base* packet);
	void transfer_client(TSocketIndex_t client_id, packet_base* packet);
	void transfer_server(TSocketIndex_t client_id, packet_base* packet);

private:
	TSocketIndex_t get_server_socket_index(TSocketIndex_t client_id) const;

private:
	std::unordered_map<TSocketIndex_t, game_process_info> m_client_2_process;
};

#define DGateServer singleton<gate_server>::get_instance()

#endif