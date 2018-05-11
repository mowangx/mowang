
#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include <unordered_map>

#include "singleton.h"
#include "service.h"
#include "dynamic_array.h"


class db_server : public service, public singleton<db_server>
{
	typedef service TBaseType_t;

public:
	db_server();
	~db_server();

public:
	bool init(TProcessID_t process_id);
private:
	virtual void do_loop(TGameTime_t diff) override;
	virtual bool connect_game_manager(const char* ip, TPort_t port) override;

private:
};

#define DDbServer singleton<db_server>::get_instance()


#endif // !_DB_SERVER_H_

