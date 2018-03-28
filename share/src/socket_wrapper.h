
#ifndef _SOCKET_WRAPPER_H_
#define _SOCKET_WRAPPER_H_

#include "socket.h"

class game_handler;

class socket_wrapper : public socket_base
{
public:
	socket_wrapper(const char* ip, TPort_t port) : socket_base(ip, port) {

	}

	socket_wrapper() : socket_base() {

	}

public:
	virtual game_handler* create_handler() = 0;

	socket_wrapper_event_arg_t& get_wrapper_event_arg() { 
		return m_wrapper_event_arg; }

private:
	socket_wrapper_event_arg_t m_wrapper_event_arg;
};

template <class T>
class socket_listener : public socket_wrapper
{
public:
	socket_listener(const char* ip, TPort_t port) : socket_wrapper(ip, port) {

	}

	~socket_listener() {

	}

public:
	virtual game_handler* create_handler() override {
		return new T();
	}
};

template <class T>
class socket_connector : public socket_wrapper
{
public:
	socket_connector() : socket_wrapper() {

	}

	~socket_connector() {

	}


public:
	virtual game_handler* create_handler() override {
		return new T();
	}
};

#endif // !_SOCKET_WRAPPER_H_

