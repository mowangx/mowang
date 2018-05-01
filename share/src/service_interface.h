
#ifndef _SERVICE_INTERFACE_H_
#define _SERVICE_INTERFACE_H_

#include "base_packet.h"
#include "packet_struct.h"

class rpc_client;

class service_interface
{
public:
	virtual TPacketSendInfo_t* allocate_packet_info() = 0;
	virtual char* allocate_memory(int n) = 0;
	virtual void push_write_packets(TPacketSendInfo_t* packet_info) = 0;

public:
	virtual void kick_socket(TSocketIndex_t socket_index) = 0;

public:
	virtual void register_client(rpc_client* client) = 0;
	virtual void unregister_client(TSocketIndex_t socket_index) = 0;

public:
	virtual void register_server(TSocketIndex_t socket_index, const game_server_info& server_info) = 0;
	
public:
	virtual const game_server_info& get_server_info() const = 0;
};

#endif // !_SERVICE_INTERFACE_H_
