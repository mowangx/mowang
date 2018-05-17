
#ifndef _ROBOT_SERVER_H_
#define _ROBOT_SERVER_H_

#include <unordered_map>

#include "singleton.h"
#include "service.h"
#include "dynamic_array.h"

class rpc_client;

class robot_server : public service, public singleton<robot_server>
{
	typedef service TBaseType_t;

public:
	robot_server();
	~robot_server();

public:
	bool init(TProcessID_t process_id) override;
private:
	virtual void net_run() override;

public:
	void logout(uint8 reason, TSocketIndex_t client_id);
};

#define DRobotServer singleton<robot_server>::get_instance()

#endif // !_ROBOT_SERVER_H_

