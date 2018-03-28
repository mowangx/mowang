
#ifndef _RPC_PARAM_H_
#define _RPC_PARAM_H_


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

template <size_t N>
struct rpc_param_wrapper {
	template <class T>
	static void convert_core(T& params, char* buffer, int& buffer_index) {
		auto v = std::get<N>(params);
		typedef typename decltype(v) TValue_t;
		std::get<N>(params) = *(TValue_t*)(buffer + buffer_index);
		buffer_index += sizeof(TValue_t);
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

