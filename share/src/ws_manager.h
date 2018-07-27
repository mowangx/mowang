
#ifndef _WS_MANAGER_H_
#define _WS_MANAGER_H_

#include <mutex>
#include <map>
#include <unordered_map>

#include "singleton.h"
#include "socket_manager.hpp"
#include "ws_wrapper.hpp"

typedef websocketpp::client<websocketpp::config::asio_client> WebSocketClient;
typedef websocketpp::server<websocketpp::config::asio> WebSocketServer;

class WebSocketManager : public socket_manager<web_socket_wrapper_base, ws_packet_recv_info>, public singleton<WebSocketManager>
{
	typedef socket_manager<web_socket_wrapper_base, ws_packet_recv_info> TBaseType_t;
public:
	WebSocketManager();
	~WebSocketManager();

public:
	virtual bool init(TProcessType_t process_type, TProcessID_t process_id) override;
	virtual void update(uint32 diff) override;

public:
	void start_listen(TPort_t port);
	void start_connect(const std::string & uri);

private:
	void on_accept(websocketpp::connection_hdl hdl);
	void on_close(websocketpp::connection_hdl hdl);

	virtual void unpack_packets(std::vector<ws_packet_recv_info*>& packets, web_socket_wrapper_base* socket) override;

	virtual void del_socket(web_socket_wrapper_base* socket) override;

private:
	WebSocketClient m_client;
	WebSocketServer m_server;
	ws_buffer_info* m_buffer_info;
};

#define DWSNetMgr WebSocketManager::get_instance()

#endif // !_WS_MANAGER_H_

