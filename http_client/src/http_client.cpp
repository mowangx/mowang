
#include "http_client.h"

#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "tcp_manager.h"
#include "game_server_handler.h"
#include "game_manager_handler.h"

http_client::http_client() : service(PROCESS_HTTP_CLIENT)
{
	m_wait_release_proxy.clear();
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

	DRegisterServerRpc(this, http_client, register_server, 2);
	DRegisterServerRpc(this, http_client, http_request, 6);

	if (!DNetMgr.start_listen<game_server_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	log_info("init socket manager success");

	return true;
}

void http_client::work_run()
{
	connect_game_manager_loop(m_config.get_game_manager_listen_ip(), m_config.get_game_manager_listen_port());
	TBaseType_t::work_run();
}

void http_client::do_loop(TGameTime_t diff)
{
	for (auto client : m_wait_release_proxy) {
		if (client->usessl()) {
			m_https_pools.deallocate((https_proxy*)client);
		}
		else {
			m_http_pools.deallocate((http_proxy*)client);
		}
	}
	m_wait_release_proxy.clear();

	m_io_service.poll_one();
	
	TBaseType_t::do_loop(diff);
}

bool http_client::connect_game_manager(const char * ip, TPort_t port)
{
	return DNetMgr.start_connect<game_manager_handler>(ip, port);
}

void http_client::http_request(TSocketIndex_t socket_index, TOptID_t opt_id, const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl)
{
	try {
		http_proxy_base* c = nullptr;
		if (usessl) {
			boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
			ctx.set_default_verify_paths();

			c = m_https_pools.allocate(m_io_service, ctx);
		}
		else {
			c = m_http_pools.allocate(m_io_service);
		}
		c->start_request(host, url, params, [=](int status, const dynamic_string& result) {
			m_wait_release_proxy.push_back(c);
			game_process_info process_info;
			if (!DRpcWrapper.get_server_simple_info_by_socket_index(process_info, socket_index)) {
				return;
			}
			rpc_client* rpc = DRpcWrapper.get_client(process_info);
			if (NULL == rpc) {
				return;
			}
			rpc->call_remote_func("on_http_response", opt_id, status, result);
		});
	}
	catch (std::exception& e) {
		log_error("http request failed for %s", e.what());
	}
}
