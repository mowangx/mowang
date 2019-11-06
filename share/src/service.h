
#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <vector>
#include <map>
#include <unordered_map>

#include "service_interface.h"
#include "server_enum.h"
#include "memory_pool.h"
#include "dynamic_array.h"
#include "service_config.h"
#include "etcd_struct.h"

class rpc_client;

class service : public service_interface
{
public:
	service(game_process_type process_type);
	virtual ~service();

public:
	virtual void start(const std::string& module_name, const char* process_id);
public:
	virtual bool load_config(ini_file& ini, const std::string& module_name);
	virtual bool init(TProcessID_t process_id);
	virtual void init_threads();
	virtual void work_run();
	virtual void net_run();
	virtual void log_run();

protected:
	virtual void do_loop(TGameTime_t diff);
	void loop_run(const std::function<bool(TGameTime_t)>& callback);

public:
	virtual bool connect_server(const char* ip, TPort_t port);
private:
	void try_reconnect_server();
protected:
	void loop_connect_http_client();

public:
	virtual const game_server_info& get_server_info() const override;

public:
	virtual packet_send_info * allocate_packet_info() override;
	virtual char* allocate_memory(int n) override;
	virtual void push_write_packets(packet_send_info* packet_info) override;

	virtual void kick_socket(TSocketIndex_t socket_index) override;
	virtual void kick_ws_socket(TSocketIndex_t socket_index) override;

public:
	virtual void register_handler(game_handler* handler) override;
	virtual void unregister_handler(TSocketIndex_t socket_index) override;

public:
	virtual void add_process(const game_server_info& server_info);
	virtual void register_server(TSocketIndex_t socket_index, const game_server_info& server_info) override;

public:
	void http_request(const dynamic_string& host, const dynamic_string& url, const dynamic_string& params, bool usessl, const std::function<void(int, const dynamic_string&)>& callback);
	void on_http_response(TSocketIndex_t socket_index, TOptID_t opt_id, int status, const dynamic_string& result);

public:
	void on_register_entities(TSocketIndex_t socket_index, const dynamic_string& name, TWaitIndex_t wait_index, const dynamic_array<etcd_process_packet_info>& processes, const dynamic_array<etcd_entity_packet_info>& entities);
	void on_unregister_process(TSocketIndex_t socket_index, const dynamic_string& name, TWaitIndex_t wait_index, const game_process_info& process_info);

protected:
	virtual void on_connect(TSocketIndex_t socket_index);
	virtual void on_disconnect(TSocketIndex_t socket_index);

public:
	TServerID_t get_server_id() const;
	TProcessID_t get_game_id() const;

protected:
	TOptID_t m_opt_id;
	TGameTime_t m_reconnect_interval_time;
	TGameTime_t m_next_reconnect_time;
	game_server_info m_server_info;
	service_config m_config;
	obj_memory_pool<packet_send_info, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TSocketIndex_t> m_wait_kick_sockets;
	std::vector<TSocketIndex_t> m_wait_kick_ws_sockets;
	std::vector<packet_send_info*> m_write_packets;
	std::vector<game_server_info> m_disconnect_server_infos;
	std::unordered_map<TSocketIndex_t, game_handler*> m_handlers;
	std::unordered_map<TOptID_t, std::function<void(int, const dynamic_string&)>> m_http_response_callbacks;
};

#endif // !_SERVICE_H_

