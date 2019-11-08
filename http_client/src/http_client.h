
#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "singleton.h"
#include "service.h"
#include "memory_pool.h"
#include "http_proxy.h"
#include "https_proxy.h"
#include "etcd_struct.h"

class http_client : public service, public singleton<http_client>
{
	typedef service TBaseType_t;

public:
	http_client();
	virtual ~http_client() override;

public:
	virtual bool init(TProcessID_t process_id) override;
public:
	virtual void do_loop(TGameTime_t diff) override;

public:
	void http_request(TSocketIndex_t socket_index, TOptID_t opt_id, const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl);

public:
	void register_process_entities(TSocketIndex_t socket_index, const dynamic_string& name, const game_server_info& server_info, const dynamic_array<etcd_entity_info>& entites);
	void refresh_ttl(TSocketIndex_t socket_index, const dynamic_string& name, const game_process_info& process_info);
	void request_all(TSocketIndex_t socket_index, const dynamic_string& name);
	void request_one(TSocketIndex_t socket_index, const dynamic_string& name, TWaitIndex_t wait_index);

private:
	void parse_nodes(TSocketIndex_t socket_index, const dynamic_string& name, const dynamic_string& header, const dynamic_string& body);
	void parse_node_info(dynamic_array<etcd_process_packet_info>& processes, dynamic_array<etcd_entity_packet_info>& entities, const boost::property_tree::ptree& node_info) const;
	bool parse_json(boost::property_tree::ptree& json, const char* result) const;
	uint64 get_etcd_key_id(const game_process_info& process_info) const;
	void parse_etcd_key_id(game_process_info& process_info, uint64 key_id) const;

private:
	std::string m_etcd_host;
	TPort_t m_etcd_port;
	boost::asio::io_service m_io_service;
	boost::asio::deadline_timer* m_io_timer;
	obj_memory_pool<http_proxy, 100> m_http_pools;
	obj_memory_pool<https_proxy, 100> m_https_pools;
	std::vector<http_proxy_base*> m_wait_release_proxy;
};

#define DHttpClient singleton<http_client>::get_instance()

#endif // !_HTTP_CLIENT_H_

