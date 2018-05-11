
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

#define DRpcCreateRolePacket \
transfer_role_packet transfer_packet; \
role_rpc_by_name_packet packet; \
int buffer_index = 0; \
init_role_packet(transfer_packet, packet, server_id, game_server_id, role_id, func_name);

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

	template <class T1>
	void call_remote_func(const std::string& func_name, const T1& p1) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, p1);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		DRpcCreatePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7, p8);
		send_packet(&packet, buffer_index);
	}

public:
	// client call game function, will transfer by gate
	void call_server(const std::string& func_name) {
		DRpcCreateServerPacket;
		send_packet(&packet, buffer_index);
	}

	template <class T1>
	void call_server(const std::string& func_name, const T1& p1) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, p1);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2>
	void call_server(const std::string& func_name, const T1& p1, const T2& p2) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3>
	void call_server(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4>
	void call_server(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_server(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_server(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5, const T6& p6) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_server(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5, const T6& p6, const T7& p7) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7);
		send_packet(&packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_server(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
		const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		DRpcCreateServerPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7, p8);
		send_packet(&packet, buffer_index);
	}

public:
	// game call client function, will transfer by gate
	void call_client(TSocketIndex_t client_id, const std::string& func_name) {
		DRpcCreateClientPacket;
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const T1& p1) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, p1);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const T1& p1, const T2& p2) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, 
		const T5& p5) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, 
		const T5& p5, const T6& p6) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, 
		const T5& p5, const T6& p6, const T7& p7) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_client(TSocketIndex_t client_id, const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, 
		const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		DRpcCreateClientPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7, p8);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

public:
	// call role function, will transfer by gate
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name) {
		DRpcCreateRolePacket;
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1>
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name, const T1& p1) {
		DRpcCreateRolePacket;
		fill_packet(packet.m_buffer, buffer_index, p1);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2>
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name, const T1& p1,
		const T2& p2) {
		DRpcCreateRolePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3>
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name, const T1& p1,
		const T2& p2, const T3& p3) {
		DRpcCreateRolePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4>
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name, const T1& p1,
		const T2& p2, const T3& p3, const T4& p4) {
		DRpcCreateRolePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name, const T1& p1,
		const T2& p2, const T3& p3, const T4& p4, const T5& p5) {
		DRpcCreateRolePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name, const T1& p1,
		const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6) {
		DRpcCreateRolePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name, const T1& p1,
		const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7) {
		DRpcCreateRolePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_role(TServerID_t server_id, TProcessID_t game_server_id, TRoleID_t role_id, const std::string& func_name, const T1& p1,
		const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		DRpcCreateRolePacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7, p8);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

public:
	// call stub function, will transfer by gate
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name) {
		DRpcCreateStubPacket;
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const T1& p1) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, p1);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const T1& p1, const T2& p2) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const T1& p1, const T2& p2, const T3& p3) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7);
		send_packet(&packet, &transfer_packet, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_stub(TServerID_t server_id, TProcessID_t game_server_id, const std::string& class_name, const std::string& func_name,
		const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		DRpcCreateStubPacket;
		fill_packet(packet.m_buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7, p8);
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

	void init_role_packet(transfer_role_packet& transfer_packet, role_rpc_by_name_packet& packet, TServerID_t server_id, TProcessID_t game_id, TRoleID_t role_id, const std::string& func_name) {
		transfer_packet.m_server_id = server_id;
		transfer_packet.m_game_id = game_id;
		packet.m_role_id = role_id;
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

private:
	template <class T1>
	void fill_packet(char* buffer, int& buffer_index, const T1& p1) {
		rpc_param_fill<T1, template_type<T1>::type>::fill_param(p1, buffer, buffer_index);
	}

	template <class T1, class T2>
	void fill_packet(char* buffer, int& buffer_index, const T1& p1, const T2& p2) {
		fill_packet(buffer, buffer_index, p1);
		rpc_param_fill<T2, template_type<T2>::type>::fill_param(p2, buffer, buffer_index);
	}

	template <class T1, class T2, class T3>
	void fill_packet(char* buffer, int& buffer_index, const T1& p1, const T2& p2, const T3& p3) {
		fill_packet(buffer, buffer_index, p1, p2);
		rpc_param_fill<T3, template_type<T3>::type>::fill_param(p3, buffer, buffer_index);
	}

	template <class T1, class T2, class T3, class T4>
	void fill_packet(char* buffer, int& buffer_index, const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
		fill_packet(buffer, buffer_index, p1, p2, p3);
		rpc_param_fill<T4, template_type<T4>::type>::fill_param(p4, buffer, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void fill_packet(char* buffer, int& buffer_index, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5) {
		fill_packet(buffer, buffer_index, p1, p2, p3, p4);
		rpc_param_fill<T5, template_type<T5>::type>::fill_param(p5, buffer, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void fill_packet(char* buffer, int& buffer_index, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6) {
		fill_packet(buffer, buffer_index, p1, p2, p3, p4, p5);
		rpc_param_fill<T6, template_type<T6>::type>::fill_param(p6, buffer, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void fill_packet(char* buffer, int& buffer_index, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7) {
		fill_packet(buffer, buffer_index, p1, p2, p3, p4, p5, p6);
		rpc_param_fill<T7, template_type<T7>::type>::fill_param(p7, buffer, buffer_index);
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void fill_packet(char* buffer, int& buffer_index, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		fill_packet(buffer, buffer_index, p1, p2, p3, p4, p5, p6, p7);
		rpc_param_fill<T8, template_type<T8>::type>::fill_param(p8, buffer, buffer_index);
	}

public:
	const game_handler* get_handler() const {
		return m_handler;
	}

private:
	game_handler * m_handler;
};

#endif // !_RPC_CLIENT_H_
