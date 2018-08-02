
#ifndef _PACKET_HANDLER_H_
#define _PACKET_HANDLER_H_

#include <map>

#include "log.h"
#include "service_interface.h"

class socket_base;
class rpc_client;

class game_handler
{
public:
	game_handler();
	virtual ~game_handler();

public:
	virtual bool handle(packet_base* packet) = 0;

	virtual void send_packet(packet_base* packet) const = 0;

	virtual void kick() const;

	virtual void handle_init();
	virtual void handle_close();

	virtual bool handle_rpc_by_index(packet_base* packet) const;
	virtual bool handle_rpc_by_name(packet_base* packet) const;
	virtual bool handle_stub_rpc_by_index(packet_base* packet) const;
	virtual bool handle_stub_rpc_by_name(packet_base* packet) const;
	virtual bool handle_role_rpc_by_index(packet_base* packet) const;
	virtual bool handle_role_rpc_by_name(packet_base* packet) const;

protected:
	virtual service_interface* get_service() const = 0;

	virtual bool need_register_server() const {
		return true;
	}

public:
	void set_socket_index(TSocketIndex_t socket_index);
	TSocketIndex_t get_socket_index() const;

	bool is_valid() const;

protected:
	TSocketIndex_t m_socket_index;
	rpc_client* m_rpc_client;
};

typedef bool (game_handler::*packet_handler_func)(packet_base* packet);
typedef std::map<TPacketID_t, packet_handler_func> packet_handler_map;


template <class T>
class packet_handler : public game_handler
{
public:
	packet_handler() : game_handler() {

	}

public:
	virtual bool on_before_handle(packet_base* packet) const { 
		return true;
	}

	virtual bool on_after_handle(packet_base* packet) const { 
		return true;
	}

	virtual bool handle(packet_base* packet) override {
		T* caller = dynamic_cast<T*>(this);
		if (!caller->on_before_handle(packet)) {
			return false;
		}

		auto itr = m_handlers.find(packet->get_packet_id());
		if (itr != m_handlers.end()) {
			packet_handler_func handler = itr->second;
			bool ret = (caller->*handler)(packet);
			caller->on_after_handle(packet);
			//log_info("packet handle sucess! packet id %u, socket index %" I64_FMT "u", packet->get_packet_id(), get_socket_index());
			return ret;
		}

		log_error("packet handle faild for not find packet id! packet id %u, socket index %" I64_FMT "u", packet->get_packet_id(), get_socket_index());
		return false;
	}

	virtual void send_packet(packet_base* packet) const override {
		if (is_valid()) {
			service_interface* s = get_service();
			packet_send_info* packet_info = s->allocate_packet_info();
			packet_info->socket_index = get_socket_index();
			packet_info->buffer_info.len = packet->get_packet_len();
			packet_info->buffer_info.buffer = s->allocate_memory(packet_info->buffer_info.len);
			memcpy(packet_info->buffer_info.buffer, packet, packet_info->buffer_info.len);
			s->push_write_packets(packet_info);
		}
		else {
			log_error("send packet failed for handle is invalid!");
		}
	}

	static void register_handler(TPacketID_t id, packet_handler_func handler) {
		m_handlers.insert(packet_handler_map::value_type(id, handler));
	}

	static void Setup() {
		register_handler((TPacketID_t)PACKET_ID_RPC_BY_INDEX, (packet_handler_func)&packet_handler<T>::handle_rpc_by_index);
		register_handler((TPacketID_t)PACKET_ID_RPC_BY_NAME, (packet_handler_func)&packet_handler<T>::handle_rpc_by_name);
		register_handler((TPacketID_t)PACKET_ID_STUB_RPC_BY_INDEX, (packet_handler_func)&packet_handler<T>::handle_stub_rpc_by_index);
		register_handler((TPacketID_t)PACKET_ID_STUB_RPC_BY_NAME, (packet_handler_func)&packet_handler<T>::handle_stub_rpc_by_name);
		register_handler((TPacketID_t)PACKET_ID_ROLE_RPC_BY_INDEX, (packet_handler_func)&packet_handler<T>::handle_role_rpc_by_index);
		register_handler((TPacketID_t)PACKET_ID_ROLE_RPC_BY_NAME, (packet_handler_func)&packet_handler<T>::handle_role_rpc_by_name);
	}

private:
	static packet_handler_map m_handlers;
};

template <class T>
packet_handler_map packet_handler<T>::m_handlers;

#endif // !_PACKET_HANDLER_H_

