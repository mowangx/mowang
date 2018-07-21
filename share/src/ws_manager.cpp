
#include "ws_manager.h"
#include "auto_lock.h"

WebSocketManager::WebSocketManager()
{
	m_socket_sequence_index = INVALID_SOCKET_INDEX;
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
	m_index_2_handle_map.clear();
	m_handle_2_info_map.clear();
}

WebSocketManager::~WebSocketManager()
{
}

void WebSocketManager::start_listen(TPort_t port)
{
	m_server.listen(port);
	m_server.start_accept();
}

bool WebSocketManager::init(TProcessType_t process_type, TProcessID_t process_id)
{
	m_server.set_access_channels(websocketpp::log::alevel::all);
	m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

	m_server.init_asio();

	m_server.set_open_handler(std::bind(&WebSocketManager::on_accept, this, std::placeholders::_1));
	m_server.set_close_handler(std::bind(&WebSocketManager::on_close, this, std::placeholders::_1));
	m_server.set_message_handler(std::bind(&WebSocketManager::on_message, this, std::placeholders::_1, std::placeholders::_2));

	m_socket_sequence_index = process_type;
	m_socket_sequence_index = (m_socket_sequence_index << (sizeof(process_id) * 8)) + process_id;
	m_socket_sequence_index <<= 40;
	return true;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
}

void WebSocketManager::update(uint32 diff)
{
	m_server.run_one();

	handle_new_socket();

	handle_kick_socket();

	handle_release_packet();

	handle_write_msg();

	handle_release_socket();
}

void WebSocketManager::test_kick()
{
}

uint32 WebSocketManager::socket_num() const
{
	return uint32();
}

void WebSocketManager::swap_net_2_logic(
	std::vector<ws_packet_recv_info*>& read_packets, 
	std::vector<packet_send_info*>& finish_write_packets, 
	std::vector<TSocketIndex_t>& new_sockets, 
	std::vector<TSocketIndex_t>& del_sockets)
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

void WebSocketManager::swap_login_2_net(
	const std::vector<packet_send_info*>& write_packets, 
	const std::vector<ws_packet_recv_info*>& finish_read_packets, 
	const std::vector<TSocketIndex_t>& kick_sockets, 
	const std::vector<TSocketIndex_t>& del_sockets)
{
	auto_lock lock(&m_mutex);

	m_write_packets.insert(m_write_packets.end(), write_packets.begin(), write_packets.end());

	m_finish_read_packets.insert(m_finish_read_packets.end(), finish_read_packets.begin(), finish_read_packets.end());

	m_wait_kick_sockets.insert(m_wait_kick_sockets.end(), kick_sockets.begin(), kick_sockets.end());

	m_delete_sockets.insert(m_delete_sockets.end(), del_sockets.begin(), del_sockets.end());
}

void WebSocketManager::on_accept(websocketpp::connection_hdl hdl)
{
	m_new_sockets.push_back(hdl);
	std::cout << "have client connected" << std::endl;
}

void WebSocketManager::on_close(websocketpp::connection_hdl hdl)
{
	std::cout << "have client disconnected" << std::endl;
}

void WebSocketManager::on_message(websocketpp::connection_hdl hdl, message_ptr msg)
{
	auto itr = m_handle_2_info_map.find(hdl);
	if (itr == m_handle_2_info_map.end()) {
		return;
	}
	const ws_socket_info& socket_info = itr->second;
	std::string data = msg->get_payload();
	int len = (int)data.length();
	memcpy(socket_info.handler->buffer(len), data.c_str(), len);

	std::vector<ws_packet_recv_info*> packets;
	packet_base* packet = socket_info.handler->unpacket();
	while (NULL != packet) {
		char* packet_pool = m_mem_pool.allocate(packet->get_packet_len());
		memcpy(packet_pool, packet, packet->get_packet_len());
		ws_packet_recv_info* packet_info = m_packet_info_pool.allocate();
		packet_info->socket_index = socket_info.client_id;
		packet_info->packet = (packet_base*)packet_pool;
		packets.push_back(packet_info);
		packet = socket_info.handler->unpacket();
	}

	{
		if (packets.empty()) {
			return;
		}
		auto_lock lock(&m_mutex);
		m_read_packets.insert(m_read_packets.end(), packets.begin(), packets.end());
	}
}

void WebSocketManager::handle_new_socket()
{
	for (auto socket : m_new_sockets) {
		add_socket(socket);
	}
	m_new_sockets.clear();
}

void WebSocketManager::handle_write_msg()
{
	std::vector<packet_send_info*> packets;
	std::vector<packet_send_info*> finish_packets;

	{
		auto_lock lock(&m_mutex);
		packets.insert(packets.begin(), m_write_packets.begin(), m_write_packets.end());
		m_write_packets.clear();
	}

	for (auto packet_info : packets) {
		send_packet(packet_info->socket_index, (char*)packet_info->packet, packet_info->packet->get_packet_len());
		finish_packets.push_back(packet_info);
	}

	{
		auto_lock lock(&m_mutex);
		m_finish_write_packets.insert(m_finish_write_packets.end(), finish_packets.begin(), finish_packets.end());
	}
}

void WebSocketManager::handle_kick_socket()
{
	std::vector<TSocketIndex_t> kick_sockets;
	{
		auto_lock lock(&m_mutex);
		kick_sockets.insert(kick_sockets.end(), m_wait_kick_sockets.begin(), m_wait_kick_sockets.end());
		m_wait_kick_sockets.clear();
	}

	for (TSocketIndex_t socket_index : kick_sockets) {
		auto itr = m_index_2_handle_map.find(socket_index);
		if (itr != m_index_2_handle_map.end()) {
			del_socket(itr->second);
		}
		else {
			log_error("handle kick socket failed for not find socket index! socket index = %" I64_FMT "u", socket_index);
		}
	}
}

void WebSocketManager::handle_release_socket()
{
	std::vector<TSocketIndex_t> sockets;
	{
		auto_lock lock(&m_mutex);
		sockets.insert(sockets.begin(), m_delete_sockets.begin(), m_delete_sockets.end());
		m_delete_sockets.clear();
	}

	for (auto socket_index : sockets) {
		auto itr = m_index_2_handle_map.find(socket_index);
		if (itr == m_index_2_handle_map.end()) {
			continue;
		}
		auto hdl = itr->second;
		m_index_2_handle_map.erase(socket_index);

		auto hdl_itr = m_handle_2_info_map.find(hdl);
		if (hdl_itr == m_handle_2_info_map.end()) {
			continue;
		}

		const ws_socket_info& socket_info = hdl_itr->second;

		m_packet_buffer_pool.deallocate(socket_info.handler->buffer());
		m_socket_handler_pool.deallocate(socket_info.handler);
		m_handle_2_info_map.erase(hdl);
	}
}

void WebSocketManager::handle_release_packet()
{
	std::vector<ws_packet_recv_info*> packets;
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

void WebSocketManager::add_socket(websocketpp::connection_hdl hdl)
{
	TSocketIndex_t client_id = gen_socket_index();

	m_index_2_handle_map[client_id] = hdl;

	socket_handler* socket_handler = m_socket_handler_pool.allocate();
	socket_handler->set_buffer(m_packet_buffer_pool.allocate());
	m_handle_2_info_map[hdl] = ws_socket_info(client_id, socket_handler);

	{
		auto_lock lock(&m_mutex);
		m_wait_init_sockets.push_back(client_id);
	}
}

void WebSocketManager::del_socket(websocketpp::connection_hdl hdl)
{
	m_server.close(hdl, websocketpp::close::status::normal, "");

	auto itr = m_handle_2_info_map.find(hdl);
	if (itr == m_handle_2_info_map.end()) {
		return;
	}

	{
		auto_lock lock(&m_mutex);
		m_wait_delete_sockets.push_back(itr->second.client_id);
	}
}

void WebSocketManager::send_packet(TSocketIndex_t socket_index, char * msg, uint32 len)
{
	auto itr = m_index_2_handle_map.find(socket_index);
	if (itr == m_index_2_handle_map.end()) {
		log_error("send packet failed for not find socket index! socket index = '%" I64_FMT "u'", socket_index);
		return;
	}

	m_server.send(itr->second, msg, len, websocketpp::frame::opcode::binary);
}

TSocketIndex_t WebSocketManager::gen_socket_index()
{
	return ++m_socket_sequence_index;
}

void WebSocketManager::clean_up()
{
}
