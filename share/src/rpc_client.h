
#ifndef _RPC_CLIENT_H_
#define _RPC_CLIENT_H_

#include "packet_handler.h"

class rpc_client
{	
public:
	rpc_client(game_handler* handler) : m_handler(handler) {

	}

public:
	void call_remote_func(const std::string& func_name) {
		rpc_by_name_packet rpc_info;
		memcpy(rpc_info.rpc_name, func_name.c_str(), func_name.length());
		rpc_info.m_len = sizeof(packet_base) + sizeof(rpc_info.rpc_name);
		m_handler->send_packet(&rpc_info);
	}

	template <class T1>
	void call_remote_func(const std::string& func_name, const T1& p1) {
		rpc_by_name_packet rpc_info;
		memcpy(rpc_info.rpc_name, func_name.c_str(), func_name.length());
		uint16 buffer_index = 0;
		memcpy(rpc_info.buffer, &p1, sizeof(p1));
		buffer_index += sizeof(p1);
		rpc_info.m_len = sizeof(packet_base) + sizeof(rpc_info.rpc_name) + buffer_index;
		m_handler->send_packet(&rpc_info);
	}

	template <class T1, class T2>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2) {
		rpc_by_name_packet rpc_info;
		memcpy(rpc_info.rpc_name, func_name.c_str(), func_name.length());
		uint16 buffer_index = 0;
		memcpy(rpc_info.buffer, &p1, sizeof(p1));
		buffer_index += sizeof(p1);
		memcpy((void*)(rpc_info.buffer + buffer_index), &p2, sizeof(p2));
		buffer_index += sizeof(p2);
		rpc_info.m_len = sizeof(packet_base) + sizeof(rpc_info.rpc_name) + buffer_index;
		m_handler->send_packet(&rpc_info);
	}

	template <class T1, class T2, class T3>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3) {
		rpc_by_name_packet rpc_info;
		memcpy(rpc_info.rpc_name, func_name.c_str(), func_name.length());
		uint16 buffer_index = 0;
		memcpy(rpc_info.buffer, &p1, sizeof(p1));
		buffer_index += sizeof(p1);
		memcpy((void*)(rpc_info.buffer + buffer_index), &p2, sizeof(p2));
		buffer_index += sizeof(p2);
		memcpy((void*)(rpc_info.buffer + buffer_index), &p3, sizeof(p3));
		buffer_index += sizeof(p3);
		rpc_info.m_len = sizeof(packet_base) + sizeof(rpc_info.rpc_name) + buffer_index;
		m_handler->send_packet(&rpc_info);
	}

private:
	game_handler * m_handler;
};

#endif // !_RPC_CLIENT_H_
