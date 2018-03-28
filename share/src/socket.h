#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "base_util.h"
#include "socket_util.h"
#include "socket_input_stream.h"
#include "socket_output_stream.h"
#include "socket_handler.h"

class game_handler;

class socket_base
{
public:
	// constructor
	socket_base(sint32 inputStreamLen = DEFAULT_SOCKET_INPUT_BUFFER_SIZE, sint32 outputStreamLen = DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE,
		sint32 maxInputStreamLen = DISCONNECT_SOCKET_INPUT_SIZE, sint32 maxOutputStreamLen = DISCONNECT_SOCKET_OUTPUT_SIZE);
	socket_base(const char* host, sint32 port, sint32 inputStreamLen = DEFAULT_SOCKET_INPUT_BUFFER_SIZE,
		sint32 outputStreamLen = DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE, sint32 maxInputStreamLen = DISCONNECT_SOCKET_INPUT_SIZE,
		sint32 maxOutputStreamLen = DISCONNECT_SOCKET_OUTPUT_SIZE);

	// destructor
	virtual ~socket_base();

public:
	virtual bool on_read();
	virtual bool on_write();

	sint32 write(const char* msg, sint32 len);
	sint32 read(char* msg, sint32 len);

	socket_input_stream* get_input_stream();
	socket_output_stream* get_output_stream();

	void clean_up();

public:
	// 需要写入
	bool is_need_write();
	// 唯一索引
	TSocketIndex_t get_socket_index();
	void set_socket_index(TSocketIndex_t index);

	// 读写事件
	TSocketEvent_t& get_read_event();
	TSocketEvent_t& get_write_event();
	socket_event_arg_t& get_event_arg();

public:
	// try connect to remote host
	bool connect(const char* host, sint32 port, sint32 diff);

	// close previous connection and connect to another socket
	bool reconnect(const char* host, sint32 port, sint32 diff);

	socket_base* accept(sint32 inputStreamLen = DEFAULT_SOCKET_INPUT_BUFFER_SIZE, sint32 outputStreamLen = DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE,
		sint32 maxInputStreamLen = DISCONNECT_SOCKET_INPUT_SIZE, sint32 maxOutputStreamLen = DISCONNECT_SOCKET_OUTPUT_SIZE);

	bool bind();
	bool bind(sint32 port);
	bool listen(sint32 backlog);

	void get_local_addr();

private:
	void on_accept_socket();
	void on_connect_socket(const char* host, sint32 port);

public:
	// Local Addr
	void set_addr(const char* host, sint32 port);
	void set_addr(SOCKADDR_IN addr);

	// Remote Addr
	void set_remote_addr(const char* host, sint32 port);
	void set_remote_addr(SOCKADDR_IN addr);

	// send data to peer
	sint32 send(const void* buf, sint32 len, sint32 flags = 0);

	// receive data from peer
	sint32 receive(void* buf, sint32 len, sint32 flags = 0);

	// 当前Socket是数据可读
	sint32 available() const;

	// 创建一个socket
	bool create();

	// close connection
	void close();

	// 重置
	void reset();

	// get/set socket's linger status
	sint32 get_linger()const;
	bool set_linger(sint32 lingertime);

	bool is_reuse_addr()const;
	bool set_reuse_addr(bool on = true);

	// get is Error
	sint32 get_sock_error() const;
	bool is_sock_error() const;

	// get/set socket's nonblocking status
	bool is_non_blocking()const;
	bool set_non_blocking(bool on = true);

	// get/set receive buffer size
	sint32 get_receive_buffer_size()const;
	bool set_receive_buffer_size(sint32 size);

	// get/set send buffer size
	sint32 get_send_buffer_size()const;
	bool set_send_buffer_size(sint32 size);

	// get local address
	sint32 get_port() const;
	TIP_t get_host_ip() const;
	std::string get_host_ip_str() const;

	// get remote address
	sint32 get_remote_port() const;
	TIP_t get_remote_host_ip() const;
	std::string get_remote_host_ip_str() const;

	// check if socket is valid
	bool is_valid()const;

	// get socket descriptor
	TSocketFD_t   get_socket_fd() const;
	void        set_socket_fd(TSocketFD_t id);

	socket_handler* get_socket_handler();
	void set_socket_handler(socket_handler* handler);

	game_handler* get_packet_handler();
	void set_packet_handler(game_handler* handler);

	// 获取缓冲区长度
	sint32 get_input_len();
	sint32 get_output_len();

	// 是否活动链接
	void set_active(bool flag);
	bool is_active();

	// 等待关闭时间
	uint32 get_wait_close_secs();
	void set_wait_close_secs(uint32 secs);
	bool need_del();

protected:
	// Socket句柄
	TSocketFD_t m_socket;

	// 本地地址
	SOCKADDR_IN m_socket_addr;
	char m_host[IP_SIZE + 1];
	sint32 m_port;

	// 远程服务地址
	SOCKADDR_IN m_remote_socket_addr;
	char m_remote_host[IP_SIZE + 1];
	sint32 m_remote_port;

	// 事件
	TSocketEvent_t m_read_event;
	TSocketEvent_t m_write_event;
	socket_event_arg_t m_event_arg;

	// 唯一索引
	TSocketIndex_t m_index;
	// 读流
	socket_input_stream  m_input_stream;
	// 写流
	socket_output_stream m_output_stream;

	// 解包时间
	TTime_t m_last_unpacket_time;
	// 解包个数
	sint32 m_unpacket_num;
	// 是否还活着
	bool m_active_flag;
	// 等待关闭时间(s)
	uint32 m_wait_close_secs;
	uint32 m_wait_close_start_time;

	socket_handler* m_socket_handler;
	game_handler* m_packet_handler;
};

#endif