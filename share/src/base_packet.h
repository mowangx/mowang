
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
	PACKET_ID_RPC_BY_INDEX = 0xF1,
	PACKET_ID_RPC_BY_NAME = 0xF2
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
	game_process_info m_process_info;
	game_server_info m_server_info;
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

#pragma pack(pop)

typedef struct PacketInfo
{
	packet_base* packet;
	socket_base* socket;
	PacketInfo() {
		clean_up();
	}

	void clean_up() {
		packet = NULL;
		socket = NULL;
	}

}TPacketInfo_t;

#endif