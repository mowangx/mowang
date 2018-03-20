
#ifndef _BASE_PACKET_H_
#define _BASE_PACKET_H_

#include "base_util.h"

class CSocket;

#pragma pack(push, 1)

class CBasePacket
{
public:
	CBasePacket() {
		clean_up();
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

class CLoginRequest : public CBasePacket
{
public:
	CLoginRequest() {
		m_user = 0;
	}

public:
	uint8  m_user;
};

class CLogoutRequest : public CBasePacket
{
public:
	CLogoutRequest() {
		m_user = 0;
		memset(m_name, '\0', 16);
	}

public:
	char	m_name[16];
	uint8	m_user;
};

#pragma pack(pop)

typedef struct PacketInfo
{
	CBasePacket* packet;
	CSocket* socket;
	TUniqueIndex_t index;
	PacketInfo() {
		clean_up();
	}

	void clean_up() {
		packet = NULL;
		socket = NULL;
		index = 0;
	}

}TPacketInfo_t;

#endif