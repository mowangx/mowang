#ifndef _SOCKET_INPUT_STREAM_H_
#define _SOCKET_INPUT_STREAM_H_

#include "socket_util.h"

class CSocket;

class CSocketInputStream
{
public:
	CSocketInputStream();
	virtual ~CSocketInputStream();


public:
	sint32						read(char* buf, sint32 len);
	sint32                      peakInt();
	uint32                      peakUint();
	sint16                      peakInt16();
	uint16                      peakUint16();
	sint8                       peakByte();
	bool						peek(char* buf, sint32 len);
	bool						skip(sint32 len);
	sint32						fill();

	void						initsize(CSocket* sock, sint32 BufferLen = DEFAULT_SOCKET_INPUT_BUFFER_SIZE, sint32 MaxBufferLen = DISCONNECT_SOCKET_INPUT_SIZE);
	bool						resize(sint32 size);

	sint32						capacity()const { return _bufferLen; }
	sint32						size()const { return length(); }
	sint32                      length() const;
	bool						isEmpty()const { return _head == _tail; }
	void						cleanUp();

	sint32						getHead(){ return _head; }
	sint32						getTail(){ return _tail; }
	sint32						getBuffLen(){ return _bufferLen; }
	char*						getBuff(){ return _buffer; }

private:
	sint32						_length() const;

private:
	CSocket*                _socket;
	char*		            _buffer;
	sint32		            _bufferLen;
	sint32		            _maxBufferLen;
	sint32		            _head;
	sint32		            _tail;
	volatile uint32			_inputLen;                  // 当前缓冲区长度, 必须实时改变数值
};

#endif