
#ifndef _RPC_PARAM_H_
#define _RPC_PARAM_H_

#include "dynamic_array.h"
#include "packet_struct.h"

template <class T>
struct func_param : func_param<decltype(&T::operator())> {};

template<class C, class R, class... Args>
struct func_param<R(C::*)(Args...)> : public func_param<R(*)(Args...)> {

};

template <class C, class R, class... Args>
struct func_param<R(C::*)(Args...) const> : public func_param<R(*)(Args...)> {

};

template<class R, class... Args>
struct func_param<R(*)(Args...)> {
	typedef typename std::integral_constant<std::size_t, sizeof...(Args)> args_count;
	typedef typename std::tuple<typename std::decay<Args>::type...> args_type;
};


template <class C>
struct template_type {
	typedef C type;
};

template <template <class> class C, class T>
struct template_type<C<T>>
{
	typedef T type;
};

template <class T1, class T2>
struct rpc_param_parse {
	static void parse_param(T1& value, char* buffer, int& buffer_index) {
		value = *(T1*)(buffer + buffer_index);
		buffer_index += sizeof(T1);
	}
};

template <>
struct rpc_param_parse<dynamic_string, dynamic_string> {
	static void parse_param(dynamic_string& value, char* buffer, int& buffer_index) {
		uint16 len = 0;
		rpc_param_parse<uint16, uint16>::parse_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			value.push_back(*(char*)(buffer + buffer_index));
			buffer_index += sizeof(char);
		}
	}
};

template <>
struct rpc_param_parse<dynamic_string_array, dynamic_string_array> {
	static void parse_param(dynamic_string_array& value, char* buffer, int& buffer_index) {
		uint16 len = 0;
		rpc_param_parse<uint16, uint16>::parse_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			dynamic_string s;
			rpc_param_parse<dynamic_string, dynamic_string>::parse_param(s, buffer, buffer_index);
			value.push_back(s);
		}
	}
};

template <class T>
struct rpc_param_parse<dynamic_array<T>, T> {
	static void parse_param(dynamic_array<T>& value, char* buffer, int& buffer_index) {
		uint16 len = 0;
		typedef typename template_type<T>::type TValueType_t;
		rpc_param_parse<uint16, uint16>::parse_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			T data;
			rpc_param_parse<T, TValueType_t>::parse_param(data, buffer, buffer_index);
			value.push_back(data);
		}
	}
};


template <class T1, class T2>
struct rpc_param_fill {
	static void fill_param(const T1& value, char* buffer, int& buffer_index) {
		memcpy((void*)(buffer + buffer_index), &value, sizeof(value));
		buffer_index += sizeof(value);
	}
};

template <>
struct rpc_param_fill<dynamic_string, dynamic_string> {
	static void fill_param(const dynamic_string& value, char* buffer, int& buffer_index) {
		uint16 len = value.size();
		rpc_param_fill<uint16, uint16>::fill_param(len, buffer, buffer_index);
		memcpy((void*)(buffer + buffer_index), value.data(), len);
		buffer_index += len;
	}
};

template <>
struct rpc_param_fill<dynamic_string_array, dynamic_string_array> {
	static void fill_param(const dynamic_string_array& value, char* buffer, int& buffer_index) {
		uint16 len = value.size();
		rpc_param_fill<uint16, uint16>::fill_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			rpc_param_fill<dynamic_string, dynamic_string>::fill_param(*value[i], buffer, buffer_index);
		}
	}
};

template <class T>
struct rpc_param_fill<dynamic_array<T>, T> {
	static void fill_param(const dynamic_array<T>& value, char* buffer, int& buffer_index) {
		uint16 len = value.size();
		typedef typename template_type<T>::type TValueType_t;
		rpc_param_fill<uint16, uint16>::fill_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			rpc_param_fill<T, TValueType_t>::fill_param(value[i], buffer, buffer_index);
		}
	}
};

template <size_t N>
struct rpc_param_wrapper {
	template <class T>
	static void convert_core(T& params, char* buffer, int& buffer_index) {
		auto v = std::get<N>(params);
		typedef decltype(v) TValue_t;
		typedef typename template_type<TValue_t>::type TValueType_t;
		rpc_param_parse<TValue_t, TValueType_t>::parse_param(std::get<N>(params), buffer, buffer_index);
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

template <size_t N>
struct call_helper
{
	template <typename Functor, typename... ArgsT, typename... ArgsF>
	static auto call(const Functor& f, std::tuple<ArgsT...>& args_t, ArgsF&&... args_f) -> decltype(call_helper<N - 1>::call(f, args_t, std::get<N - 1>(args_t), std::forward<ArgsF>(args_f)...)) {
		return call_helper<N - 1>::call(f, args_t, std::get<N - 1>(args_t), std::forward<ArgsF>(args_f)...);
	}
};

template <>
struct call_helper<0>
{
	template <typename Functor, typename... ArgsT, typename... ArgsF>
	static auto call(const Functor& f, std::tuple<ArgsT...>&, ArgsF&&... args_f) -> decltype(f(std::forward<ArgsF>(args_f)...)) {
		return f(std::forward<ArgsF>(args_f)...);
	}
};

inline std::string append_string(const std::string& s1, const std::string& s2) {
	return s1 + s2;
}

#endif // !_RPC_PARAM_H_

