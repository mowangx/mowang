
#ifndef _SOCKET_HANDLER_H_
#define _SOCKET_HANDLER_H_

class packet_base;

class socket_handler
{
public:
	socket_handler();
	~socket_handler();

public:
	char* buffer(int len);

	char* buffer();
	void set_buffer(char* p);

	packet_base* unpacket();

private:
	void clean_up();

private:
	int m_read_index;
	int m_write_index;
	int m_max_index;
	char* m_buffer;
};

#endif 
