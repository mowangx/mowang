
#ifndef _WS_WRAPPER_HPP_
#define _WS_WRAPPER_HPP_

#include "ws_wrapper.h"

template <class T>
web_socket_wrapper<T>::web_socket_wrapper(TSocketIndex_t socket_index, websocketpp::connection_hdl hdl) : 
	web_socket_wrapper_base(socket_index, hdl),
	m_endpoint(NULL)
{

}

template <class T>
void web_socket_wrapper<T>::flush()
{
	int cur_index = 0;
	while (m_output_index > 0) {
		int left_len = m_output_index - cur_index;
		if (left_len > 60000) {
			m_endpoint->send(m_hdl, (char*)(m_output_buffer + cur_index), 60000, websocketpp::frame::opcode::binary);
			cur_index += 60000;
		}
		else {
			m_endpoint->send(m_hdl, (char*)(m_output_buffer + cur_index), left_len, websocketpp::frame::opcode::binary);
			m_output_index = 0;
		}
	}
	
}

template <class T>
void web_socket_wrapper<T>::close()
{
	websocketpp::lib::error_code err;
	m_endpoint->close(m_hdl, websocketpp::close::status::going_away, "", err);
	if (err) {
		log_error("Error closing connection! socket index %" I64_FMT "u, msg %s", m_index, err.message());
	}
}

template <class T>
void web_socket_wrapper<T>::on_open(T* s, websocketpp::connection_hdl hdl)
{
	m_endpoint = s;
	m_status = "Open";

	//WebSocketEndPoint::connection_ptr con = s->get_con_from_hdl(hdl);
	//m_server = con->get_response_header("Server");
}

template <class T>
void web_socket_wrapper<T>::on_fail(T* s, websocketpp::connection_hdl hdl)
{
	m_status = "Failed";

	//WebSocketEndPoint::connection_ptr con = s->get_con_from_hdl(hdl);
	//m_server = con->get_response_header("Server");
	//m_error_reason = con->get_ec().message();
}

template <class T>
void web_socket_wrapper<T>::on_close(T* s, websocketpp::connection_hdl hdl)
{
	m_status = "Closed";
	//WebSocketEndPoint::connection_ptr con = s->get_con_from_hdl(hdl);
}

template <class T>
void web_socket_wrapper<T>::on_message(websocketpp::connection_hdl, typename T::message_ptr msg)
{
	std::string data = "";
	if (msg->get_opcode() == websocketpp::frame::opcode::text) {
		data = std::move(msg->get_payload());
	}
	else {
		data = std::move(websocketpp::utility::to_hex(msg->get_payload()));
	}
	char* buffer = m_input_handler->buffer((int)data.length());
	memcpy(buffer, data.c_str(), data.length());
}

#endif // !_WS_WRAPPER_HPP_