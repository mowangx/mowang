#ifndef _SOCKET_INPUT_STREAM_H_
#define _SOCKET_INPUT_STREAM_H_

#include "socket_util.h"

class socket_base;

class socket_input_stream
{
public:
	socket_input_stream();
	virtual ~socket_input_stream();


public:
	sint32						read(char* buf, sint32 len);
	sint32                      peak_int();
	uint32                      peak_uint();
	sint16                      peak_int16();
	uint16                      peak_uint16();
	sint8                       peak_byte();
	bool						peek(char* buf, sint32 len);
	bool						skip(sint32 len);
	sint32						fill();
private:
	sint32						check_stream_available(sint32 fill_len, sint32 error_sequence_id);
	sint32						fill_stream(sint32& fill_len, sint32 available, sint32 error_sequence_id);

public:
	void						initsize(socket_base* sock, sint32 BufferLen = DEFAULT_SOCKET_INPUT_BUFFER_SIZE, sint32 MaxBufferLen = DISCONNECT_SOCKET_INPUT_SIZE);
	bool						resize(sint32 size);

	sint32						capacity()const { return m_buffer_len; }
	sint32						size()const { return length(); }
	sint32                      length() const;
	bool						is_empty()const { return m_head == m_tail; }
	void						clean_up();

	sint32						get_head(){ return m_head; }
	sint32						get_tail(){ return m_tail; }
	sint32						get_buff_len(){ return m_buffer_len; }
	char*						get_buff(){ return m_buffer; }

private:
	sint32						_length() const;

private:
	socket_base*			m_socket;
	char*					m_buffer;
	sint32					m_buffer_len;
	sint32					m_max_buffer_len;
	sint32					m_head;
	sint32					m_tail;
	volatile uint32			m_input_len;                  // 当前缓冲区长度, 必须实时改变数值
};

#endif