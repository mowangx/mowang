
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

	int frame_time;
	if (ini.read_type_if_exist("common", "FrameTime", frame_time))
	{
		
	}
	return false;
}
