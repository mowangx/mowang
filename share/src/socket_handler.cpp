
#include "socket_handler.h"

#include "base_packet.h"
#include "log.h"

CSocketHandler::CSocketHandler()
{
	clean_up();
}

CSocketHandler::~CSocketHandler()
{
	clean_up();
}

char* CSocketHandler::buffer(int len)
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

char* CSocketHandler::buffer()
{
	return m_buffer;
}

void CSocketHandler::setBuffer(char* p)
{
	m_buffer = p;
}

CBasePacket* CSocketHandler::unpacket()
{
	if (m_read_index == m_max_index) {
		if (m_read_index > m_write_index) {
			m_read_index = 0;
			m_max_index = m_write_index;
		}
	}
	int len = m_max_index - m_read_index;
	if (len < sizeof(CBasePacket)) {
		return NULL;
	}
	CBasePacket* packet = (CBasePacket*)(m_buffer + m_read_index);
	if (packet->get_packet_len() != 11 && packet->get_packet_len() != 27) {
		log_error("socket handler! read error! read index = %d, write index = %d, max index = %d, len = %d", m_read_index, m_write_index, m_max_index, packet->get_packet_len());
		return NULL;
	}
	if (len < packet->get_packet_len()) {
		return NULL;
	}
	if (packet->m_id != 8888888 && packet->m_id != 12345678) {
		log_error("socket handler! read error! read index = %d, write index = %d, max index = %d, len = %d", m_read_index, m_write_index, m_max_index, packet->get_packet_len());
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

void CSocketHandler::clean_up()
{
	m_read_index = 0;
	m_write_index = 0;
	m_max_index = 0;
}