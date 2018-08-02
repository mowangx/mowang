
#include "socket_handler.h"

#include "base_packet.h"
#include "log.h"

socket_handler::socket_handler()
{
	clean_up();
}

socket_handler::~socket_handler()
{
	clean_up();
}

int socket_handler::get_unpack_len() const
{
	if (m_write_index >= m_read_index) {
		return m_write_index - m_read_index;
	}
	else {
		return m_max_index - m_read_index + m_write_index;
	}
}

char* socket_handler::buffer(int len)
{
	if ((m_write_index + len) >= MAX_PACKET_BUFFER_SIZE) {
		m_max_index = m_write_index;
		m_write_index = len;
		return m_buffer;
	}

	int old_write_index = m_write_index;
	m_write_index += len;
	if (m_write_index > m_read_index && m_write_index < m_max_index) {
		log_error("socket handler! write error! read index = %d, write index = %d, max index = %d, len = %d", m_read_index, m_write_index, m_max_index, len);
		return m_buffer;
	}
	if (m_max_index < m_write_index) {
		m_max_index = m_write_index;
	}
	return (char*)(m_buffer + old_write_index);
}

char* socket_handler::buffer()
{
	return m_buffer;
}

void socket_handler::set_buffer(char* p)
{
	m_buffer = p;
}

packet_base* socket_handler::unpack_packet()
{
	if (m_read_index == m_max_index) {
		if (m_read_index > m_write_index) {
			m_read_index = 0;
			m_max_index = m_write_index;
		}
	}
	int len = m_max_index - m_read_index;
	if (len < (int)sizeof(packet_base)) {
		return NULL;
	}
	packet_base* packet = (packet_base*)(m_buffer + m_read_index);
	if (len < packet->get_packet_len()) {
		return NULL;
	}
	m_read_index += packet->get_packet_len();
	if (m_read_index > MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE) {
		memcpy(m_buffer, (char*)(m_buffer + m_read_index), (m_max_index - m_read_index));
		m_max_index -= m_read_index;
		m_write_index -= m_read_index;
		m_read_index = 0;
	}
	return packet;
}

bool socket_handler::unpack_ws_packet(packet_buffer_info* packet)
{
	if (m_read_index == m_max_index) {
		if (m_read_index > m_write_index) {
			m_read_index = 0;
			m_max_index = m_write_index;
		}
	}

	int start_index = -1;
	int end_index = -1;
	for (int i=m_read_index; i < m_max_index; ++i) {
		char c = *(char*)(m_buffer + i);
		if (c == START_CHARACTER) {
			start_index = i + 1;
		}
		else if (start_index >= 0 && c == END_CHARACTER) {
			end_index = i;
			break;
		}
	}

	if (end_index < 0) {
		return false;
	}

	int len = end_index - start_index;
	packet->buffer = (char*)(m_buffer + start_index);
	packet->len = end_index - start_index;

	m_read_index = end_index + 1;
	if (m_read_index > MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE) {
		memcpy(m_buffer, (char*)(m_buffer + m_read_index), (m_max_index - m_read_index));
		m_max_index -= m_read_index;
		m_write_index -= m_read_index;
		m_read_index = 0;
	}

	return true;
}

void socket_handler::clean_up()
{
	m_read_index = 0;
	m_write_index = 0;
	m_max_index = 0;
}