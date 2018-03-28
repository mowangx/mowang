
#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

#include <unordered_map>

#include "base_packet.h"
#include "log.h"

class socket_base;

class game_handler
{
public:
	game_handler();

public:
	virtual bool handle(packet_base* packet) = 0;

	virtual TPacketInfo_t* create_packet_info() = 0;
	
	virtual char* create_packet(int n) = 0;

	virtual void write_packet(TPacketInfo_t* packet_info) = 0;

	virtual void  send_packet(packet_base* packet) = 0;

	virtual void handle_close();

public:
	void set_socket(socket_base* s);
	socket_base* get_socket() const;

	bool is_valid() const;

	TSocketIndex_t get_socket_index() const;

private:
	socket_base* m_socket;
};

typedef bool (game_handler::*packet_handler_func)(packet_base* packet);
typedef std::unordered_map<TPacketID_t, packet_handler_func> packet_handler_map;


template <class T>
class packet_handler : public game_handler
{
public:
	packet_handler() : game_handler() {

	}

public:
	virtual bool on_before_handle(packet_base* packet) { 
		return true;
	}

	virtual bool on_after_handle(packet_base* packet) { 
		return true;
	}

	bool handle(packet_base* packet) {
		T* caller = dynamic_cast<T*>(this);
		if (!caller->on_before_handle(packet)) {
			return false;
		}

		auto itr = m_handlers.find(packet->get_packet_id());
		if (itr != m_handlers.end()) {
			packet_handler_func handler = itr->second;
			bool ret = (caller->*handler)(packet);
			caller->on_after_handle(packet);
			return ret;
		}
		else {
			log_error("packet handle faild for not find func! packet id = %u", packet->get_packet_id());
		}

		return false;
	}

	void send_packet(packet_base* packet) {
		if (is_valid()) {
			TPacketInfo_t* packet_info = create_packet_info();
			packet_info->socket = get_socket();
			packet_info->packet = (packet_base*)create_packet(packet->get_packet_len());
			memcpy(packet_info->packet, packet, packet->get_packet_len());
			write_packet(packet_info);
		}
		else {
			log_info("send packet failed for handle is invalid!");
		}
	}

	static void register_handler(TPacketID_t id, packet_handler_func handler) {
		m_handlers.insert(packet_handler_map::value_type(id, handler));
	}

private:
	static packet_handler_map m_handlers;
};

template <class T>
typename packet_handler_map packet_handler<T>::m_handlers;

#endif // !_PACKET_HANDLER_H_

