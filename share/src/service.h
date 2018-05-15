
#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <vector>
#include <map>
#include <unordered_map>

#include "service_interface.h"
#include "game_enum.h"
#include "memory_pool.h"
#include "dynamic_array.h"
#include "service_config.h"

class rpc_client;

class service : public service_interface
{
public:
	service(game_process_type process_type);
	~service();

public:
	virtual void start(const std::string& module_name, const char* process_id);
protected:
	virtual void work_run(TProcessID_t process_id);
	virtual void net_run(TProcessID_t process_id);
	virtual void log_run();

public:
	virtual bool init(TProcessID_t process_id);
	virtual void run();

protected:
	virtual void do_loop(TGameTime_t diff);
	virtual bool connect_game_manager(const char* ip, TPort_t port);
	void connect_game_manager_loop(const char* ip, TPort_t port);

private:
	void try_reconnect_server();

public:
	virtual const game_server_info& get_server_info() const override;

public:
	virtual TPacketSendInfo_t * allocate_packet_info() override;
	virtual char* allocate_memory(int n) override;
	virtual void push_write_packets(TPacketSendInfo_t* packet_info) override;

	virtual void kick_socket(TSocketIndex_t socket_index) override;

public:
	virtual void register_client(rpc_client* client) override;
	virtual void unregister_client(TSocketIndex_t socket_index) override;

public:
	virtual void register_server(TSocketIndex_t socket_index, const game_server_info& server_info) override;

protected:
	virtual void on_connect(TSocketIndex_t socket_index);
	virtual void on_disconnect(TSocketIndex_t socket_index);

protected:
	void on_register_entity(TSocketIndex_t socket_index, const dynamic_string& stub_name, const game_process_info& process_info);

public:
	rpc_client* get_client(TSocketIndex_t socket_index);

protected:
	TGameTime_t m_reconnect_interval_time;
	TGameTime_t m_next_reconnect_time;
	game_server_info m_server_info;
	service_config m_config;
	obj_memory_pool<TPacketSendInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TSocketIndex_t> m_wait_kick_sockets;
	std::vector<TPacketSendInfo_t*> m_write_packets;
	std::vector<game_server_info> m_disconnect_server_infos;
	std::unordered_map<TSocketIndex_t, rpc_client*> m_clients;
};

#endif // !_SERVICE_H_

