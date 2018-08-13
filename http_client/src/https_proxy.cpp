
#include "https_proxy.h"

#include "log.h"
#include "string_common.h"

https_proxy::https_proxy(boost::asio::io_service& io_service, boost::asio::ssl::context& context) :
	http_proxy_base(io_service, 443),
	m_socket(io_service, context)
{
	
}

bool https_proxy::verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
	// The verify callback can be used to check whether the certificate that is
	// being presented is valid for the peer. For example, RFC 2818 describes
	// the steps involved in doing this for HTTPS. Consult the OpenSSL
	// documentation for more details. Note that the callback is called once
	// for each certificate in the certificate chain, starting from the root
	// certificate authority.

	// In this example we will simply print the certificate's subject name.
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);

	return preverified;
}

void https_proxy::bind_connect()
{
	boost::asio::ip::tcp::resolver::query query(m_host.data(), gx_to_string("%d", m_port));
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(query);
	m_socket.set_verify_mode(boost::asio::ssl::context::verify_none);
	//m_socket.set_verify_mode(boost::asio::ssl::verify_peer);
	m_socket.set_verify_callback(boost::bind(&https_proxy::verify_certificate, this, _1, _2));
	boost::asio::async_connect(
		m_socket.lowest_layer(),
		endpoint_iterator,
		boost::bind(&https_proxy::handle_connect, this, boost::asio::placeholders::error)
	);
}

void https_proxy::bind_handshake()
{
	m_socket.async_handshake(
		boost::asio::ssl::stream_base::client,
		boost::bind(&https_proxy::handle_handshake, this, boost::asio::placeholders::error)
	);
}

void https_proxy::bind_write()
{
	boost::asio::async_write(
		m_socket,
		m_request,
		boost::bind(&https_proxy::handle_write, this, boost::asio::placeholders::error)
	);
}

void https_proxy::bind_read_status_line()
{
	boost::asio::async_read_until(
		m_socket,
		m_response,
		"\r\n",
		boost::bind(&https_proxy::handle_read_status_line, this, boost::asio::placeholders::error)
	);
}

void https_proxy::bind_read_header()
{
	boost::asio::async_read_until(
		m_socket,
		m_response,
		"\r\n\r\n",
		boost::bind(&https_proxy::handle_read_headers, this, boost::asio::placeholders::error)
	);
}

void https_proxy::bind_read_body()
{
	boost::asio::async_read(
		m_socket,
		m_response,
		boost::asio::transfer_at_least(1),
		boost::bind(&https_proxy::handle_read_body, this, boost::asio::placeholders::error)
	);
}