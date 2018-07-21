
#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include "service.h"
#include "singleton.h"
#include "dynamic_array.h"
#include "game_struct.h"

class game_server_handler;

class gate_server : public service, public singleton<gate_server>
{
	typedef service TBaseType_t;

public:
	gate_server();
	~gate_server();

public:
	virtual bool load_config(ini_file& ini, const std::string& module_name) override;
	virtual bool init(TProcessID_t process_id) override;
private:
	virtual void init_threads() override;
	virtual void work_run() override;
	virtual void net_run() override;
	void websocket_run();
	virtual bool connect_game_manager(const char* ip, TPort_t port) override;
	virtual void do_loop(TGameTime_t diff) override;
	virtual void on_disconnect(TSocketIndex_t socket_index) override;
private:
	void do_ws_loop(TGameTime_t diff);

public:
	void set_game_server_handler(game_server_handler* handler);

public:
	void on_register_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);
	void login_server(TSocketIndex_t socket_index, TPlatformID_t platform_id, TServerID_t server_id, const TUserID_t& user_id, TSocketIndex_t test_client_id);
	void logout_server(TSocketIndex_t socket_index);
	void update_process_info(TSocketIndex_t socket_index, TSocketIndex_t client_id, const game_process_info& process_info);
	void kick_socket_delay(TSocketIndex_t socket_index, TSocketIndex_t client_id);

public:
	TSocketIndex_t get_server_socket_index(TSocketIndex_t socket_index) const;

private:
	TPort_t m_ws_list_port;
	game_server_handler* m_game_server_handler;
	std::vector<socket_kick_info> m_delay_kick_sockets;
	std::unordered_map<TSocketIndex_t, game_process_info> m_client_2_process;
};

#define DGateServer singleton<gate_server>::get_instance()

#endif