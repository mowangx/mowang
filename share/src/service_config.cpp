
#include "service_config.h"
#include "log.h"

service_config::service_config()
{
	clean_up();
}

service_config::~service_config()
{
	clean_up();
}

bool service_config::load(const std::string& filename, const std::string& module_name, const std::function<bool(ini_file&, const std::string&)>& load_config_func)
{
	ini_file ini;
	if (!ini.open(filename.c_str())) {
		log_error("load config failed for not find filename! filename = %s", filename.c_str());
		return false;
	}

	if (!ini.read_type_if_exist("common", "frame_time", m_frame_time)) {
		log_error("load config failed for not find frame time in module common!");
		return false;
	}

	if (!ini.read_type_if_exist("common", "server_id", m_server_id)) {
		log_error("load config failed for not find server id in module common!");
		return false;
	}

	if (!ini.read_type_if_exist(module_name.c_str(), "listen_port", m_listen_port)) {
		log_error("load config failed for not find listen port in module %s!", module_name.c_str());
		return false;
	}

	std::string game_manager_module_name = "game_manager1";
	std::string listen_ip;
	if (!ini.read_type_if_exist(game_manager_module_name.c_str(), "listen_ip", listen_ip)) {
		log_error("load config failed for not find listen ip in module %s!", game_manager_module_name.c_str());
		return false;
	}
	strcpy(m_game_manager_listen_ip, listen_ip.c_str());

	if (!ini.read_type_if_exist(game_manager_module_name.c_str(), "listen_port", m_game_manager_listen_port)) {
		log_error("load config failed for not find listen port in module %s!", game_manager_module_name.c_str());
		return false;
	}

	return load_config_func(ini, module_name);
}

TServerID_t service_config::get_server_id() const
{
	return m_server_id;
}

TPort_t service_config::get_listen_port() const
{
	return m_listen_port;
}

const char* service_config::get_game_manager_listen_ip() const
{
	return m_game_manager_listen_ip;
}

TPort_t service_config::get_game_manager_listen_port() const
{
	return m_game_manager_listen_port;
}

TGameTime_t service_config::get_frame_time() const
{
	return m_frame_time;
}

void service_config::clean_up()
{
	m_frame_time = INVALID_GAME_TIME;
	m_server_id = INVALID_SERVER_ID;
	m_listen_port = 0;
	m_game_manager_listen_port = 0;
	memset(m_game_manager_listen_ip, 0, IP_SIZE);
}