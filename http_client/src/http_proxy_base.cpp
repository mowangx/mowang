
#include "http_proxy_base.h"

#include "log.h"
#include "string_common.h"

http_proxy_base::http_proxy_base(boost::asio::io_service& io_service, bool usessl) :
	m_resolver(io_service),
	m_usessl(usessl)
{
	m_port = m_usessl ? 443 : 80;
}

http_proxy_base::~http_proxy_base()
{

}

void http_proxy_base::start_request(const dynamic_string& host, const dynamic_string& url, const dynamic_string& body, std::function<void(int, const dynamic_string&)> callback)
{
	m_host = host;
	m_url = url;
	m_body = body;
	m_callback = callback;
	bind_connect();
}

bool http_proxy_base::usessl() const
{
	return m_usessl;
}

void http_proxy_base::handle_connect(const boost::system::error_code& error)
{
	if (!error) {
		bind_handshake();
	}
	else {
		m_callback(-1, "connect failed");
		log_error("connect failed for %s", error.message().c_str());
	}
}

void http_proxy_base::handle_handshake(const boost::system::error_code& error)
{
	if (!error) {
		write_request();
	}
	else {
		m_callback(-1, "handshake failed");
		log_error("handshake failed for %s", error.message().c_str());
	}
}

void http_proxy_base::handle_write(const boost::system::error_code& error)
{
	if (!error) {
		bind_read_status_line();
	}
	else {
		m_callback(-1, "write failed");
		log_error("write failed for %s", error.message().c_str());
	}
}

void http_proxy_base::handle_read_status_line(const boost::system::error_code& error)
{
	if (!error) {
		// Check that response is OK.
		std::istream response_stream(&m_response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
			m_callback(-1, "invalid response");
			return;
		}
		if (status_code != 200) {
			m_callback(status_code, "");
			return;
		}

		// Read the response headers, which are terminated by a blank line.
		bind_read_header();
	}
	else {
		m_callback(-1, "read status line failed");
		log_error("read status line failed for %s", error.message().c_str());
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
		m_callback(-1, "read headers failed");
		log_error("read headers failed for %s", error.message().c_str());
	}
}

void http_proxy_base::handle_read_body(const boost::system::error_code& error)
{
	if (!error) {
		// continue reading remaining data until EOF.
		bind_read_body();
	}
	else if (error != boost::asio::error::eof) {
		m_callback(-1, "read content failed");
		log_error("read content failed for %s", error.message().c_str());
	}
	else {
		boost::asio::streambuf::const_buffers_type bufs = m_response.data();

		std::string body(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + m_response.size());
		uint16 offset = body.find("\r\n\r\n");
		uint16 len = (uint16)body.size();
		if (offset > 0) {
			len = len - offset - strlen("\r\n\r\n");
		}
		else {
			offset = 0;
		}

		m_result.append(body.c_str(), len, offset);
		m_callback(200, m_result);
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
