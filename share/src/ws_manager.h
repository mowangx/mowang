
#ifndef _WS_MANAGER_H_
#define _WS_MANAGER_H_

#include <mutex>
#include <map>
#include <unordered_map>

#include "singleton.h"
#include "socket_manager.hpp"
#include "ws_wrapper.hpp"

class web_socket_manager : public socket_manager<web_socket_wrapper_base, ws_packet_recv_info>, public singleton<web_socket_manager>
{
	typedef socket_manager<web_socket_wrapper_base, ws_packet_recv_info> TBaseType_t;
public:
	web_socket_manager();
	~web_socket_manager();

public:
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> init_client();
	void init_server();

public:
	virtual bool init(TProcessType_t process_type, TProcessID_t process_id) override;
	virtual void update(uint32 diff) override;
	void update_client(uint32 diff);

public:
	void start_listen(TPort_t port);
	void start_connect(const std::string & uri);

private:
	void on_accept(websocketpp::connection_hdl hdl);
	void on_close(websocketpp::connection_hdl hdl);

	void on_client_open(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl);
	void on_client_fail(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl);
	void on_client_close(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl);

	virtual void unpack_packets(std::vector<ws_packet_recv_info*>& packets, web_socket_wrapper_base* socket) override;

	virtual void on_send_packet(web_socket_wrapper_base* socket) override;
	virtual void del_socket(web_socket_wrapper_base* socket) override;

private:
	web_socket_client m_client;
	web_socket_server m_server;
	packet_buffer_info* m_buffer_info;
};

#define DWSNetMgr web_socket_manager::get_instance()

#endif // !_WS_MANAGER_H_

