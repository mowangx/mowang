
#include "socket_output_stream.h"
#include "socket.h"
#include "log.h"

CSocketOutputStream::CSocketOutputStream()
{
	m_socket = NULL;
	m_buffer = NULL;
	m_buffer_len = 0;
	m_max_buffer_len = 0;
	m_head = 0;
	m_tail = 0;
	m_output_len = 0;
}

CSocketOutputStream::~CSocketOutputStream()
{
	DSafeDeleteArray(m_buffer);
}

sint32 CSocketOutputStream::_length()const
{
	if (m_head<m_tail)
		return m_tail - m_head;

	else if (m_head>m_tail)
		return m_buffer_len - m_head + m_tail;

	return 0;
}

sint32 CSocketOutputStream::write(const char* buf, sint32 len)
{
	//					//
	//     T  H			//    H   T			LEN=10
	// 0123456789		// 0123456789
	// abcd...efg		// ...abcd...
	//					//

	sint32 nFree = ((m_head <= m_tail) ? (m_buffer_len - m_tail + m_head - 1) : (m_head - m_tail - 1));

	if (len >= nFree)
	{
		if (!resize(len - nFree + 1))
			return 0;
	}

	if (m_head <= m_tail)
	{
		if (m_head == 0)
		{
			nFree = m_buffer_len - m_tail - 1;
			memcpy(&m_buffer[m_tail], buf, len);
		}
		else
		{
			nFree = m_buffer_len - m_tail;
			if (len <= nFree)
			{
				memcpy(&m_buffer[m_tail], buf, len);
			}
			else
			{
				memcpy(&m_buffer[m_tail], buf, nFree);
				memcpy(m_buffer, &buf[nFree], len - nFree);
			}
		}
	}
	else
	{
		memcpy(&m_buffer[m_tail], buf, len);
	}

	m_tail = (m_tail + len) % m_buffer_len;
	sint32 tempLen = _length();
	m_output_len = tempLen;
	return len;
}

void CSocketOutputStream::initsize(CSocket* sock, sint32 BufferLen, sint32 MaxBufferLen)
{
	m_socket = sock;
	m_buffer_len = BufferLen;
	m_max_buffer_len = MaxBufferLen;

	m_head = 0;
	m_tail = 0;

	DSafeDeleteArray(m_buffer);
	m_buffer = new char[m_buffer_len];

	memset(m_buffer, 0, m_buffer_len);
}

sint32 CSocketOutputStream::flush()
{
	sint32 nFlushed = 0;
	sint32 nSent = 0;
	sint32 nLeft;

	if (m_buffer_len>m_max_buffer_len)
	{//如果单个客户端的缓存太大，则重新设置缓存，并将此客户端断开连接
		initsize(m_socket, m_buffer_len, m_max_buffer_len);
		m_socket->set_active(false);
		log_error("output stream too big, socket index = "I64_FMT"u", m_socket->get_socket_index());
		return SOCKET_ERROR - 1;
	}

#if defined(OS_WINDOWS)
	sint32 flag = MSG_DONTROUTE;
#elif defined(OS_UNIX)
	sint32 flag = MSG_NOSIGNAL;
#endif

	if (m_head < m_tail)
	{
		nLeft = m_tail - m_head;

		while (nLeft > 0)
		{
			nSent = m_socket->send(&m_buffer[m_head], nLeft, flag);
			if (nSent == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nSent == SOCKET_ERROR) return SOCKET_ERROR - 2;
			if (nSent == 0) return 0;

			nFlushed += nSent;
			nLeft -= nSent;
			m_head += nSent;
		}
	}
	else if (m_head > m_tail)
	{
		nLeft = m_buffer_len - m_head;

		while (nLeft > 0)
		{
			nSent = m_socket->send(&m_buffer[m_head], nLeft, flag);
			if (nSent == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nSent == SOCKET_ERROR) return SOCKET_ERROR - 3;
			if (nSent == 0) return 0;

			nFlushed += nSent;
			nLeft -= nSent;
			m_head += nSent;
		}

		m_head = 0;

		nLeft = m_tail;

		while (nLeft > 0)
		{
			nSent = m_socket->send(&m_buffer[m_head], nLeft, flag);
			if (nSent == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nSent == SOCKET_ERROR) return SOCKET_ERROR - 4;
			if (nSent == 0) return 0;

			nFlushed += nSent;
			nLeft -= nSent;
			m_head += nSent;
		}
	}

	m_head = m_tail = 0;

	sint32 tempLen = _length();
	m_output_len = tempLen;
	return nFlushed;
}

bool CSocketOutputStream::resize(sint32 size)
{
	sint32 len = (m_buffer_len >> 1);
	size = len > size ? len : size;
	sint32 newBufferLen = m_buffer_len + size;
	len = _length();

	if (size<0)
	{
		if (newBufferLen<0 || newBufferLen<len)
			return false;
	}

	char * newBuffer = new char[newBufferLen];
	if (newBuffer == NULL)
		return false;

	if (m_head<m_tail)
	{
		memcpy(newBuffer, &m_buffer[m_head], m_tail - m_head);
	}
	else if (m_head>m_tail)
	{
		memcpy(newBuffer, &m_buffer[m_head], m_buffer_len - m_head);
		memcpy(&newBuffer[m_buffer_len - m_head], m_buffer, m_tail);
	}

	DSafeDeleteArray(m_buffer);

	m_buffer = newBuffer;
	m_buffer_len = newBufferLen;
	m_head = 0;
	m_tail = len;

	return true;
}

void CSocketOutputStream::clean_up()
{
	m_head = 0;
	m_tail = 0;
}