
#include "http_proxy.h"

#include "string_common.h"

http_proxy::http_proxy(boost::asio::io_service& io_service) :
	http_proxy_base(io_service, 9010),
	m_socket(io_service)
{

}

void http_proxy::handle_resolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
{
	if (!error) {
		// Attempt a connection to each endpoint in the list until we
		// successfully establish a connection.
		boost::asio::async_connect(
			m_socket, 
			endpoint_iterator,
			boost::bind(&http_proxy::handle_connect, this, boost::asio::placeholders::error)
		);
	}
	else {
		log_error("handle resolve failed for %s", error.message().c_str());
	}
}

void http_proxy::bind_connect()
{
	boost::asio::ip::tcp::resolver::query query(m_host.data(), gx_to_string("%d", m_port));
	m_resolver.async_resolve(
		query,
		boost::bind(&http_proxy::handle_resolve, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator)
	);
}

void http_proxy::bind_handshake()
{
	write_request();
}

void http_proxy::bind_write()
{
	boost::asio::async_write(
		m_socket,
		m_request,
		boost::bind(&http_proxy::handle_write, this, boost::asio::placeholders::error)
	);
}

void http_proxy::bind_read_status_line()
{
	boost::asio::async_read_until(
		m_socket, 
		m_response, 
		"\r\n",
		boost::bind(&http_proxy::handle_read_status_line, this, boost::asio::placeholders::error)
	);
}

void http_proxy::bind_read_header()
{
	boost::asio::async_read_until(
		m_socket,
		m_response,
		"\r\n\r\n",
		boost::bind(&http_proxy::handle_read_headers, this, boost::asio::placeholders::error)
	);
}

void http_proxy::bind_read_body()
{
	boost::asio::async_read(
		m_socket,
		m_response,
		boost::asio::transfer_at_least(1),
		boost::bind(&http_proxy::handle_read_body, this, boost::asio::placeholders::error)
	);
}
