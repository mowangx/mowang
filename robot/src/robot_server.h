
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
	bool init(TProcessID_t process_id);

public:
	void robot_rpc_func_1(TProcessID_t process_id, TSocketIndex_t socket_index, const dynamic_string& p1, TRoleID_t role_id, const std::array<char, 127>& p3, TSocketIndex_t client_id);
	void robot_rpc_func_2(TProcessID_t process_id, TSocketIndex_t socket_index, TRoleID_t role_id, const std::array<char, 33>& p2);

private:
	rpc_client* get_robot_client(TProcessID_t process_id, TSocketIndex_t socket_index);

private:
	std::unordered_map<TSocketIndex_t, TSocketIndex_t> m_sockets;
};

#define DRobotServer singleton<robot_server>::get_instance()

#endif // !_ROBOT_SERVER_H_

