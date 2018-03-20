#ifndef _SOCKET_OUTPUT_STREAM_H_
#define _SOCKET_OUTPUT_STREAM_H_

#include "socket_util.h"

class CSocket;

class CSocketOutputStream
{
public:
	CSocketOutputStream();
	virtual ~CSocketOutputStream();


public:
	sint32						write(const char* buf, sint32 len);

	sint32						flush();

	void						initsize(CSocket* sock, sint32 BufferSize = DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE, sint32 MaxBufferSize = DISCONNECT_SOCKET_OUTPUT_SIZE);
	bool						resize(sint32 size);

	sint32                      capacity()const { return m_buffer_len; }

	sint32						size()const { return length(); }
	sint32                      length() const{ return m_output_len; }
	char*						get_buffer()const { return m_buffer; }

	char*						get_tail()const { return &(m_buffer[m_tail]); }

	bool						is_empty()const { return m_head == m_tail; }

	void						clean_up();

	sint32						get_head(){ return m_head; }
	sint32						get_tail(){ return m_tail; }
	sint32						get_buff_len(){ return m_buffer_len; }
	char*						get_buff(){ return m_buffer; }

private:
	sint32						_length()const;

protected:
	CSocket*    m_socket;
	char*		m_buffer;
	sint32		m_buffer_len;
	sint32		m_max_buffer_len;
	sint32		m_head;
	sint32		m_tail;
	volatile uint32	   m_output_len;             // 当前缓冲区长度, 必须实时改变数值
};

#endif