
#ifndef _SERVICE_CONFIG_H_
#define _SERVICE_CONFIG_H_

#include "base_util.h"

class service_config
{
public:
	service_config();
	~service_config();

public:
	bool load(const std::string& filename);
};

#endif // !_SERVICE_CONFIG_H_

