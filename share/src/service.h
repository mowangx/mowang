
#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <vector>

#include "game_enum.h"
#include "socket_manager.h"

class rpc_client;

class service : public service_interface
{
public:
	service(game_process_type process_type);
	~service();

public:
	virtual bool init(TProcessID_t process_id);
	virtual void run();

protected:
	template <class T>
	void connect_server(const char* ip, TPort_t port) {
		while (!DNetMgr.start_connect<T>(ip, port)) {
			log_error("can't connect game manager");
			std::this_thread::sleep_for(std::chrono::seconds(2));
		}
	}

protected:
	virtual void do_loop(TGameTime_t diff);

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

public:
	rpc_client* get_client(TSocketIndex_t socket_index);

protected:
	game_server_info m_server_info;
	obj_memory_pool<TPacketSendInfo_t, 1000> m_packet_pool;
	memory_pool m_mem_pool;
	std::vector<TSocketIndex_t> m_wait_kick_sockets;
	std::vector<TPacketSendInfo_t*> m_write_packets;
	std::vector<game_server_info> m_disconnect_server_infos;
	std::unordered_map<TSocketIndex_t, rpc_client*> m_clients;
};

#endif // !_SERVICE_H_

