
#ifndef _SOCKET_HANDLER_H_
#define _SOCKET_HANDLER_H_

class packet_base;
struct packet_buffer_info;

class socket_handler
{
public:
	socket_handler();
	~socket_handler();

public:
	int get_unpack_len() const;

public:
	char* buffer(int len);

	char* buffer();
	void set_buffer(char* p);

	packet_base* unpack_packet();

	bool unpack_ws_packet(packet_buffer_info* packet);

private:
	void clean_up();

private:
	int m_read_index;
	int m_write_index;
	int m_max_index;
	char* m_buffer;
};

#endif 
