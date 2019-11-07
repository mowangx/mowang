
#ifndef _ROBOT_SERVER_H_
#define _ROBOT_SERVER_H_

#include <unordered_map>

#include "singleton.h"
#include "ws_service.h"
#include "dynamic_array.h"

class rpc_client;

class robot_server : public ws_service, public singleton<robot_server>
{
	typedef ws_service TBaseType_t;

public:
	robot_server();
	virtual ~robot_server() override;

public:
	virtual bool init(TProcessID_t process_id) override;
private:
	virtual void init_ws_process_func() override;
private:
	virtual void net_run() override;
	virtual void ws_run() override;

private:
	virtual void process_ws_init_sockets(std::vector<web_socket_wrapper_base*>& sockets) override;
private:
	void process_login(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_logout(TSocketIndex_t socket_index, boost::property_tree::ptree* json);
	void process_create_role(TSocketIndex_t socket_index, boost::property_tree::ptree* json);

public:
	void create_role(TSocketIndex_t socket_index);
	void logout(uint8 reason, TSocketIndex_t client_id);
};

#define DRobotServer singleton<robot_server>::get_instance()

#endif // !_ROBOT_SERVER_H_

