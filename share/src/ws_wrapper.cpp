
#include "ws_wrapper.hpp"

web_socket_wrapper_base::web_socket_wrapper_base(TSocketIndex_t socket_index, websocketpp::connection_hdl hdl) : 
	m_index(socket_index),
	m_hdl(hdl),
	m_status("Connecting")
{
}

void web_socket_wrapper_base::set_active(bool active_flag)
{
	m_status = active_flag;
}

bool web_socket_wrapper_base::is_active() const
{
	return m_status;
}

websocketpp::connection_hdl web_socket_wrapper_base::get_hdl() const
{
	return m_hdl;
}

TSocketIndex_t web_socket_wrapper_base::get_socket_index() const
{
	return m_index;
}

int web_socket_wrapper_base::get_input_len() const
{
	return m_input_handler->get_unpack_len();
}

socket_handler * web_socket_wrapper_base::get_socket_handler()
{
	return m_input_handler;
}

void web_socket_wrapper_base::set_socket_handler(socket_handler * handler)
{
	m_input_handler = handler;
}

bool web_socket_wrapper_base::read(packet_buffer_info* buffer_info)
{
	return m_input_handler->unpack_ws_packet(buffer_info);
}

web_socket_client_wrapper::web_socket_client_wrapper(TSocketIndex_t socket_index, web_socket_client* client, websocketpp::connection_hdl hdl, std::string uri) :
	web_socket_wrapper(socket_index, client, hdl),
	m_uri(uri)
{
}

web_socket_server_wrapper::web_socket_server_wrapper(TSocketIndex_t socket_index, web_socket_server* server, websocketpp::connection_hdl hdl) :
	web_socket_wrapper(socket_index, server, hdl)
{
}
