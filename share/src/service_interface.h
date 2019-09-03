
#ifndef _SERVICE_INTERFACE_H_
#define _SERVICE_INTERFACE_H_

#include "base_packet.h"
#include "packet_struct.h"

class game_handler;

class service_interface
{
public:
	virtual packet_send_info* allocate_packet_info() = 0;
	virtual char* allocate_memory(int n) = 0;
	virtual void push_write_packets(packet_send_info* packet_info) = 0;

public:
	virtual void kick_socket(TSocketIndex_t socket_index) = 0;
	virtual void kick_ws_socket(TSocketIndex_t socket_index) = 0;

public:
	virtual void register_handler(game_handler* handler) = 0;
	virtual void unregister_handler(TSocketIndex_t socket_index) = 0;

public:
	virtual void register_server(TSocketIndex_t socket_index, const game_server_info& server_info) = 0;
	
public:
	virtual const game_server_info& get_server_info() const = 0;
};

#endif // !_SERVICE_INTERFACE_H_

