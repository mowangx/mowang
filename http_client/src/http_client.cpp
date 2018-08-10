
#include "http_client.h"

#include "rpc_proxy.h"
#include "tcp_manager.h"
#include "game_server_handler.h"
#include "game_manager_handler.h"
#include "http_proxy.h"
#include "https_proxy.h"

http_client::http_client() : service(PROCESS_HTTP_CLIENT)
{
}

http_client::~http_client()
{
}

bool http_client::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	game_server_handler::Setup();
	game_manager_handler::Setup();

	DRegisterServerRpc(this, http_client, http_request, 5);

	if (!DNetMgr.start_listen<game_server_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	log_info("init socket manager success");

	//http_request(1, "www.boost.org", "/LICENSE_1_0.txt", "user_name=mowang", true);
	//http_request(1, "localhost", "/sentry/process_trace", "user_name=mowang", false);

	return true;
}

void http_client::work_run()
{
	connect_game_manager_loop(m_config.get_game_manager_listen_ip(), m_config.get_game_manager_listen_port());
	TBaseType_t::work_run();
}

void http_client::do_loop(TGameTime_t diff)
{
	m_io_service.poll_one();
	TBaseType_t::do_loop(diff);
}

bool http_client::connect_game_manager(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<game_manager_handler>(ip, port);
}

void http_client::http_request(TSocketIndex_t socket_index, const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl)
{
	try
	{
		if (usessl) {
			boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
			ctx.set_default_verify_paths();

			https_proxy* c = new https_proxy(m_io_service, ctx);
			c->start_request(host, url, params);
		}
		else {
			http_proxy* c = new http_proxy(m_io_service);
			c->start_request(host, url, params);
		}
		
	}
	catch (std::exception& e)
	{
		log_error("http request failed for %s", e.what());
	}
}
