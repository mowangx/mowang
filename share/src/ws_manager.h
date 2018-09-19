
#ifndef _WS_MANAGER_H_
#define _WS_MANAGER_H_

#include <mutex>
#include <map>
#include <unordered_map>

#include "singleton.h"
#include "socket_manager.hpp"
#include "ws_wrapper.hpp"
#include "game_enum.h"

class web_socket_manager : public socket_manager<web_socket_wrapper_base, ws_packet_recv_info>, public singleton<web_socket_manager>
{
	typedef socket_manager<web_socket_wrapper_base, ws_packet_recv_info> TBaseType_t;
public:
	web_socket_manager();
	~web_socket_manager();

public:
	virtual bool init(TProcessType_t process_type, TProcessID_t process_id) override;
	virtual void update(uint32 diff) override;
private:
	void init_client();
	void init_server();

public:
	void start_listen(TPort_t port);
	void start_connect(const std::string& hostname, TPort_t port);
private:
	context_ptr on_tls_init_server(tls_mode mode, websocketpp::connection_hdl hdl);
	context_ptr on_tls_init_client(websocketpp::connection_hdl);
	bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);
	bool verify_common_name(X509 * cert);
	bool verify_subject_alternative_name(X509 * cert);
	std::string get_password() const;

private:
	void on_accept(websocketpp::connection_hdl hdl);
	void on_close(websocketpp::connection_hdl hdl);
	void on_client_open(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl);
	void on_client_fail(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl);
	void on_client_close(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl);

	virtual void unpack_packets(std::vector<ws_packet_recv_info*>& packets, web_socket_wrapper_base* socket) override;

	virtual void del_socket(web_socket_wrapper_base* socket) override;
	virtual void on_release_packets(std::vector<ws_packet_recv_info*>& packets) override;

private:
	std::string m_hostname;
	web_socket_client m_client;
	web_socket_server m_server;
	packet_buffer_info* m_buffer_info;
};

#define DWSNetMgr web_socket_manager::get_instance()

#endif // !_WS_MANAGER_H_

