
#include "ws_manager.h"
#include "auto_lock.h"
#include "string_common.h"

web_socket_manager::web_socket_manager()
{
	m_buffer_info = new packet_buffer_info();
}

web_socket_manager::~web_socket_manager()
{
	m_client.stop_perpetual();

	for (auto itr = m_sockets.begin(); itr != m_sockets.end(); ++itr) {
		itr->second->close();
	}

	if (nullptr != m_buffer_info) {
		delete m_buffer_info;
		m_buffer_info = nullptr;
	}
}

bool web_socket_manager::init(TProcessType_t process_type, TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_type, process_id)) {
		return false;
	}

	init_client();
	init_server();
	return true;
}

void web_socket_manager::update(uint32 diff)
{
	m_server.poll_one();
	m_client.poll_one();

	TBaseType_t::update(diff);
}

void web_socket_manager::init_client()
{
	m_client.clear_access_channels(websocketpp::log::alevel::all);
	m_client.clear_error_channels(websocketpp::log::elevel::all);

	m_client.init_asio();
	m_client.start_perpetual();
#ifdef OPEN_SSL
	m_client.set_tls_init_handler(std::bind(&web_socket_manager::on_tls_init_client, this, std::placeholders::_1));
#endif // OPEN_SSL
}

void web_socket_manager::init_server()
{
	m_server.set_access_channels(websocketpp::log::alevel::all);
	m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

	m_server.init_asio();

	m_server.set_open_handler(std::bind(&web_socket_manager::on_accept, this, std::placeholders::_1));
	m_server.set_close_handler(std::bind(&web_socket_manager::on_close, this, std::placeholders::_1));
#ifdef OPEN_SSL
	m_server.set_tls_init_handler(std::bind(&web_socket_manager::on_tls_init_server, this, MOZILLA_INTERMEDIATE, std::placeholders::_1));
#endif // OPEN_SSL
	//m_server.set_message_handler(std::bind(&WebSocketManager::on_message, this, std::placeholders::_1, std::placeholders::_2));
	//return websocketpp::lib::make_shared<websocketpp::lib::thread>(&web_socket_server::run, &m_server);
}

void web_socket_manager::start_listen(TPort_t port)
{
	m_server.listen(port);
	m_server.start_accept();
}

void web_socket_manager::start_connect(const std::string& hostname, TPort_t port)
{
	try {
		m_hostname = hostname;
		websocketpp::lib::error_code err;
#ifdef OPEN_SSL
		std::string protocol = "wss://";
#else
		std::string protocol = "ws://";
#endif // OPEN_SSL

		std::string uri = protocol + m_hostname + ":" + gx_to_string("%u", port);
		web_socket_client::connection_ptr con = m_client.get_connection(uri, err);

		if (err) {
			std::cout << "Connect initialization error: " << err.message() << std::endl;
			return;
		}

		//std::shared_ptr<web_socket_client_wrapper> socket = std::make_shared<web_socket_client_wrapper>(gen_socket_index(), nullptr, nullptr, con->get_handle(), uri);
		web_socket_client_wrapper* socket = new web_socket_client_wrapper(gen_socket_index(), &m_client, con->get_handle(), uri);
		con->set_open_handler(websocketpp::lib::bind(
			&web_socket_manager::on_client_open,
			this,
			socket,
			websocketpp::lib::placeholders::_1
		));
		con->set_fail_handler(websocketpp::lib::bind(
			&web_socket_manager::on_client_fail,
			this,
			socket,
			websocketpp::lib::placeholders::_1
		));
		con->set_close_handler(websocketpp::lib::bind(
			&web_socket_manager::on_client_close,
			this,
			socket,
			websocketpp::lib::placeholders::_1
		));
		con->set_message_handler(websocketpp::lib::bind(
			&web_socket_client_wrapper::on_message,
			socket,
			websocketpp::lib::placeholders::_1,
			websocketpp::lib::placeholders::_2
		));

		m_client.connect(con);
	}
	catch (websocketpp::exception const & e) {
		log_error("connect server failed for %s", e.what());
	}
}

context_ptr web_socket_manager::on_tls_init_server(tls_mode mode, websocketpp::connection_hdl hdl)
{
	std::cout << "on_tls_init called with hdl: " << hdl.lock().get() << std::endl;
	std::cout << "using TLS mode: " << (mode == MOZILLA_MODERN ? "Mozilla Modern" : "Mozilla Intermediate") << std::endl;

	context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

	try {
		if (mode == MOZILLA_MODERN) {
			// Modern disables TLSv1
			ctx->set_options(asio::ssl::context::default_workarounds |
				asio::ssl::context::no_sslv2 |
				asio::ssl::context::no_sslv3 |
				asio::ssl::context::no_tlsv1 |
				asio::ssl::context::single_dh_use);
		}
		else {
			ctx->set_options(asio::ssl::context::default_workarounds |
				asio::ssl::context::no_sslv2 |
				asio::ssl::context::no_sslv3 |
				asio::ssl::context::single_dh_use);
		}
		ctx->set_password_callback(std::bind(&web_socket_manager::get_password, this));
		
		ctx->load_verify_file("ca.crt");
		ctx->use_certificate_file("server.crt", asio::ssl::context::pem);
		ctx->use_private_key_file("server.key", asio::ssl::context::pem);
	}
	catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
	return ctx;
}

context_ptr web_socket_manager::on_tls_init_client(websocketpp::connection_hdl)
{
	context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

	try {
		ctx->set_options(boost::asio::ssl::context::default_workarounds |
			boost::asio::ssl::context::no_sslv2 |
			boost::asio::ssl::context::no_sslv3 |
			boost::asio::ssl::context::single_dh_use);


		ctx->set_verify_mode(boost::asio::ssl::verify_peer);
		ctx->set_verify_callback(std::bind(&web_socket_manager::verify_certificate, this, std::placeholders::_1, std::placeholders::_2));

		// Here we load the CA certificates of all CA's that this client trusts.
		ctx->load_verify_file("ca.crt");
		ctx->use_certificate_file("client.crt", asio::ssl::context::pem);
		ctx->use_private_key_file("client.key", asio::ssl::context::pem);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return ctx;
}

bool web_socket_manager::verify_certificate(bool preverified, boost::asio::ssl::verify_context & ctx)
{
	// The verify callback can be used to check whether the certificate that is
	// being presented is valid for the peer. For example, RFC 2818 describes
	// the steps involved in doing this for HTTPS. Consult the OpenSSL
	// documentation for more details. Note that the callback is called once
	// for each certificate in the certificate chain, starting from the root
	// certificate authority.

	// Retrieve the depth of the current cert in the chain. 0 indicates the
	// actual server cert, upon which we will perform extra validation
	// (specifically, ensuring that the hostname matches. For other certs we
	// will use the 'preverified' flag from Asio, which incorporates a number of
	// non-implementation specific OpenSSL checking, such as the formatting of
	// certs and the trusted status based on the CA certs we imported earlier.
	int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());

	// if we are on the final cert and everything else checks out, ensure that
	// the hostname is present on the list of SANs or the common name (CN).
	if (depth == 0 && preverified) {
		X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

		if (verify_subject_alternative_name(cert)) {
			return true;
		}
		else if (verify_common_name(cert)) {
			return true;
		}
		else {
			return false;
		}
	}

	return preverified;
}

bool web_socket_manager::verify_common_name(X509 * cert)
{
	// Find the position of the CN field in the Subject field of the certificate
	int common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name(cert), NID_commonName, -1);
	if (common_name_loc < 0) {
		return false;
	}

	// Extract the CN field
	X509_NAME_ENTRY * common_name_entry = X509_NAME_get_entry(X509_get_subject_name(cert), common_name_loc);
	if (common_name_entry == NULL) {
		return false;
	}

	// Convert the CN field to a C string
	ASN1_STRING * common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
	if (common_name_asn1 == NULL) {
		return false;
	}

	char * common_name_str = (char *)ASN1_STRING_data(common_name_asn1);

	// Make sure there isn't an embedded NUL character in the CN
	if (ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
		return false;
	}

	// Compare expected hostname with the CN
	return (strcasecmp(m_hostname.c_str(), common_name_str) == 0);
}

bool web_socket_manager::verify_subject_alternative_name(X509 * cert)
{
	STACK_OF(GENERAL_NAME) * san_names = NULL;

	san_names = (STACK_OF(GENERAL_NAME) *) X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);
	if (san_names == NULL) {
		return false;
	}

	int san_names_count = sk_GENERAL_NAME_num(san_names);

	bool result = false;

	for (int i = 0; i < san_names_count; i++) {
		const GENERAL_NAME * current_name = sk_GENERAL_NAME_value(san_names, i);

		if (current_name->type != GEN_DNS) {
			continue;
		}

		char * dns_name = (char *)ASN1_STRING_data(current_name->d.dNSName);

		// Make sure there isn't an embedded NUL character in the DNS name
		if (ASN1_STRING_length(current_name->d.dNSName) != strlen(dns_name)) {
			break;
		}
		// Compare expected hostname with the CN
		result = (strcasecmp(m_hostname.c_str(), dns_name) == 0);
	}
	sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);

	return result;
}

std::string web_socket_manager::get_password() const
{
	return "123456";
}

void web_socket_manager::on_accept(websocketpp::connection_hdl hdl)
{
	web_socket_server::connection_ptr con = m_server.get_con_from_hdl(hdl);
	web_socket_server_wrapper* socket = new web_socket_server_wrapper(gen_socket_index(), &m_server, hdl);
	con->set_message_handler(std::bind(&web_socket_server_wrapper::on_message, socket, std::placeholders::_1, std::placeholders::_2));
	m_new_sockets.push_back(socket);
	std::cout << "have client connected" << std::endl;
}

void web_socket_manager::on_close(websocketpp::connection_hdl hdl)
{
	std::cout << "have client disconnected" << std::endl;
}

void web_socket_manager::on_client_open(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl)
{
	socket->set_active(true);
	m_new_sockets.push_back(socket);
	log_info("on client open! socket index %" I64_FMT "u", socket->get_socket_index());
}

void web_socket_manager::on_client_fail(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl)
{
	log_info("on client fail! socket index %" I64_FMT "u", socket->get_socket_index());
}

void web_socket_manager::on_client_close(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl)
{
	socket->set_active(false);
	log_info("on client close! socket index %" I64_FMT "u", socket->get_socket_index());
}

void web_socket_manager::unpack_packets(std::vector<ws_packet_recv_info*>& packets, web_socket_wrapper_base* socket)
{
	while (socket->read(m_buffer_info)) {
		char* packet_pool = m_mem_pool.allocate(m_buffer_info->len);
		ws_packet_recv_info* packet_info = m_packet_info_pool.allocate();
		packet_info->socket = socket;
		packet_info->buffer_info.buffer = packet_pool;
		packet_info->buffer_info.len = m_buffer_info->len;
		memcpy(packet_pool, m_buffer_info->buffer, packet_info->buffer_info.len);
		packets.push_back(packet_info);
	}
}

void web_socket_manager::del_socket(web_socket_wrapper_base* socket)
{
	// ¹Ø±ÕÁ´½Ó
	socket->close();

	TBaseType_t::del_socket(socket);
}

void web_socket_manager::on_release_packets(std::vector<ws_packet_recv_info*>& packets)
{
	for (auto packet_info : packets) {
		m_mem_pool.deallocate(packet_info->buffer_info.buffer);
		m_packet_info_pool.deallocate(packet_info);
	}
}
