
#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include "singleton.h"
#include "service.h"


class db_server : public service, public singleton<db_server>
{
	typedef service TBaseType_t;

public:
	db_server();
	virtual ~db_server() override;

public:
	virtual bool init(TProcessID_t process_id) override;
private:
	virtual void work_run() override;
	virtual void do_loop(TGameTime_t diff) override;
public:
	virtual bool connect_server(const char* ip, TPort_t port) override;
};

#define DDbServer singleton<db_server>::get_instance()


#endif // !_DB_SERVER_H_

