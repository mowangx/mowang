
#ifndef _ROBOT_SERVER_H_
#define _ROBOT_SERVER_H_

#include "singleton.h"
#include "service.h"
#include "dynamic_array.h"

class robot_server : public service, public singleton<robot_server>
{
	typedef service TBaseType_t;

public:
	robot_server();
	~robot_server();

public:
	bool init(TProcessID_t process_id);

public:
	void robot_rpc_func_1(TSocketIndex_t socket_index, const dynamic_string& p1, uint16 p2, const std::array<char, 127>& p3);
	void robot_rpc_func_2(TSocketIndex_t socket_index, uint8 p1, const std::array<char, 33>& p2);
};

#define DRobotServer singleton<robot_server>::get_instance()

#endif // !_ROBOT_SERVER_H_

