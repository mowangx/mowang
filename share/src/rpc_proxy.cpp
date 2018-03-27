
#include "rpc_proxy.h"

CRpcProxy::CRpcProxy()
{
	clean_up();
}

CRpcProxy::~CRpcProxy()
{
	clean_up();
}

void CRpcProxy::call(const std::string& func_name, char* buffer)
{
	auto itr = m_funcs.find(func_name.c_str());
	if (itr != m_funcs.end()) {
		itr->second(buffer);
	}
}

void CRpcProxy::clean_up()
{
	m_funcs.clear();
}