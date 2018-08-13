
#ifndef _HTTPS_PROXY_H_
#define _HTTPS_PROXY_H_

#include "http_proxy_base.h"

class https_proxy : public http_proxy_base
{
	typedef http_proxy_base TBaseType_t;
public:
	https_proxy(boost::asio::io_service& io_service, boost::asio::ssl::context& context);

private:
	bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);
private:
	virtual void bind_connect() override;
	virtual void bind_handshake() override;
	virtual void bind_write() override;
	virtual void bind_read_status_line() override;
	virtual void bind_read_header() override;
	virtual void bind_read_body() override;

private:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_socket;
};

#endif // !_HTTPS_PROXY_H_

