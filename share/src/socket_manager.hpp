
#ifndef _SOCKET_MANAGER_HPP_
#define _SOCKET_MANAGER_HPP_

#include "socket_manager.h"
#include "log.h"
#include "game_random.h"

template <class socket_type, class packet_type>
socket_manager<socket_type, packet_type>::socket_manager()
{
	m_socket_sequence_index = 0;
	m_read_packets.clear();
	m_finish_read_packets.clear();
	m_write_packets.clear();
	m_finish_write_packets.clear();
	m_new_sockets.clear();
	m_wait_init_sockets.clear();
	m_wait_kick_sockets.clear();
	m_wait_delete_sockets.clear();
	m_delete_sockets.clear();
	m_sockets.clear();
}

template <class socket_type, class packet_type>
socket_manager<socket_type, packet_type>::~socket_manager()
{

}

template <class socket_type, class packet_type>
bool socket_manager<socket_type, packet_type>::init(TProcessType_t process_type, TProcessID_t process_id)
{
	m_socket_sequence_index = process_type;
	m_socket_sequence_index = (m_socket_sequence_index << (sizeof(process_id) * 8)) + process_id;
	m_socket_sequence_index <<= 40;
	return true;
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::update(uint32 diff)
{
	handle_new_socket();

	handle_unpacket();

	handle_kick_socket();

	handle_release_packet();

	handle_write_msg();

	handle_release_socket();
}

template <class socket_type, class packet_type>
uint32 socket_manager<socket_type, packet_type>::socket_num() const
{
	return (uint32)m_sockets.size();
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::test_get_sockets(std::vector<socket_type*>& sockets)
{
	auto_lock lock(&m_mutex);
	for (auto itr : m_sockets) {
		sockets.push_back(itr.second);
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::test_kick()
{
	if (DGameRandom.rand_odds(10, 1)) {
		if (m_sockets.empty()) {
			return;
		}
		for (auto itr : m_sockets) {
			log_info("test close socket! socket index = '%"I64_FMT"u'", itr.first);
			handle_close_socket(itr.second, false);
			return;
		}
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::swap_net_2_logic(
	std::vector<packet_type*>& read_packets,
	std::vector<packet_send_info*>& finish_write_packets,
	std::vector<socket_type*>& new_sockets,
	std::vector<socket_type*>& del_sockets
)
{
	auto_lock lock(&m_mutex);
	read_packets.insert(read_packets.end(), m_read_packets.begin(), m_read_packets.end());
	m_read_packets.clear();

	finish_write_packets.insert(finish_write_packets.end(), m_finish_write_packets.begin(), m_finish_write_packets.end());
	m_finish_write_packets.clear();

	new_sockets.insert(new_sockets.end(), m_wait_init_sockets.begin(), m_wait_init_sockets.end());
	m_wait_init_sockets.clear();

	del_sockets.insert(del_sockets.end(), m_wait_delete_sockets.begin(), m_wait_delete_sockets.end());
	m_wait_delete_sockets.clear();
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::swap_login_2_net(
	const std::vector<packet_send_info*>& write_packets,
	const std::vector<packet_type*>& finish_read_packets,
	const std::vector<TSocketIndex_t>& kick_sockets,
	const std::vector<socket_type*>& del_sockets
)
{
	auto_lock lock(&m_mutex);

	m_write_packets.insert(m_write_packets.end(), write_packets.begin(), write_packets.end());

	m_finish_read_packets.insert(m_finish_read_packets.end(), finish_read_packets.begin(), finish_read_packets.end());

	m_wait_kick_sockets.insert(m_wait_kick_sockets.end(), kick_sockets.begin(), kick_sockets.end());

	m_delete_sockets.insert(m_delete_sockets.end(), del_sockets.begin(), del_sockets.end());
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::handle_new_socket()
{
	for (auto socket : m_new_sockets) {
		add_socket(socket);
	}
	m_new_sockets.clear();
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::handle_unpacket()
{
	std::vector<socket_type*> del_sockets;

	for (auto itr = m_sockets.begin(); itr != m_sockets.end(); ++itr) {
		socket_type* socket = itr->second;
		if (NULL == socket) {
			continue;
		}

		if (!socket->is_active()) {
			continue;
		}

		if (socket->get_input_len() > 0) {
			handle_socket_unpacket(socket);
		}

		if (!socket->is_active()) {
			del_sockets.push_back(socket);
		}
	}

	for (uint32 i = 0; i < del_sockets.size(); ++i) {
		handle_close_socket(del_sockets[i], false);
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::handle_socket_unpacket(socket_type* socket)
{
	std::vector<packet_type*> packets;
	unpack_packets(packets, socket);
	{
		if (packets.empty()) {
			return;
		}
		auto_lock lock(&m_mutex);
		m_read_packets.insert(m_read_packets.end(), packets.begin(), packets.end());
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::handle_write_msg()
{
	std::vector<packet_send_info*> packets;
	std::vector<packet_send_info*> finish_packets;

	{
		auto_lock lock(&m_mutex);
		packets.insert(packets.begin(), m_write_packets.begin(), m_write_packets.end());
		m_write_packets.clear();
	}

	for (auto packet_info : packets) {
		log_debug("send packet!!!!!!!!! len %d", packet_info->buffer_info.len);
		send_packet(packet_info->socket_index, packet_info->buffer_info.buffer, packet_info->buffer_info.len);
		finish_packets.push_back(packet_info);
	}

	{
		auto_lock lock(&m_mutex);
		m_finish_write_packets.insert(m_finish_write_packets.end(), finish_packets.begin(), finish_packets.end());
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::handle_kick_socket()
{
	std::vector<TSocketIndex_t> kick_sockets;
	{
		auto_lock lock(&m_mutex);
		kick_sockets.insert(kick_sockets.end(), m_wait_kick_sockets.begin(), m_wait_kick_sockets.end());
		m_wait_kick_sockets.clear();
	}

	for (TSocketIndex_t socket_index : kick_sockets) {
		auto itr = m_sockets.find(socket_index);
		if (itr != m_sockets.end()) {
			del_socket(itr->second);
		}
		else {
			log_error("handle kick socket failed for not find socket index! socket index = %" I64_FMT "u", socket_index);
		}
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::handle_close_socket(socket_type* socket, bool write_flag)
{
	if (write_flag) {
		log_info("Write close socke!index = '%"I64_FMT"u'", socket->get_socket_index());
	}
	else {
		log_info("Read close socke!index = '%"I64_FMT"u'", socket->get_socket_index());
	}

	del_socket(socket);
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::handle_release_socket()
{
	std::vector<socket_type*> sockets;
	{
		auto_lock lock(&m_mutex);
		sockets.insert(sockets.begin(), m_delete_sockets.begin(), m_delete_sockets.end());
		m_delete_sockets.clear();
	}

	for (auto socket : sockets) {
		socket_handler* socket_handler = socket->get_socket_handler();
		m_packet_buffer_pool.deallocate(socket_handler->buffer());
		m_socket_handler_pool.deallocate(socket_handler);

		on_release_socket(socket);

		DSafeDelete(socket);
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::handle_release_packet()
{
	std::vector<packet_type*> packets;
	{
		auto_lock lock(&m_mutex);
		packets.insert(packets.begin(), m_finish_read_packets.begin(), m_finish_read_packets.end());
		m_finish_read_packets.clear();
	}
	for (auto packet_info : packets) {
		m_mem_pool.deallocate((char*)packet_info->packet);
		m_packet_info_pool.deallocate(packet_info);
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::send_packet(TSocketIndex_t socket_index, char* msg, uint32 len)
{
	auto itr = m_sockets.find(socket_index);
	if (itr == m_sockets.end()) {
		log_error("send packet failed for socket is invalid, socket index = '%"I64_FMT"u'", socket_index);
		return;
	}
	socket_type* socket = itr->second;
	socket->write(msg, len);
	on_send_packet(socket);
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::add_socket(socket_type* socket)
{
	TSocketIndex_t index = socket->get_socket_index();
	if (m_sockets.find(index) != m_sockets.end()) {
		log_error("The socket index is repeat! index = '%"I64_FMT"u'", index);
		return;
	}

	socket_handler* socket_handler = m_socket_handler_pool.allocate();
	socket_handler->set_buffer(m_packet_buffer_pool.allocate());
	socket->set_socket_handler(socket_handler);

	m_sockets[index] = socket;

	{
		auto_lock lock(&m_mutex);
		m_wait_init_sockets.push_back(socket);
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::del_socket(socket_type* socket)
{
	m_sockets.erase(socket->get_socket_index());

	{
		auto_lock lock(&m_mutex);
		m_wait_delete_sockets.push_back(socket);
	}
}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::on_release_socket(socket_type* socket)
{

}

template <class socket_type, class packet_type>
void socket_manager<socket_type, packet_type>::on_send_packet(socket_type* socket)
{

}

template <class socket_type, class packet_type>
TSocketIndex_t socket_manager<socket_type, packet_type>::gen_socket_index()
{
	return ++m_socket_sequence_index;
}

#endif // !_SOCKET_MANAGER_HPP_