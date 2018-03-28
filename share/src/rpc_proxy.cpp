
#include "rpc_proxy.h"

rpc_proxy::rpc_proxy()
{
	clean_up();
}

rpc_proxy::~rpc_proxy()
{
	clean_up();
}

void rpc_proxy::call(uint8 rpc_index, char* buffer)
{
	auto itr = m_index_2_name.find(rpc_index);
	if (itr != m_index_2_name.end()) {
		call(itr->second, buffer);
	}
}

void rpc_proxy::call(const std::string& func_name, char* buffer)
{
	auto itr = m_name_2_func.find(func_name.c_str());
	if (itr != m_name_2_func.end()) {
		itr->second(buffer);
	}
}

void rpc_proxy::clean_up()
{
	m_index_2_name.clear();
	m_name_2_func.clear();
}