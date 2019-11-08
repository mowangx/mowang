
#include "http_client.h"
#include "string_common.h"
#include "rpc_proxy.h"
#include "rpc_wrapper.h"
#include "tcp_manager.h"
#include "http_packet_handler.h"

http_client::http_client() : service(PROCESS_HTTP_CLIENT)
{
	m_etcd_host = "127.0.0.1";
	m_etcd_port = 2379;
	m_io_timer = new boost::asio::deadline_timer(m_io_service);
	m_wait_release_proxy.clear();
}

http_client::~http_client()
{
}

bool http_client::init(TProcessID_t process_id)
{
	if (!TBaseType_t::init(process_id)) {
		return false;
	}

	DRegisterServerRpc(this, http_client, register_server, 2);
	DRegisterServerRpc(this, http_client, http_request, 6);
	DRegisterServerRpc(this, http_client, register_process_entities, 4);
	DRegisterServerRpc(this, http_client, refresh_ttl, 3);
	DRegisterServerRpc(this, http_client, request_all, 2);
	DRegisterServerRpc(this, http_client, request_one, 3);

	if (!DNetMgr.start_listen<http_packet_handler>(m_server_info.port)) {
		log_info("init socket manager failed");
		return false;
	}

	m_io_timer->expires_at(boost::posix_time::neg_infin);
	log_info("init socket manager success");

	return true;
}

void http_client::do_loop(TGameTime_t diff)
{
	for (auto client : m_wait_release_proxy) {
		if (client->usessl()) {
			m_https_pools.deallocate((https_proxy*)client);
		}
		else {
			m_http_pools.deallocate((http_proxy*)client);
		}
	}
	m_wait_release_proxy.clear();

	m_io_service.poll_one();
	
	TBaseType_t::do_loop(diff);

	if (false) {
		http_proxy_base* c = m_http_pools.allocate(m_io_service);
		c->set_port(m_etcd_port);
		c->start_request("GET", m_etcd_host, "/v2/keys/mw/100", "wait=true&recursive=true&waitIndex=408", [=](int status, const dynamic_string& header, const dynamic_string& body) {
			log_info("aaaaaaaaaaa! %d, %s", status, body.data());
		});
	}
}

void http_client::http_request(TSocketIndex_t socket_index, TOptID_t opt_id, const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl)
{
	try {
		http_proxy_base* c = nullptr;
		if (usessl) {
			boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
			ctx.set_default_verify_paths();

			c = m_https_pools.allocate(m_io_service, ctx);
		}
		else {
			c = m_http_pools.allocate(m_io_service);
		}
		log_info("http request test!!!! %s, %s, %s", host.data(), url.data(), params.data());
		c->start_request("POST", m_etcd_host, url, params, [=](int status, const dynamic_string& header, const dynamic_string& result) {
			m_wait_release_proxy.push_back(c);
			rpc_client* rpc = DRpcWrapper.get_client_by_socket_index(socket_index);
			if (NULL == rpc) {
				return;
			}
			rpc->call_remote_func("on_http_response", opt_id, status, result);
		});
	}
	catch (std::exception& e) {
		log_error("http request failed for %s", e.what());
	}
}

void http_client::register_process_entities(TSocketIndex_t socket_index, const dynamic_string& name, const game_server_info& server_info, const dynamic_array<etcd_entity_info>& entities)
{
	try {
		uint64 key_id = get_etcd_key_id(server_info.process_info);
		dynamic_string url = gx_to_string("/v2/keys/mw/%s/%" I64_FMT "u", name.data(), key_id).c_str();
		std::string body = gx_to_string("{\"ip\":\"%s\",\"port\":%d,\"server_id\":%d,\"process_type\":%d,\"process_id\":%d,\"entities\":[",
			server_info.ip.data(), server_info.port, server_info.process_info.server_id, server_info.process_info.process_type, server_info.process_info.process_id);
		for (int i = 0; i < entities.size(); ++i) {
			if (i > 0) {
				body += ",";
			}
			body += gx_to_string("{\"id\":%" I64_FMT "u,\"name\":\"%s\",\"tag\":\"%s\"}", 
				entities[i].entity_id, entities[i].entity_name.data(), entities[i].tag.data());
		}
		body += "]}";
		dynamic_string params = gx_to_string("value=%s&ttl=30", body.c_str()).c_str();
		http_proxy_base* c = m_http_pools.allocate(m_io_service);
		c->set_port(m_etcd_port);
		log_info("register process entities! %s, %s, %s", url.data(), name.data(), params.data());
		c->start_request("PUT", m_etcd_host, url, params, [=](int status, const dynamic_string& header, const dynamic_string& body) {
			log_info("register entities response! status: %d, body: %s", status, body.data());
			m_wait_release_proxy.push_back(c);
		});
	}
	catch (std::exception& e) {
		log_error("http request failed for %s", e.what());
	}
}

void http_client::refresh_ttl(TSocketIndex_t socket_index, const dynamic_string& name, const game_process_info& process_info)
{
	try {
		uint64 key_id = get_etcd_key_id(process_info);
		dynamic_string url = gx_to_string("/v2/keys/mw/%s/%" I64_FMT "u", name.data(), key_id).c_str();
		dynamic_string params = "ttl=60&refresh=true";
		http_proxy_base* c = m_http_pools.allocate(m_io_service);
		c->set_port(m_etcd_port);
		c->start_request("PUT", m_etcd_host, url, params, [=](int status, const dynamic_string& header, const dynamic_string& body) {
			log_info("refresh ttl response! status: %d, body: %s", status, body.data());
			m_wait_release_proxy.push_back(c);
		});
	}
	catch (std::exception& e) {
		log_error("http request failed for %s", e.what());
	}
}

void http_client::request_all(TSocketIndex_t socket_index, const dynamic_string& name)
{
	try {
		dynamic_string url = gx_to_string("/v2/keys/mw/%s", name.data()).c_str();
		dynamic_string params = "recursive=true";
		http_proxy_base* c = m_http_pools.allocate(m_io_service);
		c->set_port(m_etcd_port);
		c->start_request("GET", m_etcd_host, url, params, [=](int status, const dynamic_string& header, const dynamic_string& body) {
			log_info("request all response! stauts: %d, body: %s", status, body.data());
			m_wait_release_proxy.push_back(c);
			parse_nodes(socket_index, name, header, body);
		});
	}
	catch (std::exception& e) {
		log_error("http request failed for %s", e.what());
	}
}

void http_client::request_one(TSocketIndex_t socket_index, const dynamic_string& name, TWaitIndex_t wait_index)
{
	try {
		dynamic_string url = gx_to_string("/v2/keys/mw/%s/?wait=true&recursive=true&waitIndex=%u", name.data(), wait_index + 1).c_str();
		http_proxy_base* c = m_http_pools.allocate(m_io_service);
		c->set_port(m_etcd_port);
		c->start_request("GET", m_etcd_host, url, "", [=](int status, const dynamic_string& header, const dynamic_string& body) {
			log_info("request one response! status: %d, body: %s", status, body.data());
			m_wait_release_proxy.push_back(c);
			rpc_client* rpc = DRpcWrapper.get_client_by_socket_index(socket_index);
			if (NULL == rpc) {
				log_error("request one success but can not find socket index! socket index %" I64_FMT "u", socket_index);
				return;
			}
			boost::property_tree::ptree body_json;
			if (!parse_json(body_json, body.data())) {
				log_error("request one success but parse json failed! socket index %" I64_FMT "u", socket_index);
				return;
			}
			std::string action = body_json.get<std::string>("action");
			if (action == "set" || action == "create" || action == "delete" || action == "expire") {
				boost::property_tree::ptree node_info = body_json.get_child("node");
				if (node_info.empty()) {
					log_error("request one success but node info is empty! url: %s, request one node is empty!", url.data());
					return;
				}
				TWaitIndex_t modify_index = node_info.get<TWaitIndex_t>("modifiedIndex", 0);
				if (action == "set" || action == "create") {
					dynamic_array<etcd_process_packet_info> processes;
					dynamic_array<etcd_entity_packet_info> entities;
					parse_node_info(processes, entities, node_info);
					rpc->call_remote_func("on_register_entities", name, modify_index, processes, entities, true);
				}
				else {
					std::string key = node_info.get<std::string>("key");
					std::string::size_type idx = key.find_last_of("/");
					if (idx == std::string::npos) {
						log_error("request one success but parse key failed!socket index %" I64_FMT "u, key %s", socket_index, key.c_str());
						return;
					}
					game_process_info process_info;
					uint64 key_id = std::atoll(key.substr(idx + 1).data());
					parse_etcd_key_id(process_info, key_id);
					rpc->call_remote_func("on_unregister_process", name, modify_index, process_info);
					log_info("request one success!socket index %" I64_FMT "u, key %s", socket_index, key.c_str());
				}
			}
			else if (body_json.get<int>("errorCode", 0) == 401) {

			}
			else {

			}
		});
	}
	catch (std::exception& e) {
		log_error("http request failed for %s", e.what());
	}
}

void http_client::parse_nodes(TSocketIndex_t socket_index, const dynamic_string& name, const dynamic_string& header, const dynamic_string& body)
{
	rpc_client* rpc = DRpcWrapper.get_client_by_socket_index(socket_index);
	if (NULL == rpc) {
		log_error("parse nodes failed for not find client by socket index! socket index %" I64_FMT "u", socket_index);
		return;
	}
	if (header.size() == 0) {
		log_error("parse nodes failed for header is empty!");
		return;
	}
	if (body.size() == 0) {
		log_error("parse nodes failed for body is empty!");
		return;
	}
	//boost::property_tree::ptree header_json;
	//if (!parse_json(header_json, header.data())) {
	//	return;
	//}
	//TWaitIndex_t wait_index = header_json.get<TWaitIndex_t>("X-Etcd-Index");

	TWaitIndex_t wait_index = INVALID_WAIT_INDEX;
	char* p = std::strtok(const_cast<char*>(header.data()), "\r\n");
	do {
		std::string line = p;
		std::string::size_type index = line.find(':', 0);
		if (index == std::string::npos) {
			continue;
		}
		std::string key = line.substr(0, index);
		if (key != "X-Etcd-Index") {
			continue;
		}
		std::string value = line.substr(index + 1);
		wait_index = std::atoi(value.data());
	} while ((p = std::strtok(NULL, "\r\n")));

	boost::property_tree::ptree body_json;
	if (!parse_json(body_json, body.data())) {
		return;
	}
	dynamic_array<etcd_process_packet_info> processes;
	dynamic_array<etcd_entity_packet_info> entities;
	if (body_json.get<std::string>("action", "") == "get") {
		boost::property_tree::ptree node = body_json.get_child("node");
		boost::property_tree::ptree nodes = node.get_child("nodes");
		for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {
			boost::property_tree::ptree node_info = itr->second;
			parse_node_info(processes, entities, node_info);
		}
		rpc->call_remote_func("on_register_entities", name, wait_index, processes, entities);
	}
	else if (body_json.get<int>("errorCode", 0) == 100) {

	}
	else {
		log_error("request all from etcd failed! header: %s, body: %s", header.data(), body.data());
	}
}

void http_client::parse_node_info(dynamic_array<etcd_process_packet_info>& processes, dynamic_array<etcd_entity_packet_info>& entities, const boost::property_tree::ptree& node_info) const
{
	std::string key = node_info.get<std::string>("key");
	std::string process_str = node_info.get<std::string>("value");
	boost::property_tree::ptree process_json;
	if (!parse_json(process_json, process_str.c_str())) {
		return;
	}
	etcd_process_packet_info process_info;
	std::string ip = process_json.get<std::string>("ip");
	memset(process_info.server_info.ip.data(), 0, IP_LEN);
	memcpy(process_info.server_info.ip.data(), ip.data(), ip.size());
	process_info.server_info.port = process_json.get<TPort_t>("port");
	process_info.server_info.process_info.server_id = process_json.get<TServerID_t>("server_id");
	process_info.server_info.process_info.process_type = process_json.get<TProcessType_t>("process_type");
	process_info.server_info.process_info.process_id = process_json.get<TProcessID_t>("process_id");
	bool process_exist_flag = false;
	for (int i = 0; i < processes.size(); ++i) {
		if (processes[i].server_info.process_info == process_info.server_info.process_info) {
			process_exist_flag = true;
			break;
		}
	}
	if (!process_exist_flag) {
		process_info.process_id = processes.size() + 1;
		processes.push_back(process_info);
	}
	boost::property_tree::ptree entities_json = process_json.get_child("entities");
	for (auto itr = entities_json.begin(); itr != entities_json.end(); ++itr) {
		boost::property_tree::ptree entity_json = itr->second;
		etcd_entity_packet_info entity_info;
		entity_info.process_id = process_info.process_id;
		entity_info.entity_info.entity_id = entity_json.get<TEntityID_t>("id");
		std::string name = entity_json.get<std::string>("name");
		memset(entity_info.entity_info.entity_name.data(), 0, ENTITY_NAME_LEN);
		memcpy(entity_info.entity_info.entity_name.data(), name.data(), name.size());
		std::string tag = entity_json.get<std::string>("tag");
		memset(entity_info.entity_info.tag.data(), 0, TAG_NAME_LEN);
		memcpy(entity_info.entity_info.tag.data(), tag.data(), tag.size());
		entities.push_back(entity_info);
	}
}

bool http_client::parse_json(boost::property_tree::ptree& json, const char* result) const
{
	std::stringstream json_stream(result);
	try {
		boost::property_tree::read_json(json_stream, json);
		return true;
	}
	catch (const boost::property_tree::ptree_error& e) {
		log_error("parse json failed! error %s, %s", e.what(), result);
	}
	return false;
}

uint64 http_client::get_etcd_key_id(const game_process_info & process_info) const
{
	uint64 key_id = process_info.server_id;
	key_id = (key_id << sizeof(process_info.process_type) * 8) + process_info.process_type;
	key_id = (key_id << sizeof(process_info.process_id) * 8) + process_info.process_id;
	return key_id;
}

void http_client::parse_etcd_key_id(game_process_info & process_info, uint64 key_id) const
{
	process_info.process_id = key_id & 0xFFFF;
	key_id >>= (sizeof(TProcessID_t) * 8);
	process_info.process_type = key_id & 0xFF;
	key_id >>= (sizeof(TProcessType_t) * 8);
	process_info.server_id = key_id & 0xFFFF;
}
