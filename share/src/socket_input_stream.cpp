
#include "socket_input_stream.h"
#include "socket.h"
#include "log.h"

CSocketInputStream::CSocketInputStream()
{
	_socket = NULL;
	_buffer = NULL;
	_bufferLen = 0;
	_maxBufferLen = 0;
	_head = 0;
	_tail = 0;
	_inputLen = 0;
}

CSocketInputStream::~CSocketInputStream()
{
	DSafeDeleteArray(_buffer);
}

sint32 CSocketInputStream::_length() const
{
	if (_head<_tail)
	{
		return _tail - _head;
	}
	else if (_head>_tail)
	{
		return _bufferLen - _head + _tail;
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

	if (_head < _tail)
	{
		memcpy(buf, &_buffer[_head], len);
	}
	else
	{
		sint32 rightLen = _bufferLen - _head;
		if (len <= rightLen)
		{
			memcpy(buf, &_buffer[_head], len);
		}
		else
		{
			memcpy(buf, &_buffer[_head], rightLen);
			memcpy(&buf[rightLen], _buffer, len - rightLen);
		}
	}

	_head = (_head + len) % _bufferLen;

	sint32 tempLen = _length();
	_inputLen = tempLen;
	return len;
}

bool CSocketInputStream::peek(char* buf, sint32 len)
{
	if (len == 0)
		return false;

	if (len>_length())
		return false;

	if (_head<_tail)
	{
		memcpy(buf, &_buffer[_head], len);

	}
	else
	{
		sint32 rightLen = _bufferLen - _head;
		if (len <= rightLen)
		{
			memcpy(&buf[0], &_buffer[_head], len);
		}
		else
		{
			memcpy(&buf[0], &_buffer[_head], rightLen);
			memcpy(&buf[rightLen], &_buffer[0], len - rightLen);
		}
	}

	return true;
}

sint32 CSocketInputStream::peakInt()
{
	sint32 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

uint32 CSocketInputStream::peakUint()
{
	uint32 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

sint16 CSocketInputStream::peakInt16()
{
	sint16 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

uint16 CSocketInputStream::peakUint16()
{
	uint16 len = 0;
	if (uint32(_length()) >= sizeof(len))
	{
		peek((char*)&len, sizeof(len));
	}

	return len;
}

sint8 CSocketInputStream::peakByte()
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

	_head = (_head + len) % _bufferLen;

	return true;
}

// @todo 重新初始化
void CSocketInputStream::initsize(CSocket* sock, sint32 BufferLen, sint32 MaxBufferLen)
{
	_socket = sock;
	_bufferLen = BufferLen;
	_maxBufferLen = MaxBufferLen;

	_head = 0;
	_tail = 0;

	DSafeDeleteArray(_buffer);
	_buffer = new char[_bufferLen];

	memset(_buffer, 0, _bufferLen);
}

sint32 CSocketInputStream::fill()
{
	sint32 nFilled = 0;
	sint32 nReceived = 0;
	sint32 nFree = 0;

	if (_head <= _tail)
	{
		if (_head == 0)
		{
			//
			// H   T		LEN=10
			// 0123456789
			// abcd......
			//

			nReceived = 0;
			nFree = _bufferLen - _tail - 1;
			if (nFree != 0)
			{
				nReceived = _socket->receive(&_buffer[_tail], nFree);
				if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
				if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 1;
				if (nReceived == 0) return SOCKET_ERROR - 2;

				_tail += nReceived;
				nFilled += nReceived;
			}

			if (nReceived == nFree)
			{
				sint32 available = _socket->available();
				if (available > 0)
				{
					// 如果缓冲区过大则断开连接
					if ((_bufferLen + available + 1)>_maxBufferLen)
					{
						initsize(_socket, _bufferLen, _maxBufferLen);
						log_error("Socket input buff is too big!");
						_socket->setActive(false);
						return SOCKET_ERROR - 3;
					}
					if (!resize(available + 1))
					{
						return 0;
					}

					nReceived = _socket->receive(&_buffer[_tail], available);
					if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
					if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 4;
					if (nReceived == 0) return SOCKET_ERROR - 5;

					_tail += nReceived;
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

			nFree = _bufferLen - _tail;
			nReceived = _socket->receive(&_buffer[_tail], nFree);
			if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 6;
			if (nReceived == 0) return SOCKET_ERROR - 7;

			_tail = (_tail + nReceived) % _bufferLen;
			nFilled += nReceived;

			if (nReceived == nFree)
			{
				//				Assert( m_Tail == 0 );

				nReceived = 0;
				nFree = _head - 1;
				if (nFree != 0)
				{
					nReceived = _socket->receive(&_buffer[0], nFree);
					if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
					if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 8;
					if (nReceived == 0) return SOCKET_ERROR - 9;

					_tail += nReceived;
					nFilled += nReceived;
				}

				if (nReceived == nFree)
				{
					sint32 available = _socket->available();
					if (available > 0)
					{
						if ((_bufferLen + available + 1)>_maxBufferLen)
						{
							initsize(_socket, _bufferLen, _maxBufferLen);
							_socket->setActive(false);
							log_error("Socket input buff is too big!");
							return SOCKET_ERROR - 10;
						}
						if (!resize(available + 1))
							return 0;

						nReceived = _socket->receive(&_buffer[_tail], available);
						if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
						if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 11;
						if (nReceived == 0) return SOCKET_ERROR - 12;

						_tail += nReceived;
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
		nFree = _head - _tail - 1;
		if (nFree != 0)
		{
			nReceived = _socket->receive(&_buffer[_tail], nFree);
			if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 13;
			if (nReceived == 0) return SOCKET_ERROR - 14;

			_tail += nReceived;
			nFilled += nReceived;
		}
		if (nReceived == nFree)
		{
			sint32 available = _socket->available();
			if (available>0)
			{
				if ((_bufferLen + available + 1)>_maxBufferLen)
				{
					initsize(_socket, _bufferLen, _maxBufferLen);
					_socket->setActive(false);
					log_error("Socket input buff is too big!");
					return SOCKET_ERROR - 15;
				}
				if (!resize(available + 1))
					return 0;

				nReceived = _socket->receive(&_buffer[_tail], available);
				if (nReceived == SOCKET_ERROR_WOULDBLOCK) return 0;
				if (nReceived == SOCKET_ERROR) return SOCKET_ERROR - 16;
				if (nReceived == 0) return SOCKET_ERROR - 17;

				_tail += nReceived;
				nFilled += nReceived;
			}
		}
	}

	sint32 tempLen = _length();
	_inputLen = tempLen;

	return nFilled;
}

bool CSocketInputStream::resize(sint32 size)
{
	sint32 len = (_bufferLen >> 1);
	size = len > size ? len : size;
	sint32 newBufferLen = _bufferLen + size;
	len = _length();

	if (size < 0)
	{
		if (newBufferLen < 0 || newBufferLen < len)
			return false;
	}

	char * newBuffer = new char[newBufferLen];

	if (_head < _tail)
	{
		memcpy(newBuffer, &_buffer[_head], _tail - _head);
	}
	else if (_head > _tail)
	{
		memcpy(newBuffer, &_buffer[_head], _bufferLen - _head);
		memcpy(&newBuffer[_bufferLen - _head], _buffer, _tail);
	}

	DSafeDeleteArray(_buffer);

	_buffer = newBuffer;
	_bufferLen = newBufferLen;
	_head = 0;
	_tail = len;

	return true;
}

void CSocketInputStream::cleanUp()
{
	_head = 0;
	_tail = 0;
}

sint32 CSocketInputStream::length() const
{
	return _inputLen;
}