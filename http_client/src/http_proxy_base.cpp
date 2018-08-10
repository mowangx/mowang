
#include "http_proxy_base.h"

#include "log.h"
#include "string_common.h"

http_proxy_base::http_proxy_base(boost::asio::io_service& io_service, int port) :
	m_resolver(io_service),
	m_port(port)
{

}

http_proxy_base::~http_proxy_base()
{

}

void http_proxy_base::start_request(const dynamic_string& host, const dynamic_string& url, const dynamic_string& body)
{
	m_host = host;
	m_url = url;
	m_body = body;
	bind_connect();
}

void http_proxy_base::handle_connect(const boost::system::error_code& error)
{
	if (!error) {
		bind_handshake();
	}
	else {
		log_error("connect failed for %s", error.message().c_str());
	}
}

void http_proxy_base::handle_handshake(const boost::system::error_code& error)
{
	if (!error) {
		write_request();
	}
	else {
		log_error("handshake failed for %s", error.message().c_str());
	}
}

void http_proxy_base::handle_write(const boost::system::error_code& error)
{
	if (!error) {
		bind_read_header();
	}
	else {
		log_error("write failed for %s", error.message().c_str());
	}
}

void http_proxy_base::handle_read_headers(const boost::system::error_code& error)
{
	if (!error) {
		// process the response headers.
		boost::asio::streambuf::const_buffers_type bufs = m_response.data();

		std::string header(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + m_response.size());

		log_info("header is %s", header.c_str());

		// start reading remaining data until EOF.
		bind_read_body();
	}
	else {
		log_error("read headers failed for %s", error.message().c_str());
	}
}

void http_proxy_base::handle_read_body(const boost::system::error_code& error)
{
	if (!error) {
		boost::asio::streambuf::const_buffers_type bufs = m_response.data();

		std::string body(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + m_response.size());

		log_info("body is %s", body.c_str());

		// continue reading remaining data until EOF.
		bind_read_body();
	}
	else if (error != boost::asio::error::eof) {
		log_error("read content failed for %s", error.message().c_str());
	}
}

void http_proxy_base::write_request()
{
	std::ostream request_stream(&m_request);
	std::string s1 = gx_to_string("POST %s HTTP/1.1\r\n", m_url.data());
	request_stream << s1;
	std::string s2 = gx_to_string("Host: %s:%d\r\n", m_host.data(), m_port);
	request_stream << s2;
	request_stream << "Accept-Encoding: *\r\n";
	request_stream << "Content-Length: " << m_body.size() << "\r\n";
	request_stream << "Content-Type: application/x-www-form-urlencoded\r\n";
	request_stream << "Connection: close\r\n\r\n";
	request_stream << m_body.data();
	bind_write();
}
