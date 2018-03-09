
#ifndef _SOCKET_HANDLER_H_
#define _SOCKET_HANDLER_H_

class CBasePacket;

class CSocketHandler
{
public:
	CSocketHandler();
	~CSocketHandler();

public:
	char* buffer(int len);

	char* buffer();
	void setBuffer(char* p);

	CBasePacket* unpacket();

private:
	void clean_up();

private:
	int m_read_index;
	int m_write_index;
	int m_max_index;
	char* m_buffer;
};

#endif 
