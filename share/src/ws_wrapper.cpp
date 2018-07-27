
#include "ws_wrapper.hpp"

web_socket_wrapper_base::web_socket_wrapper_base(TSocketIndex_t socket_index, websocketpp::connection_hdl hdl) : 
	m_index(socket_index),
	m_output_index(0),
	m_hdl(hdl),
	m_status("Connecting")
{
}

bool web_socket_wrapper_base::is_active() const
{
	return m_status == "Open";
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

bool web_socket_wrapper_base::read(ws_buffer_info* buffer_info)
{
	return m_input_handler->unpack_ws_packet(buffer_info);
}

void web_socket_wrapper_base::write(char* msg, int len)
{
	if ((m_output_index + len) > MAX_WEB_SOCKET_OUTPUT_BUFFER_SIZE) {
		log_error("websocket send buffer size is reach max! cur len %d, send len %d, socket index %" I64_FMT "u", m_output_index, len, m_index);
		return;
	}
	memcpy((char*)(m_output_buffer + m_output_index), msg, len);
	m_output_index += len;
}

web_socket_client_wrapper::web_socket_client_wrapper(TSocketIndex_t socket_index,  websocketpp::connection_hdl hdl, std::string uri) : 
	web_socket_wrapper(socket_index, hdl),
	m_uri(uri)
{
}

web_socket_server_wrapper::web_socket_server_wrapper(TSocketIndex_t socket_index,  websocketpp::connection_hdl hdl) : 
	web_socket_wrapper(socket_index, hdl)
{
}
