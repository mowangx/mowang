
#ifndef _SOCKET_WRAPPER_H_
#define _SOCKET_WRAPPER_H_

#include "socket.h"

class CGameHandler;

class CSocketWrapper : public CSocket
{
public:
	CSocketWrapper(const char* ip, TPort_t port) : CSocket(ip, port) {

	}

	CSocketWrapper() : CSocket() {

	}

public:
	virtual CGameHandler* create_handler() = 0;

	TSocketWrapperEventArg_t& get_wrapper_event_arg() { 
		return m_wrapper_event_arg; }

private:
	TSocketWrapperEventArg_t m_wrapper_event_arg;
};

template <class T>
class CSocketListener : public CSocketWrapper
{
public:
	CSocketListener(const char* ip, TPort_t port) : CSocketWrapper(ip, port) {

	}

	~CSocketListener() {

	}

public:
	virtual CGameHandler* create_handler() override {
		return new T();
	}
};

template <class T>
class CSocketConnector : public CSocketWrapper
{
public:
	CSocketConnector() : CSocketWrapper() {

	}

	~CSocketConnector() {

	}


public:
	virtual CGameHandler* create_handler() override {
		return new T();
	}
};

#endif // !_SOCKET_WRAPPER_H_

