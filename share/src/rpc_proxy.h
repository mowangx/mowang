
#ifndef _RPC_PROXY_H_
#define _RPC_PROXY_H_

#include <map>
#include <vector>
#include <string>

#include "base_util.h"
#include "singleton.h"
#include "rpc_param.h"

class CRpcProxy : public CSingleton<CRpcProxy>
{
public:
	CRpcProxy();
	~CRpcProxy();

public:
	template <size_t N, class F1, class F2>
	void register_func(const std::string& func_name, F1 func1, F2 func2) {
		using args_type = func_param<F1>::args_type;
		m_funcs[func_name] = [&](char* buffer) {
			int buffer_index = 0;
			args_type args_real;
			CRpcParam<N, N>::convert(args_real, buffer, buffer_index);
			call_func(func2, args_real);
		};
	}

	void call(const std::string& func_name, char* buffer);

private:
	template<class F, class T>
	decltype(auto) call_func(F f, const T& args) {
		return call_func_core(f, args, std::make_index_sequence<std::tuple_size<T>::value>());
	}

	template<class F, class T, std::size_t... Idx>
	decltype(auto) call_func_core(F f, const T& args, std::index_sequence<Idx...>) {
		return f(std::get<Idx>(args)...);
	}

private:
	void clean_up();

private:
	std::map<std::string, std::function<void(char*)>> m_funcs;
};

#define DRpcProxy CSingleton<CRpcProxy>::getInstance()


#define DRpcBindFunc_0(obj) obj
#define DRpcBindFunc_1(obj) DRpcBindFunc_0(obj), std::placeholders::_1
#define DRpcBindFunc_2(obj) DRpcBindFunc_1(obj), std::placeholders::_2
#define DRpcBindFunc_3(obj) DRpcBindFunc_2(obj), std::placeholders::_3
#define DRpcBindFunc_4(obj) DRpcBindFunc_3(obj), std::placeholders::_4
#define DRpcBindFunc_5(obj) DRpcBindFunc_4(obj), std::placeholders::_5
#define DRpcBindFunc_6(obj) DRpcBindFunc_5(obj), std::placeholders::_6
#define DRpcBindFunc_7(obj) DRpcBindFunc_6(obj), std::placeholders::_7
#define DRpcBindFunc_8(obj) DRpcBindFunc_7(obj), std::placeholders::_8

// 参数依次为：
#define DRegisterRpc(obj, class_name, func_name, args_count) { \
	DRpcProxy.register_func<args_count>(#func_name, &class_name::func_name, std::bind(&class_name::func_name, DRpcBindFunc_##args_count(obj))); \
}

#endif // !_RPC_PROXY_H_

