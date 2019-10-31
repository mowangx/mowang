
#ifndef _SERVER_ENTITY_H_
#define _SERVER_ENTITY_H_

#include "entity.h"

class server_entity : public entity
{
	typedef entity TBaseType_t;

public:
	server_entity();
	virtual ~server_entity();

public:
	virtual bool init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id);

public:
	void call_client(const std::string& func_name) {
		DRpcWrapper.call_client(get_proxy(), func_name);
	}

	template <class... Args>
	void call_client(const std::string& func_name, const Args&... args) {
		DRpcWrapper.call_client(get_proxy(), func_name, args...);
	}

	void call_ws_client(const std::string& msg) {
		DRpcWrapper.call_ws_client(get_proxy(), msg);
	}

public:
	void update_proxy(const proxy_info& proxy);

public:
	TProcessID_t get_gate_id() const;
	TSocketIndex_t get_client_id() const;
	const proxy_info& get_proxy() const;

private:
	proxy_info m_proxy;
};

#endif // !_SERVER_ENTITY_H_

