
#include "socket_output_stream.h"
#include "socket.h"
#include "log.h"

CSocketOutputStream::CSocketOutputStream()
{
	_socket = NULL;
	_buffer = NULL;
	_bufferLen = 0;
	_maxBufferLen = 0;
	_head = 0;
	_tail = 0;
	_outputLen = 0;
}

CSocketOutputStream::~CSocketOutputStream()
{
	DSafeDeleteArray(_buffer);
}

sint32 CSocketOutputStream::_length()const
{
	if (_head<_tail)
		return _tail - _head;

	else if (_head>_tail)
		return _bufferLen - _head + _tail;

	return 0;
}

sint32 CSocketOutputStream::write(const char* buf, sint32 len)
{
	//					//
	//     T  H			//    H   T			LEN=10
	// 0123456789		// 0123456789
	// abcd...efg		// ...abcd...
	//					//

	sint32 nFree = ((_head <= _tail) ? (_bufferLen - _tail + _head - 1) : (_head - _tail - 1));

	if (len >= nFree)
	{
		if (!resize(len - nFree + 1))
			return 0;
	}

	if (_head <= _tail)
	{
		if (_head == 0)
		{
			nFree = _bufferLen - _tail - 1;
			memcpy(&_buffer[_tail], buf, len);
		}
		else
		{
			nFree = _bufferLen - _tail;
			if (len <= nFree)
			{
				memcpy(&_buffer[_tail], buf, len);
			}
			else
			{
				memcpy(&_buffer[_tail], buf, nFree);
				memcpy(_buffer, &buf[nFree], len - nFree);
			}
		}
	}
	else
	{
		memcpy(&_buffer[_tail], buf, len);
	}

	_tail = (_tail + len) % _bufferLen;
	sint32 tempLen = _length();
	_outputLen = tempLen;
	return len;
}

void CSocketOutputStream::initsize(CSocket* sock, sint32 BufferLen, sint32 MaxBufferLen)
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

sint32 CSocketOutputStream::flush()
{
	sint32 nFlushed = 0;
	sint32 nSent = 0;
	sint32 nLeft;

	if (_bufferLen>_maxBufferLen)
	{//如果单个客户端的缓存太大，则重新设置缓存，并将此客户端断开连接
		initsize(_socket, _bufferLen, _maxBufferLen);
		_socket->setActive(false);
		log_error("output stream too big, socket index = "I64_FMT"u", _socket->getUniqueIndex());
		return SOCKET_ERROR - 1;
	}

#if defined(OS_WINDOWS)
	sint32 flag = MSG_DONTROUTE;
#elif defined(OS_UNIX)
	sint32 flag = MSG_NOSIGNAL;
#endif

	if (_head < _tail)
	{
		nLeft = _tail - _head;

		while (nLeft > 0)
		{
			nSent = _socket->send(&_buffer[_head], nLeft, flag);
			if (nSent == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nSent == SOCKET_ERROR) return SOCKET_ERROR - 2;
			if (nSent == 0) return 0;

			nFlushed += nSent;
			nLeft -= nSent;
			_head += nSent;
		}
	}
	else if (_head > _tail)
	{
		nLeft = _bufferLen - _head;

		while (nLeft > 0)
		{
			nSent = _socket->send(&_buffer[_head], nLeft, flag);
			if (nSent == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nSent == SOCKET_ERROR) return SOCKET_ERROR - 3;
			if (nSent == 0) return 0;

			nFlushed += nSent;
			nLeft -= nSent;
			_head += nSent;
		}

		_head = 0;

		nLeft = _tail;

		while (nLeft > 0)
		{
			nSent = _socket->send(&_buffer[_head], nLeft, flag);
			if (nSent == SOCKET_ERROR_WOULDBLOCK) return 0;
			if (nSent == SOCKET_ERROR) return SOCKET_ERROR - 4;
			if (nSent == 0) return 0;

			nFlushed += nSent;
			nLeft -= nSent;
			_head += nSent;
		}
	}

	_head = _tail = 0;

	sint32 tempLen = _length();
	_outputLen = tempLen;
	return nFlushed;
}

bool CSocketOutputStream::resize(sint32 size)
{
	sint32 len = (_bufferLen >> 1);
	size = len > size ? len : size;
	sint32 newBufferLen = _bufferLen + size;
	len = _length();

	if (size<0)
	{
		if (newBufferLen<0 || newBufferLen<len)
			return false;
	}

	char * newBuffer = new char[newBufferLen];
	if (newBuffer == NULL)
		return false;

	if (_head<_tail)
	{
		memcpy(newBuffer, &_buffer[_head], _tail - _head);
	}
	else if (_head>_tail)
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

void CSocketOutputStream::cleanUp()
{
	_head = 0;
	_tail = 0;
}