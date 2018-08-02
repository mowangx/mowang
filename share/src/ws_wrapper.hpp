
#ifndef _WS_WRAPPER_HPP_
#define _WS_WRAPPER_HPP_

#include "ws_wrapper.h"

template <class T>
web_socket_wrapper<T>::web_socket_wrapper(TSocketIndex_t socket_index, T* endpoint, websocketpp::connection_hdl hdl) : 
	web_socket_wrapper_base(socket_index, hdl),
	m_endpoint(endpoint)
{

}

template<class T>
inline void web_socket_wrapper<T>::write(char* msg, int len)
{
	try {
		m_endpoint->send(m_hdl, msg, len, websocketpp::frame::opcode::text);
	}
	catch (const websocketpp::exception & e) {
		log_error("ws send msg failed for %s", e.what());
	}
}

template <class T>
void web_socket_wrapper<T>::close()
{
	try {
		websocketpp::lib::error_code err;
		m_endpoint->close(m_hdl, websocketpp::close::status::going_away, "", err);
		if (err) {
			log_error("Error closing connection! socket index %" I64_FMT "u, msg %s", m_index, err.message().c_str());
		}
	}
	catch (const websocketpp::exception & e) {
		log_error("ws close failed for %s", e.what());
	}
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
	log_debug("on message! data %s, len %d", data.c_str(), data.length());
}

#endif // !_WS_WRAPPER_HPP_