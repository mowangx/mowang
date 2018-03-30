
#ifndef _RPC_CLIENT_H_
#define _RPC_CLIENT_H_

#include "packet_handler.h"
#include "rpc_param.h"

#define DRpcCreatePacket \
rpc_by_name_packet rpc_info; \
memcpy(rpc_info.m_rpc_name, func_name.c_str(), func_name.length()); \
int buffer_index = 0;

#define DRpcSendPacket \
rpc_info.m_len = (TPacketLen_t)(sizeof(packet_base) + sizeof(rpc_info.m_rpc_name) + buffer_index); \
m_handler->send_packet(&rpc_info);

class rpc_client
{	
public:
	rpc_client(game_handler* handler) : m_handler(handler) {

	}

public:
	void call_remote_func(const std::string& func_name) {
		DRpcCreatePacket;
		DRpcSendPacket;
	}

	template <class T1>
	void call_remote_func(const std::string& func_name, const T1& p1) {
		DRpcCreatePacket;
		rpc_param_fill<T1>::fill_param(p1, rpc_info.m_buffer, buffer_index);
		DRpcSendPacket;
	}

	template <class T1, class T2>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2) {
		DRpcCreatePacket;
		rpc_param_fill<T1>::fill_param(p1, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T2>::fill_param(p2, rpc_info.m_buffer, buffer_index);
		DRpcSendPacket;
	}

	template <class T1, class T2, class T3>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3) {
		DRpcCreatePacket;
		rpc_param_fill<T1>::fill_param(p1, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T2>::fill_param(p2, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T3>::fill_param(p3, rpc_info.m_buffer, buffer_index);
		DRpcSendPacket;
	}

	template <class T1, class T2, class T3, class T4>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
		DRpcCreatePacket;
		rpc_param_fill<T1>::fill_param(p1, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T2>::fill_param(p2, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T3>::fill_param(p3, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T4>::fill_param(p4, rpc_info.m_buffer, buffer_index);
		DRpcSendPacket;
	}

	template <class T1, class T2, class T3, class T4, class T5>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5) {
		DRpcCreatePacket;
		rpc_param_fill<T1>::fill_param(p1, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T2>::fill_param(p2, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T3>::fill_param(p3, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T4>::fill_param(p4, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T5>::fill_param(p5, rpc_info.m_buffer, buffer_index);
		DRpcSendPacket;
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6) {
		DRpcCreatePacket;
		rpc_param_fill<T1>::fill_param(p1, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T2>::fill_param(p2, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T3>::fill_param(p3, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T4>::fill_param(p4, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T5>::fill_param(p5, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T6>::fill_param(p6, rpc_info.m_buffer, buffer_index);
		DRpcSendPacket;
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7) {
		DRpcCreatePacket;
		rpc_param_fill<T1>::fill_param(p1, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T2>::fill_param(p2, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T3>::fill_param(p3, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T4>::fill_param(p4, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T5>::fill_param(p5, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T6>::fill_param(p6, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T7>::fill_param(p7, rpc_info.m_buffer, buffer_index);
		DRpcSendPacket;
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void call_remote_func(const std::string& func_name, const T1& p1, const T2& p2, const T3& p3, const T4& p4, const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
		DRpcCreatePacket;
		rpc_param_fill<T1>::fill_param(p1, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T2>::fill_param(p2, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T3>::fill_param(p3, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T4>::fill_param(p4, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T5>::fill_param(p5, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T6>::fill_param(p6, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T7>::fill_param(p7, rpc_info.m_buffer, buffer_index);
		rpc_param_fill<T8>::fill_param(p8, rpc_info.m_buffer, buffer_index);
		DRpcSendPacket;
	}

private:
	game_handler * m_handler;
};

#endif // !_RPC_CLIENT_H_
