
#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include "ws_service.h"
#include "singleton.h"
#include "dynamic_array.h"
#include "game_struct.h"

class game_server_handler;

class gate_server : public ws_service, public singleton<gate_server>
{
	typedef ws_service TBaseType_t;

public:
	gate_server();
	virtual ~gate_server() override;

public:
	virtual bool load_config(ini_file& ini, const std::string& module_name) override;
	virtual bool init(TProcessID_t process_id) override;
private:
	virtual void init_ws_process_func() override;
private:
	virtual void work_run() override;

private:
	virtual void do_loop(TGameTime_t diff) override;
	virtual void on_disconnect(TSocketIndex_t socket_index) override;

private:
	virtual bool connect_game_manager(const char* ip, TPort_t port) override;

public:
	void on_register_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type, const dynamic_array<game_server_info>& servers);
	void login_server(TSocketIndex_t socket_index, TPlatformID_t platform_id, TServerID_t server_id, const account_info& account);
	void logout_server(TSocketIndex_t socket_index);
	void transfer_server(TSocketIndex_t socket_index, packet_base* packet);
	void update_process_info(TSocketIndex_t socket_index, TSocketIndex_t client_id, const game_process_info& process_info);
	void kick_socket_delay(TSocketIndex_t socket_index, TSocketIndex_t client_id);

private:
	virtual void process_ws_close_sockets(std::vector<web_socket_wrapper_base*>& sockets);
private:
	void process_login(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_test(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_create_room(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_enter_room(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_pop_cards(TSocketIndex_t socket_index, boost::property_tree::ptree* json);

public:
	TSocketIndex_t get_server_socket_index(TSocketIndex_t socket_index) const;

private:
	TPort_t m_ws_list_port;
	std::vector<socket_kick_info> m_delay_kick_sockets;
	std::unordered_map<TSocketIndex_t, game_process_info> m_client_2_process;
};

#define DGateServer singleton<gate_server>::get_instance()

#endif