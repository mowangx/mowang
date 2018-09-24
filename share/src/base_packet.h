
#ifndef _BASE_PACKET_H_
#define _BASE_PACKET_H_

#include "base_util.h"
#include "socket_util.h"
#include "packet_struct.h"

class socket_base;
class web_socket_wrapper_base;

#pragma pack(push, 1)

enum packet_id_type
{
	PACKET_ID_LOGIN = 0x1,

	PACKET_ID_RPC_BY_INDEX = 0x65,
	PACKET_ID_RPC_BY_NAME = 0x66,
	PACKET_ID_STUB_RPC_BY_INDEX = 0x67,
	PACKET_ID_STUB_RPC_BY_NAME = 0x68,
	PACKET_ID_ENTITY_RPC_BY_INDEX = 0x6e,
	PACKET_ID_ENTITY_RPC_BY_NAME = 0x6f,

	PACKET_ID_TRANSFER_ENTITY = 0x85,
	PACKET_ID_TRANSFER_STUB = 0x86,
	PACKET_ID_TRANSFER_CLIENT = 0x87,
	PACKET_ID_TRANSFER_WS_CLIENT = 0x88,
	PACKET_ID_TRANSFER_SERVER_BY_INDEX = 0x89,
	PACKET_ID_TRANSFER_SERVER_BY_NAME = 0x8a,
};

class packet_base
{
public:
	packet_base() {
		clean_up();
	}

	packet_base(int id) {
		clean_up();
		m_id = id;
	}

public:
	TPacketLen_t get_packet_len() const {
		return m_len;
	}

	TPacketID_t	get_packet_id() const {
		return m_id;
	}

protected:
	void clean_up() {
		m_len = 0;
		m_id = 0;
		m_check = 0;
	}

public:
	TPacketLen_t m_len;
	TPacketID_t m_id;
	uint32 m_check;
};

class login_packet : public packet_base
{
public:
	login_packet() : packet_base(PACKET_ID_LOGIN) {
		m_platform_id = INVALID_PLATFORM_ID;
		m_server_id = INVALID_SERVER_ID;
		memset(m_user_id.data(), 0, USER_ID_LEN);
	}

public:
	TPlatformID_t m_platform_id;
	TServerID_t m_server_id;
	TUserID_t m_user_id;
};

class rpc_by_index_packet : public packet_base
{
public:
	rpc_by_index_packet() : packet_base(PACKET_ID_RPC_BY_INDEX) {
		m_rpc_index = 0;
		memset(m_buffer, 0, 65000);
	}

public:
	uint8 m_rpc_index;
	char m_buffer[65000];
};

class rpc_by_name_packet : public packet_base
{
public:
	rpc_by_name_packet() : packet_base(PACKET_ID_RPC_BY_NAME){
		memset(m_rpc_name, 0, 100);
		memset(m_buffer, 0, 65000);
	}

public:
	char m_rpc_name[100];
	char m_buffer[65000];
};

class stub_rpc_by_index_packet : public packet_base
{
public:
	stub_rpc_by_index_packet() : packet_base(PACKET_ID_STUB_RPC_BY_INDEX) {
		m_rpc_index = 0;
		memset(m_buffer, 0, 65000);
	}

public:
	uint8 m_rpc_index;
	char m_buffer[65000];
};

class stub_rpc_by_name_packet : public packet_base
{
public:
	stub_rpc_by_name_packet() : packet_base(PACKET_ID_STUB_RPC_BY_NAME) {
		memset(m_rpc_name, 0, 100);
		memset(m_buffer, 0, 65000);
	}

public:
	char m_rpc_name[100];
	char m_buffer[65000];
};

class entity_rpc_by_index_packet : public packet_base
{
public:
	entity_rpc_by_index_packet() : packet_base(PACKET_ID_ENTITY_RPC_BY_INDEX) {
		m_entity_id = INVALID_ENTITY_ID;
		m_rpc_index = 0;
		memset(m_buffer, 0, 65000);
	}

public:
	TEntityID_t m_entity_id;
	uint8 m_rpc_index;
	char m_buffer[65000];
};

class entity_rpc_by_name_packet : public packet_base
{
public:
	entity_rpc_by_name_packet() : packet_base(PACKET_ID_ENTITY_RPC_BY_NAME) {
		m_entity_id = INVALID_ENTITY_ID;
		memset(m_rpc_name, 0, 100);
		memset(m_buffer, 0, 65000);
	}

public:
	TRoleID_t m_entity_id;
	char m_rpc_name[100];
	char m_buffer[65000];
};

class transfer_entity_packet : public packet_base
{
public:
	transfer_entity_packet() : packet_base(PACKET_ID_TRANSFER_ENTITY) {
		m_server_id = INVALID_SERVER_ID;
		m_game_id = INVALID_PROCESS_ID;
		memset(m_buffer, 0, 65000);
	}

public:
	TServerID_t m_server_id;
	TProcessID_t m_game_id;
	char m_buffer[65000];
};

class transfer_stub_packet : public packet_base
{
public:
	transfer_stub_packet() : packet_base(PACKET_ID_TRANSFER_STUB) {
		m_server_id = INVALID_SERVER_ID;
		m_game_id = INVALID_PROCESS_ID;
		memset(m_buffer, 0, 65000);
	}

public:
	TServerID_t m_server_id;
	TProcessID_t m_game_id;
	char m_buffer[65000];
};

class transfer_client_packet : public packet_base
{
public:
	transfer_client_packet() : packet_base(PACKET_ID_TRANSFER_CLIENT) {
		m_client_id = INVALID_SOCKET_INDEX;
		memset(m_buffer, 0, 65000);
	}

public:
	TSocketIndex_t m_client_id;
	char m_buffer[65000];
};

class transfer_client_ws_packet : public packet_base
{
public:
	transfer_client_ws_packet() : packet_base(PACKET_ID_TRANSFER_WS_CLIENT) {
		m_client_id = INVALID_SOCKET_INDEX;
		memset(m_buffer, 0, 65000);
	}

public:
	TSocketIndex_t m_client_id;
	char m_buffer[65000];
};

class transfer_server_by_index_packet : public packet_base
{
public:
	transfer_server_by_index_packet() : packet_base(PACKET_ID_TRANSFER_SERVER_BY_INDEX) {
		m_rpc_index = 0;
		memset(m_buffer, 0, 65000);
	}

public:
	uint8 m_rpc_index;
	char m_buffer[65000];
};

class transfer_server_by_name_packet : public packet_base
{
public:
	transfer_server_by_name_packet() : packet_base(PACKET_ID_TRANSFER_SERVER_BY_NAME) {
		memset(m_rpc_name, 0, 100);
		memset(m_buffer, 0, 65000);
	}

public:
	char m_rpc_name[100];
	char m_buffer[65000];
};

#pragma pack(pop)

struct packet_recv_info
{
	packet_base* packet;
	socket_base* socket;
	packet_recv_info() {
		clean_up();
	}

	void clean_up() {
		packet = nullptr;
		socket = nullptr;
	}

};

struct packet_buffer_info
{
	char* buffer;
	uint32 len;
	packet_buffer_info() {
		clean_up();
	}

	void clean_up() {
		buffer = nullptr;
		len = 0;
	}
};

struct ws_packet_recv_info
{
	web_socket_wrapper_base* socket;
	packet_buffer_info buffer_info;
	ws_packet_recv_info() {
		clean_up();
	}

	void clean_up() {
		socket = nullptr;
		buffer_info.clean_up();
	}
};

struct packet_send_info
{
	TSocketIndex_t socket_index;
	packet_buffer_info buffer_info;
	packet_send_info() {
		clean_up();
	}

	void clean_up() {
		socket_index = INVALID_SOCKET_INDEX;
		buffer_info.clean_up();
	}

};

#endif