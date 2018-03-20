
#include "socket_input_stream.h"
#include "socket.h"
#include "log.h"

CSocketInputStream::CSocketInputStream()
{
	m_socket = NULL;
	m_buffer = NULL;
	m_buffer_len = 0;
	m_max_buffer_len = 0;
	m_head = 0;
	m_tail = 0;
	m_input_len = 0;
}

CSocketInputStream::~CSocketInputStream()
{
	DSafeDeleteArray(m_buffer);
}

sint32 CSocketInputStream::_length() const
{
	if (m_head<m_tail)
	{
		return m_tail - m_head;
	}
	else if (m_head>m_tail)
	{
		return m_buffer_len - m_head + m_tail;
	}

	return 0;
}

//返回0表示没有读到数据
sint32 CSocketInputStream::read(char* buf, sint32 len)
{
	if (len == 0)
		return 0;

	if (len > _length())
		return 0;

	if (m_head < m_tail)
	{
		memcpy(buf, &m_buffer[m_head], len);
	}
	else
	{
		sint32 rightLen = m_buffer_len - m_head;
		if (len <= rightLen)
		{
			memcpy(buf, &m_buffer[m_head], len);
		}
		else
		{
			memcpy(buf, &m_buffer[m_head], rightLen);
			memcpy(&buf[rightLen], m_buffer, len - rightLen);
		}
	}

	m_head = (m_head + len) % m_buffer_len;

	sint32 tempLen = _length();
	m_input_len = tempLen;
	return len;
}

bool CSocketInputStream::peek(char* buf, sint32 len)
{
	if (len == 0)
		return false;

	if (len>_length())
		return false;

	if (m_head<m_tail)
	{
		memcpy(buf, &m_buffer[m_head], len);

	}
	else
	{
		sint32 rightLen = m_buffer_len - m_head;
		if (len <= rightLen)
		{
			memcpy(&buf[0], &m_buffer[m_head], len);
		}
		else
		{
			memcpy(&buf[0], &m_buffer[m_head], rightLen);
			memcpy(&buf[rightLen], &m_buffer[0], len - rightLen);
		}
	}

	return true;
}

sint32 CSocketInputStream::peak_int()
{
	sint32 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

uint32 CSocketInputStream::peak_uint()
{
	uint32 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

sint16 CSocketInputStream::peak_int16()
{
	sint16 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

uint16 CSocketInputStream::peak_uint16()
{
	uint16 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

sint8 CSocketInputStream::peak_byte()
{
	sint8 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}


bool CSocketInputStream::skip(sint32 len)
{
	if (len == 0)
	{
		return false;
	}

	if (len>_length())
	{
		return false;
	}

	m_head = (m_head + len) % m_buffer_len;

	return true;
}

// @todo 重新初始化
void CSocketInputStream::initsize(CSocket* sock, sint32 BufferLen, sint32 MaxBufferLen)
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

sint32 CSocketInputStream::fill()
{
	sint32 nFilled = 0;
	sint32 nReceived = 0;
	sint32 nFree = 0;

	if (m_head <= m_tail)
	{
		if (m_head == 0)
		{
			//
			// H   T		LEN=10
			// 0123456789
			// abcd......
			//

			nReceived = 0;
			nFree = m_buffer_len - m_tail - 1;
			if (nFree != 0)
			{
				nReceived = m_socket->receive(&m_buffer[m_tail], nFree);
				if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
				if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 1;
				if (nReceived == 0) return SOCKET_ERROR - 2;

				m_tail += nReceived;
				nFilled += nReceived;
			}

			if (nReceived == nFree)
			{
				sint32 available = m_socket->available();
				if (available > 0)
				{
					// 如果缓冲区过大则断开连接
					if ((m_buffer_len + available + 1)>m_max_buffer_len)
					{
						initsize(m_socket, m_buffer_len, m_max_buffer_len);
						log_error("Socket input buff is too big!");
						m_socket->set_active(false);
						return SOCKET_ERROR - 3;
					}
					if (!resize(available + 1))
					{
						return 0;
					}

					nReceived = m_socket->receive(&m_buffer[m_tail], available);
					if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
					if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 4;
					if (nReceived == 0) return SOCKET_ERROR - 5;

					m_tail += nReceived;
					nFilled += nReceived;
				}
			}
		}
		else
		{
			//
			//    H   T		LEN=10
			// 0123456789
			// ...abcd...
			//

			nFree = m_buffer_len - m_tail;
			nReceived = m_socket->receive(&m_buffer[m_tail], nFree);
			if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 6;
			if (nReceived == 0) return SOCKET_ERROR - 7;

			m_tail = (m_tail + nReceived) % m_buffer_len;
			nFilled += nReceived;

			if (nReceived == nFree)
			{
				//				Assert( m_Tail == 0 );

				nReceived = 0;
				nFree = m_head - 1;
				if (nFree != 0)
				{
					nReceived = m_socket->receive(&m_buffer[0], nFree);
					if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
					if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 8;
					if (nReceived == 0) return SOCKET_ERROR - 9;

					m_tail += nReceived;
					nFilled += nReceived;
				}

				if (nReceived == nFree)
				{
					sint32 available = m_socket->available();
					if (available > 0)
					{
						if ((m_buffer_len + available + 1)>m_max_buffer_len)
						{
							initsize(m_socket, m_buffer_len, m_max_buffer_len);
							m_socket->set_active(false);
							log_error("Socket input buff is too big!");
							return SOCKET_ERROR - 10;
						}
						if (!resize(available + 1))
							return 0;

						nReceived = m_socket->receive(&m_buffer[m_tail], available);
						if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
						if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 11;
						if (nReceived == 0) return SOCKET_ERROR - 12;

						m_tail += nReceived;
						nFilled += nReceived;
					}
				}
			}
		}

	}
	else
	{
		//
		//     T  H		LEN=10
		// 0123456789
		// abcd...efg
		//

		nReceived = 0;
		nFree = m_head - m_tail - 1;
		if (nFree != 0)
		{
			nReceived = m_socket->receive(&m_buffer[m_tail], nFree);
			if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 13;
			if (nReceived == 0) return SOCKET_ERROR - 14;

			m_tail += nReceived;
			nFilled += nReceived;
		}
		if (nReceived == nFree)
		{
			sint32 available = m_socket->available();
			if (available>0)
			{
				if ((m_buffer_len + available + 1)>m_max_buffer_len)
				{
					initsize(m_socket, m_buffer_len, m_max_buffer_len);
					m_socket->set_active(false);
					log_error("Socket input buff is too big!");
					return SOCKET_ERROR - 15;
				}
				if (!resize(available + 1))
					return 0;

				nReceived = m_socket->receive(&m_buffer[m_tail], available);
				if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
				if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 16;
				if (nReceived == 0) return SOCKET_ERROR - 17;

				m_tail += nReceived;
				nFilled += nReceived;
			}
		}
	}

	sint32 tempLen = _length();
	m_input_len = tempLen;

	return nFilled;
}

bool CSocketInputStream::resize(sint32 size)
{
	sint32 len = (m_buffer_len >> 1);
	size = len > size ? len : size;
	sint32 newBufferLen = m_buffer_len + size;
	len = _length();

	if (size < 0)
	{
		if (newBufferLen < 0 || newBufferLen < len)
			return false;
	}

	char * newBuffer = new char[newBufferLen];

	if (m_head < m_tail)
	{
		memcpy(newBuffer, &m_buffer[m_head], m_tail - m_head);
	}
	else if (m_head > m_tail)
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

void CSocketInputStream::clean_up()
{
	m_head = 0;
	m_tail = 0;
}

sint32 CSocketInputStream::length() const
{
	return m_input_len;
}