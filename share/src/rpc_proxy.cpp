
#include "rpc_proxy.h"

rpc_proxy::rpc_proxy()
{
	clean_up();
}

rpc_proxy::~rpc_proxy()
{
	clean_up();
}

void rpc_proxy::call(uint8 rpc_index, char* buffer) const
{
	auto itr = m_index_2_name.find(rpc_index);
	if (itr != m_index_2_name.end()) {
		call(itr->second, buffer);
	}
	else {
		log_error("call proxy func failed not find rpc index! rpc index = %u", rpc_index);
	}
}

void rpc_proxy::call(const std::string& func_name, char* buffer) const
{
	auto itr = m_name_2_func.find(func_name.c_str());
	if (itr != m_name_2_func.end()) {
		itr->second(buffer);
	}
	else {
		log_error("call proxy func failed not find func name! func name = %s", func_name.c_str());
	}
}

void rpc_proxy::call_with_index(uint8 rpc_index, char * buffer, TSocketIndex_t socket_index) const
{
	auto itr = m_index_2_name.find(rpc_index);
	if (itr != m_index_2_name.end()) {
		call_with_index(itr->second, buffer, socket_index);
	}
	else {
		log_error("call proxy func failed not find rpc index! rpc index = %u", rpc_index);
	}
}

void rpc_proxy::call_with_index(const std::string & func_name, char * buffer, TSocketIndex_t socket_index) const
{
	auto itr = m_name_2_func_with_index.find(func_name.c_str());
	if (itr != m_name_2_func_with_index.end()) {
		itr->second(buffer, socket_index);
	}
	else {
		log_error("call proxy func failed not find func name! func name = %s", func_name.c_str());
	}
}

void rpc_proxy::clean_up()
{
	m_index_2_name.clear();
	m_name_2_func.clear();
	m_name_2_func_with_index.clear();
}

rpc_stub::rpc_stub()
{
	m_proxy = new rpc_proxy();
}

rpc_stub::~rpc_stub()
{
	if (NULL != m_proxy) {
		delete m_proxy;
	}
	m_proxy = NULL;
}

void rpc_stub::call(uint8 func_index, char * buffer)
{
	m_proxy->call(func_index, buffer);
}

void rpc_stub::call(const std::string & func_name, char * buffer)
{
	m_proxy->call(func_name, buffer);
}

void rpc_stub::call_with_index(uint8 func_index, char * buffer, TSocketIndex_t socket_index)
{
	m_proxy->call_with_index(func_index, buffer, socket_index);
}

void rpc_stub::call_with_index(const std::string & func_name, char * buffer, TSocketIndex_t socket_index)
{
	m_proxy->call_with_index(func_name, buffer, socket_index);
}

rpc_entity::rpc_entity()
{
	m_proxys.clear();
}

rpc_entity::~rpc_entity()
{
	for (auto itr = m_proxys.begin(); itr != m_proxys.end(); ++itr) {
		rpc_proxy* proxy = itr->second;
		if (NULL != proxy) {
			delete proxy;
		}
	}
	m_proxys.clear();
}

void rpc_entity::call(TEntityID_t entity_id, uint8 func_index, char * buffer)
{
	rpc_proxy* proxy = get_proxy(entity_id);
	if (NULL != proxy) {
		proxy->call(func_index, buffer);
	}
	else {
		log_error("call entity func, but not find entity id, entity id %" I64_FMT "u", entity_id);
	}
}

void rpc_entity::call(TEntityID_t entity_id, const std::string & func_name, char * buffer)
{
	rpc_proxy* proxy = get_proxy(entity_id);
	if (NULL != proxy) {
		proxy->call(func_name, buffer);
	}
	else {
		log_error("call entity func, but not find entity id, entity id %" I64_FMT "u", entity_id);
	}
}

rpc_proxy * rpc_entity::get_proxy(TEntityID_t entity_id) const
{
	auto itr = m_proxys.find(entity_id);
	return itr != m_proxys.end() ? itr->second : NULL;
}
