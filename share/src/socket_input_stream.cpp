
#include "socket_input_stream.h"
#include "socket.h"
#include "log.h"

socket_input_stream::socket_input_stream()
{
	m_socket = NULL;
	m_buffer = NULL;
	m_buffer_len = 0;
	m_max_buffer_len = 0;
	m_head = 0;
	m_tail = 0;
	m_input_len = 0;
}

socket_input_stream::~socket_input_stream()
{
	DSafeDeleteArray(m_buffer);
}

sint32 socket_input_stream::_length() const
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
sint32 socket_input_stream::read(char* buf, sint32 len)
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

bool socket_input_stream::peek(char* buf, sint32 len)
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

sint32 socket_input_stream::peak_int()
{
	sint32 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

uint32 socket_input_stream::peak_uint()
{
	uint32 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

sint16 socket_input_stream::peak_int16()
{
	sint16 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

uint16 socket_input_stream::peak_uint16()
{
	uint16 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

sint8 socket_input_stream::peak_byte()
{
	sint8 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}


bool socket_input_stream::skip(sint32 len)
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
void socket_input_stream::initsize(socket_base* sock, sint32 BufferLen, sint32 MaxBufferLen)
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

sint32 socket_input_stream::fill()
{
	sint32 nFilled = 0;
	sint32 nReceived = 0;
	sint32 nFree = 0;

	if (m_head <= m_tail) {
		if (m_head == 0) {
			nReceived = 0;
			nFree = m_buffer_len - m_tail - 1;
			if (nFree != 0) {
				nReceived = fill_stream(nFilled, nFree, 1);
				if (nReceived < 1) {
					return nReceived;
				}
				m_tail += nReceived;
			}

			if (nReceived == nFree) {
				sint32 ret = check_stream_available(nFilled, 3);
				if (ret < 1) {
					return ret;
				}
			}
		}
		else {
			nFree = m_buffer_len - m_tail;
			nReceived = fill_stream(nFilled, nFree, 6);
			if (nReceived < 1) {
				return nReceived;
			}
			m_tail = (m_tail + nReceived) % m_buffer_len;

			if (nReceived == nFree) {
				nReceived = 0;
				nFree = m_head - 1;
				if (nFree != 0) {
					nReceived = fill_stream(nFilled, nFree, 8);
					if (nReceived < 1) {
						return nReceived;
					}
					m_tail += nReceived;
				}

				if (nReceived == nFree) {
					sint32 ret = check_stream_available(nFilled, 10);
					if (ret < 1) {
						return ret;
					}
				}
			}
		}
	}
	else {
		nReceived = 0;
		nFree = m_head - m_tail - 1;
		if (nFree != 0) {
			nReceived = fill_stream(nFilled, nFree, 13);
			if (nReceived < 1) {
				return nReceived;
			}
			m_tail += nReceived;
		}
		if (nReceived == nFree) {
			sint32 ret = check_stream_available(nFilled, 15);
			if (ret < 1) {
				return ret;
			}
		}
	}

	m_input_len = _length();
	return nFilled;
}

sint32 socket_input_stream::check_stream_available(sint32 fill_len, sint32 error_sequence_id)
{
	m_input_len = _length(); 

	sint32 available = m_socket->available();
	if (available > 0) {

		// 如果缓冲区过大则断开连接
		if ((m_buffer_len + available + 1) > m_max_buffer_len) {
			initsize(m_socket, m_buffer_len, m_max_buffer_len);
			log_error("Socket input buff is too big!");
			m_socket->set_active(false);
			return SOCKET_ERROR - error_sequence_id;
		}
		if (!resize(available + 1)) {
			return 0;
		}

		sint32 receive_len = fill_stream(fill_len, available, error_sequence_id + 1);
		if (receive_len < 1) {
			return receive_len;
		}

		m_tail += receive_len;
	}
	return 1;
}

sint32 socket_input_stream::fill_stream(sint32& fill_len, sint32 available, sint32 error_sequence_id)
{
	m_input_len = _length();

	sint32 receive_len = m_socket->receive(&m_buffer[m_tail], available);
	if (receive_len == SOCKET_ERROR_WOULDBLOCK) {
		return 0;
	}

	if (receive_len == SOCKET_ERROR) {
		return SOCKET_ERROR - error_sequence_id;
	}

	if (receive_len == 0) {
		return SOCKET_ERROR - error_sequence_id - 1;
	}

	fill_len += receive_len;

	return receive_len;
}

bool socket_input_stream::resize(sint32 size)
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

void socket_input_stream::clean_up()
{
	m_head = 0;
	m_tail = 0;
}

sint32 socket_input_stream::length() const
{
	return m_input_len;
}