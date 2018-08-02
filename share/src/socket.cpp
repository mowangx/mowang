
#include "socket.h"
#include <string>

#include "debug.h"
#include "log.h"
#include "time_manager.h"
#include "socket_api.h"

#define gxStrcpy(dest, dsize, src, ssize)  {\
	uint32 count = (dsize - 1) > ssize ? (dsize - 1) : ssize; \
	strncpy(dest, src, count);  \
	dest[count] = '\0'; \
}

void socket_base::clean_up()
{
	reset();
	m_index = INVALID_SOCKET_INDEX;
	m_unpacket_num = 0;
	m_last_unpacket_time = time_manager::SysNowTime();
	m_active_flag = true;
	m_wait_close_secs = 0;
	m_wait_close_start_time = MAX_UINT32_NUM;
}

socket_base::socket_base(sint32 inputStreamLen /*= DEFAULT_SOCKET_INPUT_BUFFER_SIZE*/,
	sint32 outputStreamLen /*= DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE*/, sint32 maxInputStreamLen /*= DISCONNECT_SOCKET_INPUT_SIZE*/,
	sint32 maxOutputStreamLen /*= DISCONNECT_SOCKET_OUTPUT_SIZE*/)
{
	clean_up();
	m_input_stream.initsize(this, inputStreamLen, maxInputStreamLen);
	m_output_stream.initsize(this, outputStreamLen, maxOutputStreamLen);
}

socket_base::socket_base(const char* host, sint32 port, sint32 inputStreamLen /*= DEFAULT_SOCKET_INPUT_BUFFER_SIZE*/,
	sint32 outputStreamLen /*= DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE*/, sint32 maxInputStreamLen /*= DISCONNECT_SOCKET_INPUT_SIZE*/,
	sint32 maxOutputStreamLen /*= DISCONNECT_SOCKET_OUTPUT_SIZE*/)
{
	clean_up();
	set_addr(host, port);
	m_input_stream.initsize(this, inputStreamLen, maxOutputStreamLen);
	m_output_stream.initsize(this, outputStreamLen, maxOutputStreamLen);
}

socket_base::~socket_base()
{

}

bool socket_base::create()
{
	m_socket = SOCKET_API::gx_socket(AF_INET, SOCK_STREAM, 0);

	if (is_valid())
	{
		return true;
	}
	else
	{
		return false;
	}
}

// close previous connection and connect to another server socket
bool socket_base::reconnect(const char* host, sint32 port, sint32 diff)
{
	// delete old socket impl object
	close();

	set_addr(host, port);

	create();

	// try to connect
	return connect(host, port, diff);
}

void socket_base::close()
{
	log_info("Close socket! Index %" I64_FMT "u", get_socket_index());
	if (is_valid() && !is_sock_error())
	{
		SOCKET_API::gx_closesocket(m_socket);
	}

	reset();
}

void socket_base::reset()
{
	m_socket = INVALID_SOCKET;
	memset(&m_socket_addr, 0, sizeof(SOCKADDR_IN));
	memset(m_host, 0, IP_SIZE);
	m_port = 0;
}

bool socket_base::connect(const char* host, sint32 port, sint32 diff)
{
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host);
	addr.sin_port = htons(port);

	bool ret = false;
	if (diff != 0)
	{
		ret = SOCKET_API::gx_connect2(m_socket, (const struct sockaddr *)&addr, sizeof(addr), diff);
	}
	else
	{
		ret = SOCKET_API::gx_connect(m_socket, (const struct sockaddr *)&addr, sizeof(addr));
	}

	if (ret)
	{
		on_connect_socket(host, port);
	}

	return ret;
}

socket_base* socket_base::accept(sint32 inputStreamLen /*= DEFAULT_SOCKET_INPUT_BUFFER_SIZE*/, sint32 outputStreamLen /*= DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE*/,
	sint32 maxInputStreamLen /*= DISCONNECT_SOCKET_INPUT_SIZE*/, sint32 maxOutputStreamLen /*= DISCONNECT_SOCKET_OUTPUT_SIZE*/)
{
	socket_base* socket = new socket_base(inputStreamLen, outputStreamLen, maxInputStreamLen, maxOutputStreamLen);
	uint32 addrlen = sizeof(SOCKADDR_IN);
	SOCKADDR_IN addr;
	//        socket->close();

	TSocketFD_t id = SOCKET_API::gx_accept(m_socket, (struct sockaddr *)&addr, &addrlen);
	if (id == INVALID_SOCKET)
	{
		//			gxError("Can't accept socket! %u, %s", errno, strerror(errno));
		DSafeDelete(socket);
		return NULL;
	}

	socket->set_socket_fd(id);
	socket->set_non_blocking(true);
	socket->set_linger(0);
	socket->set_remote_addr(addr);
	socket->on_accept_socket();

	return socket;
}

sint32 socket_base::send(const void* buf, sint32 len, sint32 flags)
{
	sint32 ret = SOCKET_API::gx_send(m_socket, buf, len, flags);
	return ret;
}

sint32 socket_base::receive(void* buf, sint32 len, sint32 flags)
{
	sint32 ret = SOCKET_API::gx_recv(m_socket, buf, len, flags);
	return ret;
}

sint32 socket_base::available()const
{
	return SOCKET_API::gx_availablesocket(m_socket);
}

sint32 socket_base::get_linger()const
{
	struct linger ling;
	uint32 len = sizeof(ling);

	SOCKET_API::gx_getsockopt(m_socket, SOL_SOCKET, SO_LINGER, &ling, &len);

	return ling.l_linger;
}

bool socket_base::set_linger(sint32 lingertime)
{
	struct linger ling;

	ling.l_onoff = lingertime > 0 ? 1 : 0;
	ling.l_linger = lingertime;

	return SOCKET_API::gx_setsockopt(m_socket, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
}

sint32 socket_base::get_sock_error()const
{
	return is_sock_error();
}

bool socket_base::is_non_blocking()const
{
	return SOCKET_API::gx_getsocketnonblocking(m_socket);
}

bool socket_base::set_non_blocking(bool on)
{
	return SOCKET_API::gx_setsocketnonblocking(m_socket, on);
}

sint32 socket_base::get_receive_buffer_size()const
{
	sint32 ReceiveBufferSize;
	uint32 size = sizeof(ReceiveBufferSize);

	SOCKET_API::gx_getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, &ReceiveBufferSize, &size);

	return ReceiveBufferSize;
}

bool socket_base::set_receive_buffer_size(sint32 size)
{
	return (bool)(SOCKET_API::gx_setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, &size, sizeof(sint32)));
}

sint32 socket_base::get_send_buffer_size()const
{
	sint32 SendBufferSize;
	uint32 size = sizeof(SendBufferSize);

	SOCKET_API::gx_getsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, &SendBufferSize, &size);

	return SendBufferSize;
}

bool socket_base::set_send_buffer_size(sint32 size)
{
	return (bool)(SOCKET_API::gx_setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, &size, sizeof(sint32)));
}

sint32 socket_base::get_port()const
{
	return m_port;
}

TIP_t socket_base::get_host_ip()const
{
	return (TIP_t)(m_socket_addr.sin_addr.s_addr);
}

bool socket_base::is_valid()const
{
	return m_socket != INVALID_SOCKET;
}

TSocketFD_t socket_base::get_socket_fd() const
{
	return m_socket;
}

void socket_base::set_socket_fd(TSocketFD_t id)
{
	m_socket = id;
}

socket_handler* socket_base::get_socket_handler()
{
	return m_socket_handler;
}

void socket_base::set_socket_handler(socket_handler* handler)
{
	m_socket_handler = handler;
}

game_handler* socket_base::get_packet_handler()
{
	return m_packet_handler;
}

void socket_base::set_packet_handler(game_handler* handler)
{
	m_packet_handler = handler;
}

bool socket_base::is_sock_error()const
{
	sint32 error;
	uint32 len = sizeof(error);

	sint32 Result = SOCKET_API::gx_getsockopt2(m_socket, SOL_SOCKET, SO_ERROR, &error, &len);
	if (Result == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool socket_base::bind()
{
	bool result = SOCKET_API::gx_bind(m_socket, (const struct sockaddr *)&m_socket_addr, sizeof(m_socket_addr));
	return result;
}

bool socket_base::bind(sint32 port)
{
	m_socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_socket_addr.sin_port = htons(port);

	bool result = SOCKET_API::gx_bind(m_socket, (const struct sockaddr *)&m_socket_addr, sizeof(m_socket_addr));
	set_addr(m_socket_addr);
	if (result)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool socket_base::listen(sint32 backlog)
{
	return SOCKET_API::gx_listen(m_socket, backlog);
}

void socket_base::set_addr(const char* host, sint32 port)
{
	m_socket_addr.sin_family = AF_INET;
	m_socket_addr.sin_port = htons(port);
	if (strcmp("any", host) == 0)
	{
		m_socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		m_socket_addr.sin_addr.s_addr = inet_addr(host);
	}

	gxStrcpy(m_host, IP_SIZE, host, IP_SIZE);
	m_port = port;
}

void socket_base::set_addr(SOCKADDR_IN addr)
{
	m_port = ntohs(addr.sin_port);
	gxStrcpy(m_host, IP_SIZE, inet_ntoa(addr.sin_addr), IP_SIZE);

	memcpy(&m_socket_addr, &addr, sizeof(addr));
	m_socket_addr.sin_family = AF_INET;
}

void socket_base::set_remote_addr(const char* host, sint32 port)
{
	memset(&m_remote_socket_addr, 0, sizeof(m_remote_socket_addr));
	m_remote_socket_addr.sin_family = AF_INET;
	m_remote_socket_addr.sin_port = htons(port);
	m_remote_socket_addr.sin_addr.s_addr = inet_addr(host);

	gxStrcpy(m_remote_host, IP_SIZE, host, IP_SIZE);
	m_remote_port = port;
}

void socket_base::set_remote_addr(SOCKADDR_IN addr)
{
	memset(&m_remote_socket_addr, 0, sizeof(m_remote_socket_addr));

	gxStrcpy(m_remote_host, IP_SIZE, inet_ntoa(addr.sin_addr), IP_SIZE);
	m_remote_port = ntohs(addr.sin_port);

	memcpy(&m_remote_socket_addr, &addr, sizeof(addr));
	m_remote_socket_addr.sin_family = AF_INET;
}

bool socket_base::is_reuse_addr()const
{
	sint32 reuse;
	uint32 len = sizeof(reuse);

	SOCKET_API::gx_getsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, &len);

	return reuse == 1;
}

bool socket_base::set_reuse_addr(bool on)
{
	sint32 opt = on == true ? 1 : 0;

	return SOCKET_API::gx_setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

bool socket_base::on_read()
{
	return m_input_stream.fill() > SOCKET_ERROR;
}

bool socket_base::on_write()
{
	return m_output_stream.flush() > SOCKET_ERROR;
}

TSocketIndex_t socket_base::get_socket_index()
{
	return m_index;
}

void socket_base::set_socket_index(TSocketIndex_t index)
{
	m_index = index;
}

TSocketEvent_t& socket_base::get_read_event()
{
	return m_read_event;
}

TSocketEvent_t& socket_base::get_write_event()
{
	return m_write_event;
}

socket_event_arg_t& socket_base::get_event_arg()
{
	return m_event_arg;
}

bool socket_base::is_need_write()
{
	return m_output_stream.size() > 0;
}

sint32 socket_base::write(const char* msg, sint32 len)
{
	sint32 ret = m_output_stream.write(msg, len);
	return ret;
}

sint32 socket_base::read(char* msg, sint32 len)
{
	sint32 ret = m_input_stream.read(msg, len);
	return ret;
}

socket_input_stream* socket_base::get_input_stream()
{
	return &m_input_stream;
}

socket_output_stream* socket_base::get_output_stream()
{
	return &m_output_stream;
}

sint32 socket_base::get_input_len()
{
	return m_input_stream.length();
}

sint32 socket_base::get_output_len()
{
	return m_output_stream.length();
}

void socket_base::set_active(bool flag)
{
	m_active_flag = flag;
}

bool socket_base::is_active()
{
	return m_active_flag;
}

uint32 socket_base::get_wait_close_secs()
{
	return m_wait_close_secs;
}

void socket_base::set_wait_close_secs(uint32 secs)
{
	m_wait_close_start_time = time_manager::SysNowTime();
	m_wait_close_secs = secs;
}

bool socket_base::need_del()
{
	if (is_active())
	{
		return false;
	}

	if ((time_manager::SysNowTime() - m_wait_close_start_time)>m_wait_close_secs)
	{
		return true;
	}

	return false;
}

std::string socket_base::get_host_ip_str() const
{
	return std::string(m_host);
}

void socket_base::get_local_addr()
{
	SOCKADDR_IN localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	socklen_t len = sizeof(localAddr);
	getsockname(m_socket, (struct sockaddr *)&localAddr, &len);
	set_addr(localAddr);
}

void socket_base::on_accept_socket()
{
	get_local_addr();
}

void socket_base::on_connect_socket(const char* host, sint32 port)
{
	get_local_addr();
	set_remote_addr(host, port);
}

sint32 socket_base::get_remote_port() const
{
	return m_remote_port;
}

TIP_t socket_base::get_remote_host_ip() const
{
	return (TIP_t)(m_remote_socket_addr.sin_addr.s_addr);
}

std::string socket_base::get_remote_host_ip_str() const
{
	return std::string(m_remote_host);
}