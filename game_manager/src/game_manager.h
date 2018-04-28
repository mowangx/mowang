
#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include "singleton.h"
#include "service.h"

class game_manager : public service, public singleton<game_manager>
{
	typedef service TBaseType_t;

public:
	game_manager();
	~game_manager();
	
public:
	bool init(TProcessID_t process_id);

public:
	void register_server(TSocketIndex_t socket_index, const game_server_info& server_info);
	void query_servers(TSocketIndex_t socket_index, TServerID_t server_id, TProcessType_t process_type);
};

#define DGameManager singleton<game_manager>::get_instance()

#endif // !_GAME_MANAGER_H_
