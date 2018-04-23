
#ifndef _RPC_PROXY_H_
#define _RPC_PROXY_H_

#include <map>
#include <vector>
#include <string>

#include "base_util.h"
#include "singleton.h"
#include "rpc_param.h"

class rpc_proxy
{
public:
	rpc_proxy();
	~rpc_proxy();

public:
	template <size_t N, class F1, class F2>
	void register_func(const std::string& func_name, F1 func1, const F2& func2) {
		typedef typename func_param<F1>::args_type args_type;
		m_name_2_func[func_name] = [&](char* buffer) {
			int buffer_index = 0;
			args_type args_real;
			rpc_param<N, N>::convert(args_real, buffer, buffer_index);
			call_func(func2, args_real);
		};
	}

	void call(uint8 func_index, char* buffer);
	void call(const std::string& func_name, char* buffer);

private:
	template <typename F, typename... T>
	auto call_func(F f, std::tuple<T...>& args) -> decltype(call_helper<sizeof...(T)>::call(f, args))
	{
		return call_helper<sizeof...(T)>::call(f, args);
	}

private:
	void clean_up();

private:
	std::map<uint8, std::string> m_index_2_name;
	std::map<std::string, std::function<void(char*)>> m_name_2_func;
};

class rpc_stub : public singleton<rpc_stub>
{
public:
	rpc_stub() { 
		m_proxy = new rpc_proxy();
	}

	~rpc_stub() {
		if (NULL != m_proxy) {
			delete m_proxy;
		}
	}

public:
	template <size_t N, class F1, class F2>
	void register_func(const std::string& func_name, F1 func1, const F2& func2) {
		m_proxy->register_func<N>(func_name, func1, func2);
	}

	void call(uint8 func_index, char* buffer) {
		m_proxy->call(func_index, buffer);
	}

	void call(const std::string& func_name, char* buffer) {
		m_proxy->call(func_name, buffer);
	}

private:
	rpc_proxy* m_proxy;
};

#define DRpcStub singleton<rpc_stub>::get_instance()

class rpc_role : public singleton<rpc_role>
{
public:
	rpc_role() {
		m_proxys.clear();
	}

	~rpc_role() {
		for (auto itr = m_proxys.begin(); itr != m_proxys.end(); ++itr) {
			rpc_proxy* proxy = itr->second;
			if (NULL != proxy) {
				delete proxy;
			}
		}
		m_proxys.clear();
	}

public:
	template <size_t N, class F1, class F2>
	void register_func(TRoleID_t role_id, const std::string& func_name, F1 func1, const F2& func2) {
		auto itr = m_proxys.find(role_id);
		if (itr == m_proxys.end()) {
			rpc_proxy* proxy = new rpc_proxy();
			m_role_proxys[role_id] = proxy;
			itr = m_proxys.find(role_id);
		}
		rpc_proxy* proxy = itr->second;
		proxy->register_func<N>(func_name, func1, func2);
	}

	void call(TRoleID_t role_id, uint8 func_index, char* buffer) {
		auto itr = m_proxys.find(role_id);
		if (itr != m_proxys.end()) {
			rpc_proxy* proxy = itr->second;
			proxy->call(func_index, buffer);
		}
	}

	void call(TRoleID_t role_id, const std::string& func_name, char* buffer) {
		auto itr = m_proxys.find(role_id);
		if (itr != m_proxys.end()) {
			rpc_proxy* proxy = itr->second;
			proxy->call(func_name, buffer);
		}
	}

private:
	std::map<TRoleID_t, rpc_proxy*> m_proxys;
};

#define DRpcRole singleton<rpc_role>::get_instance()

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
	DRpcStub.register_func<args_count>(#func_name, &class_name::func_name, std::bind(&class_name::func_name, DRpcBindFunc_##args_count(obj))); \
}

#define DRegisterStubRpc(obj, class_name, func_name, args_count) { \
	DRpcStub.register_func<args_count>(#class_name###func_name, &class_name::func_name, std::bind(&class_name::func_name, DRpcBindFunc_##args_count(obj))); \
}

#define DRegisterRoleRpc(role_id, obj, class_name, func_name, args_count) { \
	DRpcRole.register_func<args_count>(role_id, #func_name, &class_name::func_name, std::bind(&class_name::func_name, DRpcBindFunc_##args_count(obj))); \
}

#endif // !_RPC_PROXY_H_

