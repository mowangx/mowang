
#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

#include <unordered_map>

#include "base_packet.h"
#include "log.h"

class CSocket;

class CGameHandler
{
public:
	CGameHandler();

public:
	virtual bool handle(CBasePacket* packet) = 0;

	virtual TPacketInfo_t* create_packet_info() = 0;
	
	virtual char* create_packet(int n) = 0;

	virtual void write_packet(TPacketInfo_t* packet_info) = 0;

	virtual void handle_close();

public:
	void set_socket(CSocket* s);
	CSocket* get_socket() const;

	bool is_valid() const;

	TSocketIndex_t get_socket_index() const;

private:
	CSocket* m_socket;
};

typedef bool (CGameHandler::*TPacketHandler)(CBasePacket* packet);
typedef std::unordered_map<TPacketID_t, TPacketHandler> TPacketHandlers;


template <class T>
class CPacketHandler : public CGameHandler
{
public:
	CPacketHandler() : CGameHandler() {

	}

public:
	virtual bool on_before_handle(CBasePacket* packet) { 
		return true;
	}

	virtual bool on_after_handle(CBasePacket* packet) { 
		return true;
	}

	bool handle(CBasePacket* packet) {
		T* caller = dynamic_cast<T*>(this);
		if (!caller->on_before_handle(packet)) {
			return false;
		}

		auto itr = m_handlers.find(packet->get_packet_id());
		if (itr != m_handlers.end()) {
			TPacketHandler handler = itr->second;
			bool ret = (caller->*handler)(packet);
			caller->on_after_handle(packet);
			return ret;
		}
		else {
			log_error("packet handle faild for not find func! packet id = %u", packet->get_packet_id());
		}

		return false;
	}

	template <class TPacketType_t>
	void send_packet(const TPacketType_t& packet) {
		if (is_valid()) {
			TPacketInfo_t* packet_info = create_packet_info();
			packet_info->socket = get_socket();
			packet_info->packet = (CBasePacket*)create_packet(packet.get_packet_len());
			memcpy(packet_info->packet, &packet, packet.get_packet_len());
			write_packet(packet_info);
		}
		else {
			log_info("send packet failed for handle is invalid!");
		}
	}

	static void register_handler(TPacketID_t id, TPacketHandler handler) {
		m_handlers.insert(TPacketHandlers::value_type(id, handler));
	}

private:
	static TPacketHandlers m_handlers;
};

template <class T>
typename TPacketHandlers CPacketHandler<T>::m_handlers;

#endif // !_PACKET_HANDLER_H_

