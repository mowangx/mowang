
#ifndef _HTTP_PROXY_BASE_H_
#define _HTTP_PROXY_BASE_H_

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "dynamic_array.h"

class http_proxy_base
{
public:
	http_proxy_base(boost::asio::io_service & io_service, int port);
	virtual ~http_proxy_base();

public:
	void start_request(const dynamic_string& host, const dynamic_string& url, const dynamic_string& body);

protected:
	void handle_connect(const boost::system::error_code& error);
	void handle_handshake(const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error);
	void handle_read_headers(const boost::system::error_code& err);
	void handle_read_body(const boost::system::error_code& err);
protected:
	void write_request();
protected:
	virtual void bind_connect() = 0;
	virtual void bind_handshake() = 0;
	virtual void bind_write() = 0;
	virtual void bind_read_header() = 0;
	virtual void bind_read_body() = 0;

protected:
	boost::asio::ip::tcp::resolver m_resolver;
	boost::asio::streambuf m_request;
	boost::asio::streambuf m_response;
	int m_port;
	dynamic_string m_host;
	dynamic_string m_url;
	dynamic_string m_body;
};

#endif // !_HTTP_PROXY_BASE_H_

