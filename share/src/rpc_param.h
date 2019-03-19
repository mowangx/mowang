
#ifndef _RPC_PARAM_H_
#define _RPC_PARAM_H_

#include "dynamic_array.h"
#include "packet_struct.h"
#include "binary_string.h"

template<class Ret, class... Args>
struct func_param_helper {
	typedef typename std::integral_constant<std::size_t, sizeof...(Args)> args_count;
	typedef typename std::tuple<typename std::decay<Args>::type...> args_type;
};

template <class Callable>
struct func_param : func_param<decltype(&Callable::operator())> {

};

template <class Cls, class Ret, class... Args>
struct func_param<Ret(Cls::*)(Args...)> : public func_param_helper<Ret, Args...> {

};

template <class Cls, class Ret, class... Args>
struct func_param<Ret(Cls::*)(Args...) const> : public func_param_helper<Ret, Args...> {

};

template <class Ret, class... Args>
struct func_param<Ret(*)(Args...)> : public func_param_helper<Ret, Args...> {

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
	static void parse_param(T1& value, const char* buffer, int& buffer_index) {
		value = *(T1*)(buffer + buffer_index);
		buffer_index += sizeof(T1);
	}
};

template <>
struct rpc_param_parse<dynamic_string, dynamic_string> {
	static void parse_param(dynamic_string& value, const char* buffer, int& buffer_index) {
		uint16 len = 0;
		rpc_param_parse<uint16, uint16>::parse_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			value.push_back(*(char*)(buffer + buffer_index));
			buffer_index += sizeof(char);
		}
	}
};

template <>
struct rpc_param_parse<dynamic_string_array, dynamic_string> {
	static void parse_param(dynamic_string_array& value, const char* buffer, int& buffer_index) {
		uint16 len = 0;
		rpc_param_parse<uint16, uint16>::parse_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			dynamic_string s;
			rpc_param_parse<dynamic_string, dynamic_string>::parse_param(s, buffer, buffer_index);
			value.push_back(s);
		}
	}
};

template <>
struct rpc_param_parse<dynamic_string_array2, dynamic_string_array> {
	static void parse_param(dynamic_string_array2& value, const char* buffer, int& buffer_index) {
		uint16 len = 0;
		rpc_param_parse<uint16, uint16>::parse_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			dynamic_string_array s_array;
			rpc_param_parse<dynamic_string_array, dynamic_string>::parse_param(s_array, buffer, buffer_index);
			value.push_back(s_array);
		}
	}
};

template <class T>
struct rpc_param_parse<dynamic_array<T>, T> {
	static void parse_param(dynamic_array<T>& value, const char* buffer, int& buffer_index) {
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
struct db_param_parse {
	static void parse_param(T1& value, const char* buffer, int& buffer_index) {
		char* bstr = (char*)(buffer + buffer_index);
		bstr_2_flat_struct(value, bstr);
		buffer_index += (strlen(bstr) + 1);
	}
};

template <class T>
struct db_param_parse<dynamic_array<T>, T> {
	static void parse_param(dynamic_array<T>& value, const char* buffer, int& buffer_index) {
		uint16 len = 0;
		rpc_param_parse<uint16, uint16>::parse_param(len, buffer, buffer_index);
		char* bstr = (char*)(buffer + buffer_index);
		bstr_2_dynamic_struct(value, bstr);
		buffer_index += len;
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
struct rpc_param_fill<dynamic_string_array, dynamic_string> {
	static void fill_param(const dynamic_string_array& value, char* buffer, int& buffer_index) {
		uint16 len = value.size();
		rpc_param_fill<uint16, uint16>::fill_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			rpc_param_fill<dynamic_string, dynamic_string>::fill_param(*value[i], buffer, buffer_index);
		}
	}
};

template <>
struct rpc_param_fill<dynamic_string_array2, dynamic_string_array> {
	static void fill_param(const dynamic_string_array2& value, char* buffer, int& buffer_index) {
		uint16 len = value.size();
		rpc_param_fill<uint16, uint16>::fill_param(len, buffer, buffer_index);
		for (int i = 0; i < len; ++i) {
			rpc_param_fill<dynamic_string_array, dynamic_string>::fill_param(*value[i], buffer, buffer_index);
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
	template <class Functor, class... TupleArgs, class... Args>
	static auto call(const Functor& f, std::tuple<TupleArgs...>& tuple_args, Args&&... args) -> decltype(call_helper<N - 1>::call(f, tuple_args, std::get<N - 1>(tuple_args), std::forward<Args>(args)...)) {
		return call_helper<N - 1>::call(f, tuple_args, std::get<N - 1>(tuple_args), std::forward<Args>(args)...);
	}
};

template <>
struct call_helper<0>
{
	template <class Functor, class... TupleArgs, class... Args>
	static auto call(const Functor& f, std::tuple<TupleArgs...>&, Args&&... args) -> decltype(f(std::forward<Args>(args)...)) {
		return f(std::forward<Args>(args)...);
	}
};

inline std::string append_string(const std::string& s1, const std::string& s2) {
	return s1 + s2;
}

template <class T>
inline void fill_packet(char* buffer, int& buffer_index, const T& p) {
	typedef typename template_type<T>::type TValueType_t;
	rpc_param_fill<T, TValueType_t>::fill_param(p, buffer, buffer_index);
}

template <class T, class... Args>
inline void fill_packet(char* buffer, int& buffer_index, const T& p, const Args&... args) {
	fill_packet(buffer, buffer_index, p);
	fill_packet(buffer, buffer_index, args...);
}

#endif // !_RPC_PARAM_H_

