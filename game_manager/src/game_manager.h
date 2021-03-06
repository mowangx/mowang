
#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include "singleton.h"
#include "service.h"
#include "dynamic_array.h"

class game_manager : public service, public singleton<game_manager>
{
	typedef service TBaseType_t;

public:
	game_manager();
	virtual ~game_manager() override;
	
public:
	virtual bool init(TProcessID_t process_id) override;

private:
	bool check_all_process_start() const;

	void broadcast_dbs() const;
	void broadcast_db(const game_server_info& server_info) const;
	void broadcast_games() const;
	void broadcast_game(const game_server_info& server_info) const;
	void broadcast_http_clients() const;
	void broadcast_http_client(const game_server_info& server_info) const;
private:
	void broadcast_core(const dynamic_array<game_server_info>& servers, game_process_type src_type, game_process_type dest_type) const;
	void unicast_to_game(const game_process_info& process_info) const;
	void unicast_to_gate(const game_process_info& process_info) const;
	void unicast_core(rpc_client* rpc, TServerID_t server_id, const std::vector<TProcessType_t>& process_types) const;

public:
	void create_entity(TSocketIndex_t socket_index, TServerID_t server_id, const TEntityName_t& entity_name, TProcessID_t process_id, bool check_repeat);
	void register_entity(TSocketIndex_t socket_index, const TEntityName_t& entity_name, const game_process_info& process_info);

private:
	virtual void on_connect(TSocketIndex_t socket_index) override;
	virtual void on_disconnect(TSocketIndex_t socket_index) override;

private:
	bool m_broadcast_flag;
	TProcessNum_t m_process_num[MAX_PROCESS_TYPE_NUM];
	std::map<std::string, TProcessID_t> m_stub_name_2_process_id;
};

#define DGameManager singleton<game_manager>::get_instance()

#endif // !_GAME_MANAGER_H_
