
#ifndef _SERVICE_CONFIG_H_
#define _SERVICE_CONFIG_H_

#include <functional>

#include "socket_util.h"
#include "ini_file.h"

class service_config
{
public:
	service_config();
	~service_config();

public:
	bool load(const std::string& filename, const std::string& module_name, const std::function<bool(ini_file&, const std::string&)>& load_config_func);

public:
	TGameTime_t get_frame_time() const;
	TServerID_t get_server_id() const;
	TPort_t get_listen_port() const;
	const char* get_game_manager_listen_ip() const;
	TPort_t get_game_manager_listen_port() const;

private:
	void clean_up();

private:
	TGameTime_t m_frame_time;
	TServerID_t m_server_id;
	TPort_t m_listen_port;
	TPort_t m_game_manager_listen_port;
	char m_game_manager_listen_ip[IP_SIZE];
};

#endif // !_SERVICE_CONFIG_H_

