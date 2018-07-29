
#ifndef _WS_WRAPPER_H_
#define _WS_WRAPPER_H_

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "base_util.h"
#include "socket_util.h"
#include "log.h"
#include "socket_handler.h"

typedef websocketpp::client<websocketpp::config::asio_client> web_socket_client;
typedef websocketpp::server<websocketpp::config::asio> web_socket_server;

struct packet_buffer_info;

class web_socket_wrapper_base
{
public:
	web_socket_wrapper_base(TSocketIndex_t socket_index, websocketpp::connection_hdl hdl);

public:
	void set_active(bool active_flag);
	bool is_active() const;

	websocketpp::connection_hdl get_hdl() const;
	TSocketIndex_t get_socket_index() const;

	int get_input_len() const;

	socket_handler* get_socket_handler();
	void set_socket_handler(socket_handler* handler);

public:
	bool read(packet_buffer_info* buffer_info);

public:
	virtual void write(char* msg, int len) = 0;
	virtual void flush() = 0;
	virtual void close() = 0;

protected:
	TSocketIndex_t m_index;
	socket_handler* m_input_handler;
	websocketpp::connection_hdl m_hdl;
	bool m_status;
	std::string m_error_reason;
};

template <class T>
class web_socket_wrapper : public web_socket_wrapper_base
{
public:
	web_socket_wrapper(TSocketIndex_t socket_index, T* endpoint, websocketpp::connection_hdl hdl);

public:
	virtual void write(char* msg, int len) override;
	virtual void flush() override;
	virtual void close() override;

public:
	void on_message(websocketpp::connection_hdl, typename T::message_ptr msg);

private:
	T* m_endpoint;
};

class web_socket_client_wrapper : public web_socket_wrapper<websocketpp::client<websocketpp::config::asio_client>>
{
public:
	web_socket_client_wrapper(TSocketIndex_t socket_index, web_socket_client* client, websocketpp::connection_hdl hdl, std::string uri);

private:
	std::string m_uri;
	std::string m_server;
};

class web_socket_server_wrapper : public web_socket_wrapper<websocketpp::server<websocketpp::config::asio>>
{
public:
	web_socket_server_wrapper(TSocketIndex_t socket_index, web_socket_server* server, websocketpp::connection_hdl hdl);

};

#endif // !_WS_WRAPPER_H_