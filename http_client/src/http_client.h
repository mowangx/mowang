
#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "singleton.h"
#include "service.h"
#include "http_proxy_base.h"

class http_client : public service, public singleton<http_client>
{
	typedef service TBaseType_t;

public:
	http_client();
	virtual ~http_client() override;

public:
	virtual bool init(TProcessID_t process_id) override;
private:
	virtual void work_run() override;
	virtual void do_loop(TGameTime_t diff) override;
	virtual bool connect_game_manager(const char* ip, TPort_t port) override;

public:
	void http_request(TSocketIndex_t socket_index, TOptID_t opt_id, const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl);

private:
	boost::asio::io_service m_io_service;
};

#define DHttpClient singleton<http_client>::get_instance()

#endif // !_HTTP_CLIENT_H_

