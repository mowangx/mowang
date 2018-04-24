
#ifndef _BASE_PACKET_H_
#define _BASE_PACKET_H_

#include "base_util.h"
#include "socket_util.h"
#include "packet_struct.h"

class socket_base;

#pragma pack(push, 1)

enum packet_id_type
{
	PACKET_ID_SERVER_INFO = 0x01,

	PACKET_ID_LOGIN = 0x10,

	PACKET_ID_RPC_BY_INDEX = 0x61,
	PACKET_ID_RPC_BY_NAME = 0x62,

	PACKET_ID_ROLE_RPC_BY_INDEX = 0x81,
	PACKET_ID_ROLE_RPC_BY_NAME = 0x81,

	PACKET_ID_TRANSFER_ROLE = 0xF5,
	PACKET_ID_TRANSFER_STUB = 0xF6,
	PACKET_ID_TRANSFER_CLIENT = 0xF7,
	PACKET_ID_TRANSFER_SERVER_BY_INDEX = 0xF8,
	PACKET_ID_TRANSFER_SERVER_BY_NAME = 0xF9,
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

class server_info_packet : public packet_base
{
public:
	server_info_packet() : packet_base(PACKET_ID_SERVER_INFO) {

	}

public:
	game_server_info m_server_info;
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

class role_rpc_by_index_packet : public packet_base
{
public:
	role_rpc_by_index_packet() : packet_base(PACKET_ID_ROLE_RPC_BY_INDEX) {
		m_role_id = INVALID_ROLE_ID;
		m_rpc_index = 0;
		memset(m_buffer, 0, 65000);
	}

public:
	TRoleID_t m_role_id;
	uint8 m_rpc_index;
	char m_buffer[65000];
};

class role_rpc_by_name_packet : public packet_base
{
public:
	role_rpc_by_name_packet() : packet_base(PACKET_ID_ROLE_RPC_BY_NAME) {
		m_role_id = INVALID_ROLE_ID;
		memset(m_rpc_name, 0, 100);
		memset(m_buffer, 0, 65000);
	}

public:
	TRoleID_t m_role_id;
	char m_rpc_name[100];
	char m_buffer[65000];
};

class transfer_role_packet : public packet_base
{
public:
	transfer_role_packet() : packet_base(PACKET_ID_TRANSFER_ROLE) {
		m_server_id = INVALID_SERVER_ID;
		m_game_id = INVALID_PROCESS_ID;
		m_role_id = INVALID_ROLE_ID;
		memset(m_buffer, 0, 65000);
	}

public:
	TServerID_t m_server_id;
	TProcessID_t m_game_id;
	TRoleID_t m_role_id;
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

typedef struct PacketRecvInfo
{
	packet_base* packet;
	socket_base* socket;
	PacketRecvInfo() {
		clean_up();
	}

	void clean_up() {
		packet = NULL;
		socket = NULL;
	}

}TPacketRecvInfo_t;

typedef struct PacketSendInfo
{
	packet_base* packet;
	TSocketIndex_t socket_index;
	PacketSendInfo() {
		clean_up();
	}

	void clean_up() {
		packet = NULL;
		socket_index = INVALID_SOCKET_INDEX;
	}

}TPacketSendInfo_t;

#endif