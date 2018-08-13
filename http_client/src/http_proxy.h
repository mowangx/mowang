
#ifndef _HTTP_PROXY_H_
#define _HTTP_PROXY_H_

#include "http_proxy_base.h"

class http_proxy : public http_proxy_base
{
	typedef http_proxy_base TBaseType_t;
public:
	http_proxy(boost::asio::io_service& io_service);

private:
	void handle_resolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

private:
	virtual void bind_connect() override;
	virtual void bind_handshake() override;
	virtual void bind_write() override;
	virtual void bind_read_status_line() override;
	virtual void bind_read_header() override;
	virtual void bind_read_body() override;

private:
	boost::asio::ip::tcp::socket m_socket;
};

#endif // !_HTTP_PROXY_H_

