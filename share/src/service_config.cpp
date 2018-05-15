
#include "service_config.h"
#include "ini_file.h"

service_config::service_config()
{
}

service_config::~service_config()
{
}

bool service_config::load(const std::string& filename)
{
	ini_file ini;
	if (!ini.open(filename.c_str())) {
		return false;
	}

	int frame_time = 30;
	if (!ini.read_type_if_exist("common", "FrameTime", frame_time)) {
		return false;
	}

	int game_2_listen_port = 0;
	if (!ini.read_type_if_exist("game2", "ListenPort", game_2_listen_port)) {
		return false;
	}
	return true;
}
