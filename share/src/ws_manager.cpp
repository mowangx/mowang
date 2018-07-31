
#include "ws_manager.h"
#include "auto_lock.h"

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
}

void web_socket_manager::init_server()
{
	m_server.set_access_channels(websocketpp::log::alevel::all);
	m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

	m_server.init_asio();

	m_server.set_open_handler(std::bind(&web_socket_manager::on_accept, this, std::placeholders::_1));
	m_server.set_close_handler(std::bind(&web_socket_manager::on_close, this, std::placeholders::_1));
	//m_server.set_message_handler(std::bind(&WebSocketManager::on_message, this, std::placeholders::_1, std::placeholders::_2));
	//return websocketpp::lib::make_shared<websocketpp::lib::thread>(&web_socket_server::run, &m_server);
}

void web_socket_manager::start_listen(TPort_t port)
{
	m_server.listen(port);
	m_server.start_accept();
}

void web_socket_manager::start_connect(const std::string& uri)
{
	websocketpp::lib::error_code err;

	web_socket_client::connection_ptr con = m_client.get_connection(uri, err);

	if (err) {
		std::cout << "Connect initialization error: " << err.message() << std::endl;
		return ;
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
	log_info("on client open! sockete index %" I64_FMT "u", socket->get_socket_index());
}

void web_socket_manager::on_client_fail(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl)
{
	log_info("on client fail! sockete index %" I64_FMT "u", socket->get_socket_index());
}

void web_socket_manager::on_client_close(web_socket_wrapper_base* socket, websocketpp::connection_hdl hdl)
{
	socket->set_active(false);
	log_info("on client close! sockete index %" I64_FMT "u", socket->get_socket_index());
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
