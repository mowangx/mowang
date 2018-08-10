
#include "stdio.h"
#include <iostream>

#include "http_client.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "argv is less than 2" << std::endl;
		return false;
	}

	std::cout << "start http client" << argv[1] << std::endl;

	DHttpClient.start("http_client", argv[1]);

	return 0;
}






















//
//
//
//class client
//{
//public:
//	client(boost::asio::io_service& io_service,
//		boost::asio::ssl::context& context,
//		boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
//		: socket_(io_service, context)
//	{
//		socket_.set_verify_mode(boost::asio::ssl::context::verify_none);
//		//socket_.set_verify_mode(boost::asio::ssl::verify_peer);
//		socket_.set_verify_callback(
//			boost::bind(&client::verify_certificate, this, _1, _2));
//
//		boost::asio::async_connect(socket_.lowest_layer(), endpoint_iterator,
//			boost::bind(&client::handle_connect, this,
//				boost::asio::placeholders::error));
//	}
//
//	bool verify_certificate(bool preverified,
//		boost::asio::ssl::verify_context& ctx)
//	{
//		// The verify callback can be used to check whether the certificate that is
//		// being presented is valid for the peer. For example, RFC 2818 describes
//		// the steps involved in doing this for HTTPS. Consult the OpenSSL
//		// documentation for more details. Note that the callback is called once
//		// for each certificate in the certificate chain, starting from the root
//		// certificate authority.
//
//		// In this example we will simply print the certificate's subject name.
//		char subject_name[256];
//		X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
//		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
//		std::cout << "Verifying " << subject_name << "\n";
//
//		return preverified;
//	}
//
//	void handle_connect(const boost::system::error_code& error)
//	{
//		if (!error)
//		{
//			socket_.async_handshake(boost::asio::ssl::stream_base::client,
//				boost::bind(&client::handle_handshake, this,
//					boost::asio::placeholders::error));
//		}
//		else
//		{
//			std::cout << "Connect failed: " << error.message() << "\n";
//		}
//	}
//
//	void handle_handshake(const boost::system::error_code& error)
//	{
//		if (!error)
//		{
//			request_ << "GET /LICENSE_1_0.txt HTTP/1.1\r\n";
//			request_ << "Host: www.boost.org\r\n";
//			request_ << "Accept-Encoding: *\r\n";
//			request_ << "\r\n";
//
//			boost::asio::async_write(socket_,
//				boost::asio::buffer(request_.str()),
//				boost::bind(&client::handle_write, this,
//					boost::asio::placeholders::error,
//					boost::asio::placeholders::bytes_transferred));
//		}
//		else
//		{
//			std::cout << "Handshake failed: " << error.message() << "\n";
//		}
//	}
//
//	void handle_write(const boost::system::error_code& error,
//		size_t bytes_transferred)
//	{
//		if (!error)
//		{
//			boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
//				boost::bind(&client::handle_read_headers, this,
//					boost::asio::placeholders::error));
//		}
//		else
//		{
//			std::cout << "Write failed: " << error.message() << "\n";
//		}
//	}
//
//	void handle_read_headers(const boost::system::error_code& err)
//	{
//		if (!err)
//		{
//			// Process the response headers.
//			std::istream response_stream(&response_);
//			std::string header;
//			while (std::getline(response_stream, header) && header != "\r")
//				std::cout << header << "\n";
//			std::cout << "\n";
//
//			// Write whatever content we already have to output.
//			if (response_.size() > 0)
//				std::cout << &response_;
//
//			// Start reading remaining data until EOF.
//			boost::asio::async_read(socket_, response_,
//				boost::asio::transfer_at_least(1),
//				boost::bind(&client::handle_read_content, this,
//					boost::asio::placeholders::error));
//		}
//		else
//		{
//			std::cout << "Error: " << err << "\n";
//		}
//	}
//
//	void handle_read_content(const boost::system::error_code& err)
//	{
//		if (!err)
//		{
//			// Write all of the data that has been read so far.
//			//std::cout << &response_;
//			boost::asio::streambuf::const_buffers_type bufs = response_.data();
//			std::string str(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + response_.size());
//			std::cout << str.c_str();
//
//			// Continue reading remaining data until EOF.
//			boost::asio::async_read(socket_, response_,
//				boost::asio::transfer_at_least(1),
//				boost::bind(&client::handle_read_content, this,
//					boost::asio::placeholders::error));
//		}
//		else if (err != boost::asio::error::eof)
//		{
//			std::cout << "Error: " << err << "\n";
//		}
//	}
//
//private:
//	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
//	std::stringstream request_;
//	boost::asio::streambuf response_;
//};
//
//int main(int argc, char* argv[])
//{
//	try
//	{
//		if (argc != 3)
//		{
//			std::cerr << "Usage: client <host> <port>\n";
//			return 1;
//		}
//
//		boost::asio::io_service io_service;
//
//		boost::asio::ip::tcp::resolver resolver(io_service);
//		boost::asio::ip::tcp::resolver::query query(argv[1], argv[2]);
//		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
//
//		boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
//		ctx.set_default_verify_paths();
//
//		client c(io_service, ctx, iterator);
//
//		io_service.run();
//	}
//	catch (std::exception& e)
//	{
//		std::cerr << "Exception: " << e.what() << "\n";
//	}
//
//	return 0;
//}














//#include <iostream>
//#include <istream>
//#include <ostream>
//#include <string>
//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//
//using boost::asio::ip::tcp;
//
//class client
//{
//public:
//	client(boost::asio::io_service& io_service,
//		const std::string& server, const std::string& path)
//		: resolver_(io_service),
//		socket_(io_service)
//	{
//		// Form the request. We specify the "Connection: close" header so that the
//		// server will close the socket after transmitting the response. This will
//		// allow us to treat all data up until the EOF as the content.
//		std::ostream request_stream(&request_);
//		request_stream << "GET " << path << " HTTP/1.0\r\n";
//		request_stream << "Host: " << "localhost:9010" << "\r\n";
//		request_stream << "Accept: */*\r\n";
//		request_stream << "Connection: close\r\n\r\n";
//
//		// Start an asynchronous resolve to translate the server and service names
//		// into a list of endpoints.
//		tcp::resolver::query query(server, "9010");
//		resolver_.async_resolve(query,
//			boost::bind(&client::handle_resolve, this,
//				boost::asio::placeholders::error,
//				boost::asio::placeholders::iterator));
//	}
//
//private:
//	void handle_resolve(const boost::system::error_code& err,
//		tcp::resolver::iterator endpoint_iterator)
//	{
//		if (!err)
//		{
//			// Attempt a connection to each endpoint in the list until we
//			// successfully establish a connection.
//			boost::asio::async_connect(socket_, endpoint_iterator,
//				boost::bind(&client::handle_connect, this,
//					boost::asio::placeholders::error));
//		}
//		else
//		{
//			std::cout << "Error: " << err.message() << "\n";
//		}
//	}
//
//	void handle_connect(const boost::system::error_code& err)
//	{
//		if (!err)
//		{
//			// The connection was successful. Send the request.
//			boost::asio::async_write(socket_,
//				//boost::asio::buffer(request_.str()),
//				request_,
//				boost::bind(&client::handle_write_request, this,
//					boost::asio::placeholders::error));
//		}
//		else
//		{
//			std::cout << "Error: " << err.message() << "\n";
//		}
//	}
//
//	void handle_write_request(const boost::system::error_code& err)
//	{
//		if (!err)
//		{
//			// Read the response status line. The response_ streambuf will
//			// automatically grow to accommodate the entire line. The growth may be
//			// limited by passing a maximum size to the streambuf constructor.
//			boost::asio::async_read_until(socket_, response_, "\r\n",
//				boost::bind(&client::handle_read_status_line, this,
//					boost::asio::placeholders::error));
//		}
//		else
//		{
//			std::cout << "Error: " << err.message() << "\n";
//		}
//	}
//
//	void handle_read_status_line(const boost::system::error_code& err)
//	{
//		if (!err)
//		{
//			// Check that response is OK.
//			std::istream response_stream(&response_);
//			std::string http_version;
//			response_stream >> http_version;
//			unsigned int status_code;
//			response_stream >> status_code;
//			std::string status_message;
//			std::getline(response_stream, status_message);
//			if (!response_stream || http_version.substr(0, 5) != "HTTP/")
//			{
//				std::cout << "Invalid response\n";
//				return;
//			}
//			if (status_code != 200)
//			{
//				std::cout << "Response returned with status code ";
//				std::cout << status_code << "\n";
//				return;
//			}
//
//			// Read the response headers, which are terminated by a blank line.
//			boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
//				boost::bind(&client::handle_read_headers, this,
//					boost::asio::placeholders::error));
//		}
//		else
//		{
//			std::cout << "Error: " << err << "\n";
//		}
//	}
//
//	void handle_read_headers(const boost::system::error_code& err)
//	{
//		if (!err)
//		{
//			// Process the response headers.
//			std::istream response_stream(&response_);
//			std::string header;
//			while (std::getline(response_stream, header) && header != "\r")
//				std::cout << header << "\n";
//			std::cout << "\n";
//
//			// Write whatever content we already have to output.
//			if (response_.size() > 0)
//				std::cout << &response_;
//
//			// Start reading remaining data until EOF.
//			boost::asio::async_read(socket_, response_,
//				boost::asio::transfer_at_least(1),
//				boost::bind(&client::handle_read_content, this,
//					boost::asio::placeholders::error));
//		}
//		else
//		{
//			std::cout << "Error: " << err << "\n";
//		}
//	}
//
//	void handle_read_content(const boost::system::error_code& err)
//	{
//		if (!err)
//		{
//			// Write all of the data that has been read so far.
//			std::cout << &response_;
//
//			// Continue reading remaining data until EOF.
//			boost::asio::async_read(socket_, response_,
//				boost::asio::transfer_at_least(1),
//				boost::bind(&client::handle_read_content, this,
//					boost::asio::placeholders::error));
//		}
//		else if (err != boost::asio::error::eof)
//		{
//			std::cout << "Error: " << err << "\n";
//		}
//	}
//
//	tcp::resolver resolver_;
//	tcp::socket socket_;
//	//std::stringstream request_;
//	boost::asio::streambuf request_;
//	boost::asio::streambuf response_;
//};
//
//int main(int argc, char* argv[])
//{
//	try
//	{
//		boost::asio::io_service io_service;
//		client c(io_service, "localhost", "sentry/process_trace");
//		io_service.run();
//	}
//	catch (std::exception& e)
//	{
//		std::cout << "Exception: " << e.what() << "\n";
//	}
//
//	return 0;
//}