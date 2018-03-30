
#ifndef _RPC_PARAM_H_
#define _RPC_PARAM_H_

#include "dynamic_array.h"
#include "packet_struct.h"

template <typename T>
struct func_param : func_param<decltype(&T::operator())> {};

template<class C, class R, class... Args>
struct func_param<R(C::*)(Args...)> : public func_param<R(*)(Args...)> {

};

template <class C, class R, class... Args>
struct func_param<R(C::*)(Args...) const> : public func_param<R(*)(Args...)> {

};

template<class R, class... Args>
struct func_param<R(*)(Args...)> {
	using args_count = std::integral_constant<std::size_t, sizeof...(Args)>;
	using args_type = std::tuple<typename std::decay<Args>::type...>;
};

template <class T>
struct rpc_param_parse {
	static void parse_param(T& value, char* buffer, int& buffer_index) {
		value = *(T*)(buffer + buffer_index);
		buffer_index += sizeof(T);
	}
};

template <>
struct rpc_param_parse<dynamic_string> {
	static void parse_param(dynamic_string& value, char* buffer, int& buffer_index) {
		uint16 len = 0;
		rpc_param_parse<uint16>::parse_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			value.push_back(*(char*)(buffer + buffer_index));
			buffer_index += sizeof(char);
		}
	}
};

template <>
struct rpc_param_parse<dynamic_array<game_server_info>> {
	static void parse_param(dynamic_array<game_server_info>& value, char* buffer, int& buffer_index) {
		uint16 len = 0;
		rpc_param_parse<uint16>::parse_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			game_server_info server_info;
			rpc_param_parse<game_server_info>::parse_param(server_info, buffer, buffer_index);
			value.push_back(server_info);
		}
	}
};


template <class T>
struct rpc_param_fill {
	static void fill_param(const T& value, char* buffer, int& buffer_index) {
		memcpy((void*)(buffer + buffer_index), &value, sizeof(value));
		buffer_index += sizeof(value);
	}
};

template <>
struct rpc_param_fill<dynamic_string> {
	static void fill_param(const dynamic_string& value, char* buffer, int& buffer_index) {
		uint16 len = value.size();
		rpc_param_fill<uint16>::fill_param(len, buffer, buffer_index);
		memcpy((void*)(buffer + buffer_index), value.data(), len);
		buffer_index += len;
	}
};

template <>
struct rpc_param_fill<dynamic_array<game_server_info>> {
	static void fill_param(const dynamic_array<game_server_info>& value, char* buffer, int& buffer_index) {
		uint16 len = value.size();
		rpc_param_fill<uint16>::fill_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			rpc_param_fill<game_server_info>::fill_param(value[i], buffer, buffer_index);
		}
	}
};

template <size_t N>
struct rpc_param_wrapper {
	template <class T>
	static void convert_core(T& params, char* buffer, int& buffer_index) {
		auto v = std::get<N>(params);
		typedef typename decltype(v) TValue_t;
		rpc_param_parse<TValue_t>::parse_param(std::get<N>(params), buffer, buffer_index);
	}
};

template <size_t N, size_t M>
struct rpc_param {
	template <class T>
	static void convert(T& params, char* buffer, int& buffer_index) {
		rpc_param_wrapper<M - N>::convert_core(params, buffer, buffer_index);
		rpc_param<N - 1, M>::convert(params, buffer, buffer_index);
	}
};

template <size_t M>
struct rpc_param<0, M> {
	template <class T>
	static void convert(T& params, char* buffer, int& buffer_index) {

	}
};

#endif // !_RPC_PARAM_H_

