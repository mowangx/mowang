
#ifndef _GATE_SERVER_H_
#define _GATE_SERVER_H_

#include "ws_service.h"
#include "singleton.h"
#include "dynamic_array.h"
#include "server_struct.h"

class rpc_client;

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
	virtual void do_loop(TGameTime_t diff) override;
	virtual void on_disconnect(TSocketIndex_t socket_index) override;
public:
	virtual bool connect_server(const char* ip, TPort_t port) override;

public:
	virtual void add_process(const game_server_info& server_info) override;
	void on_client_connect(TSocketIndex_t socket_index);
	void on_client_disconnect(TSocketIndex_t socket_index);
	void kick_socket_delay(TSocketIndex_t socket_index, TSocketIndex_t client_id);
	void update_client_process_info(TSocketIndex_t socket_index, const game_process_info& process_info);

private:
	virtual void process_ws_close_sockets(std::vector<web_socket_wrapper_base*>& sockets);
private:
	void process_login(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_create_role(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_income_resource(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_build_building(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_up_building_level(TSocketIndex_t socket_index, boost::property_tree::ptree* json);

public:
	virtual TProcessID_t get_process_id_by_client_id(TSocketIndex_t client_id) const override;

private:
	TPort_t m_ws_port;
	std::vector<socket_kick_info> m_delay_kick_sockets;
	std::unordered_map<TSocketIndex_t, game_process_info> m_client_2_process;
};

#define DGateServer singleton<gate_server>::get_instance()

#endif