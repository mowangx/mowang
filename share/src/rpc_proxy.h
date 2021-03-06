
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

private:
	rpc_proxy(const rpc_proxy&) = delete;
	rpc_proxy& operator = (const rpc_proxy&) = delete;

public:
	template <size_t N, class F1, class F2>
	void register_func(const std::string& func_name, F1 func1, const F2& func2) {
		typedef typename func_param<F1>::args_type args_type;
		m_name_2_func.insert(std::make_pair(func_name, [func2](char* buffer) {
			int buffer_index = 0;
			args_type args_real;
			rpc_param<N, N>::convert(args_real, buffer, buffer_index);
			call_helper<N>::call(func2, args_real);
		}));
	}

	template <size_t N, class F1, class F2>
	void register_func_with_index(const std::string& func_name, F1 func1, const F2& func2) {
		typedef typename func_param<F1>::args_type args_type;
		m_name_2_func_with_index.insert(std::make_pair(func_name, [func2](char* buffer, TSocketIndex_t socket_index) {
			int buffer_index = 0;
			args_type args_real;
			std::get<0>(args_real) = socket_index;
			rpc_param<N-1, N>::convert(args_real, buffer, buffer_index);
			call_helper<N>::call(func2, args_real);
		}));
	}

public:
	void call(uint8 func_index, char* buffer) const;
	void call(const std::string& func_name, char* buffer) const;

	void call_with_index(uint8 func_index, char* buffer, TSocketIndex_t socket_index) const;
	void call_with_index(const std::string& func_name, char* buffer, TSocketIndex_t socket_index) const;

private:
	void clean_up();

private:
	std::map<uint8, std::string> m_index_2_name;
	std::map<std::string, std::function<void(char*)>> m_name_2_func;
	std::map<std::string, std::function<void(char*, TSocketIndex_t)>> m_name_2_func_with_index;
};

class rpc_stub : public singleton<rpc_stub>
{
public:
	rpc_stub();
	~rpc_stub();

public:
	template <size_t N, class F1, class F2>
	void register_func(const std::string& func_name, F1 func1, const F2& func2) {
		m_proxy->register_func<N>(func_name, func1, func2);
	}

	template <size_t N, class F1, class F2>
	void register_func_with_index(const std::string& func_name, F1 func1, const F2& func2) {
		m_proxy->register_func_with_index<N>(func_name, func1, func2);
	}

	void call(uint8 func_index, char* buffer);
	void call(const std::string& func_name, char* buffer);

	void call_with_index(uint8 func_index, char* buffer, TSocketIndex_t socket_index);
	void call_with_index(const std::string& func_name, char* buffer, TSocketIndex_t socket_index);

private:
	rpc_proxy * m_proxy;
};

#define DRpcStub singleton<rpc_stub>::get_instance()

class rpc_entity : public singleton<rpc_entity>
{
public:
	rpc_entity();
	~rpc_entity();

public:
	template <size_t N, class F1, class F2>
	void register_func(TEntityID_t entity_id, const std::string& func_name, F1 func1, const F2& func2) {
		rpc_proxy* proxy = NULL;
		auto itr = m_proxys.find(entity_id);
		if (itr != m_proxys.end()) {
			proxy = itr->second;
		}
		else {
			proxy = new rpc_proxy();
			m_proxys[entity_id] = proxy;
		}
		proxy->register_func<N>(func_name, func1, func2);
	}

	void call(TEntityID_t entity_id, uint8 func_index, char* buffer);
	void call(TEntityID_t entity_id, const std::string& func_name, char* buffer);

private:
	rpc_proxy* get_proxy(TEntityID_t entity_id) const;

private:
	std::map<TEntityID_t, rpc_proxy*> m_proxys;
};

#define DRpcEntity singleton<rpc_entity>::get_instance()

#define DRpcBindFunc_0(obj) obj
#define DRpcBindFunc_1(obj) DRpcBindFunc_0(obj), std::placeholders::_1
#define DRpcBindFunc_2(obj) DRpcBindFunc_1(obj), std::placeholders::_2
#define DRpcBindFunc_3(obj) DRpcBindFunc_2(obj), std::placeholders::_3
#define DRpcBindFunc_4(obj) DRpcBindFunc_3(obj), std::placeholders::_4
#define DRpcBindFunc_5(obj) DRpcBindFunc_4(obj), std::placeholders::_5
#define DRpcBindFunc_6(obj) DRpcBindFunc_5(obj), std::placeholders::_6
#define DRpcBindFunc_7(obj) DRpcBindFunc_6(obj), std::placeholders::_7
#define DRpcBindFunc_8(obj) DRpcBindFunc_7(obj), std::placeholders::_8

// only for client, a simple remote function without any feature
#define DRegisterClientRpc(obj, class_name, func_name, args_count) { \
	DRpcStub.register_func<args_count>(#func_name, &class_name::func_name, std::bind(&class_name::func_name, DRpcBindFunc_##args_count(obj))); \
}

// call singleton mgr, such as game_server, gate_server, the first param of the callback is socket index
#define DRegisterServerRpc(obj, class_name, func_name, args_count) { \
	DRpcStub.register_func_with_index<args_count>(#func_name, &class_name::func_name, std::bind(&class_name::func_name, DRpcBindFunc_##args_count(obj))); \
}

// call stub by stub name, such as roll_stub, every stub will has only one in same server
#define DRegisterStubRpc(obj, class_name, func_name, args_count) { \
	DRpcStub.register_func<args_count>(append_string(#class_name, #func_name), &class_name::func_name, std::bind(&class_name::func_name, DRpcBindFunc_##args_count(obj))); \
}

// call entity by entity id, such as account, role
#define DRegisterEntityRpc(entity_id, obj, class_name, func_name, args_count) { \
	DRpcEntity.register_func<args_count>(entity_id, #func_name, &class_name::func_name, std::bind(&class_name::func_name, DRpcBindFunc_##args_count(obj))); \
}

#endif // !_RPC_PROXY_H_

