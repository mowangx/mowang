
#ifndef _RPC_CLIENT_H_
#define _RPC_CLIENT_H_

#include "packet_handler.h"
#include "rpc_param.h"

#define DRpcCreatePacket \
rpc_by_name_packet packet; \
int buffer_index = 0; \
init_packet(packet, func_name);

#define DRpcCreateServerPacket \
transfer_server_by_name_packet packet; \
int buffer_index = 0; \
init_packet(packet, func_name);

#define DRpcCreateClientPacket \
transfer_client_packet transfer_packet; \
rpc_by_name_packet packet; \
int buffer_index = 0; \
init_client_packet(transfer_packet, packet, client_id, func_name);

#define DRpcCreateEntityPacket \
transfer_entity_packet transfer_packet; \
entity_rpc_by_name_packet packet; \
int buffer_index = 0; \
init_entity_packet(transfer_packet, packet, server_id, game_server_id, entity_id, func_name);

#define DRpcCreateStubPacket \
std::string real_name = class_name + func_name; \
transfer_stub_packet transfer_packet; \
stub_rpc_by_name_packet packet; \
int buffer_index = 0; \
init_stub_packet(transfer_packet, packet, server_id, game_server_id, real_name);

class rpc_client
{	
public:
	rpc_client(game_handler* handler) : m_handler(handler) {

	}

public:
	// call directly without transfer send, we can use when register process or login
	void call_remote_func(const std::string& func_name) {
		DRpcCreatePacket;
		send_packet(&packet, buffer_index);
	}

	template <class... Args>
	void call_remote_func(const std::string& func_name, const Args&... args) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, args...);
		send_packet(&packet, buffer_index);
	}

public:
	// client call game function, will transfer by gate
	void call_server(const std::string& func_name) {
		DRpcCreateServerPacket;
		send_packet(&packet, buffer_index);
	}

	template <class... Args>
	void call_server(const std::string& func_name, const Args&... args) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, args...);
		send_packet(&packet, buffer_index);
	}

public:
	// game call client function, will transfer by gate
	void call_client(TSocketIndex_t client_id, const std::string& func_name) {
		DRpcCreateClientPacket;
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class... Args>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const Args&... args) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, args...);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	void call_ws_client(TSocketIndex_t client_id, const std::string& msg) {
		transfer_client_ws_packet transfer_packet;
		transfer_packet.m_client_id = client_id;
		memcpy(transfer_packet.m_buffer, msg.c_str(), msg.length());
		send_packet(&transfer_packet, (int)msg.length());
	}

public:
	// call role function, will transfer by gate
	void call_entity(TServerID_t server_id, TProcessID_t game_server_id, TEntityID_t entity_id, const std::string& func_name) {
		DRpcCreateEntityPacket;
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class... Args>
	void call_entity(TServerID_t server_id, TProcessID_t game_server_id, TEntityID_t entity_id, const std::string& func_name, const Args&... args) {
		DRpcCreateEntityPacket;
		fill_packet(packet.m_buffer, buffer_index, args...);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

public:
	// call stub function, will transfer by gate
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name) {
		DRpcCreateStubPacket;
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class... Args>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const Args&... args) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, args...);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

private:
	template <class T>
	void init_packet(T& packet, const std::string& func_name) {
		memcpy(packet.m_rpc_name, func_name.c_str(), func_name.length());
	}

	void init_client_packet(transfer_client_packet& transfer_packet, rpc_by_name_packet& packet, TSocketIndex_t client_id, const std::string& func_name) {
		transfer_packet.m_client_id = client_id;
		init_packet(packet, func_name);
	}

	void init_entity_packet(transfer_entity_packet& transfer_packet, entity_rpc_by_name_packet& packet, TServerID_t server_id, TProcessID_t game_id, TEntityID_t entity_id, const std::string& func_name) {
		transfer_packet.m_server_id = server_id;
		transfer_packet.m_game_id = game_id;
		packet.m_entity_id = entity_id;
		init_packet(packet, func_name);
	}

	void init_stub_packet(transfer_stub_packet& transfer_packet, stub_rpc_by_name_packet& packet, TServerID_t server_id, TProcessID_t game_id, const std::string& func_name) {
		transfer_packet.m_server_id = server_id;
		transfer_packet.m_game_id = game_id;
		init_packet(packet, func_name);
	}

	template <class T>
	void send_packet(T* packet, int len) {
		packet->m_len = TPacketLen_t(sizeof(T) - sizeof(packet->m_buffer) + len);
		m_handler->send_packet(packet);
	}

	template <class T1, class T2>
	void send_packet(T1* packet, T2* transfer_packet, int len) {
		packet->m_len = TPacketLen_t(sizeof(T1) - sizeof(transfer_packet->m_buffer) + len);
		memcpy(transfer_packet->m_buffer, packet, packet->get_packet_len());
		transfer_packet->m_len = TPacketLen_t(sizeof(T2) - sizeof(transfer_packet->m_buffer) + packet->get_packet_len());
		m_handler->send_packet(transfer_packet);
	}

//private:
//	template <class T>
//	void fill_packet(char* buffer, int& buffer_index, const T& p) {
//		typedef typename template_type<T>::type TValueType_t;
//		rpc_param_fill<T, TValueType_t>::fill_param(p, buffer, buffer_index);
//	}
//
//	template <class T, class... Args>
//	void fill_packet(char* buffer, int& buffer_index, const T& p, const Args&... args) {
//		fill_packet(buffer, buffer_index, p);
//		fill_packet(buffer, buffer_index, args...);
//	}

public:
	const game_handler* get_handler() const {
		return m_handler;
	}

private:
	game_handler * m_handler;
};

#endif // !_RPC_CLIENT_H_