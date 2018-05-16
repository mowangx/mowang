
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
	~game_manager();
	
public:
	bool init(TProcessID_t process_id);
private:
	virtual bool load_config(ini_file& ini, const std::string& module_name) override;
	virtual void net_run() override;

private:
	bool check_all_process_start() const;

	void broadcast_dbs() const;
	void broadcast_db(const game_server_info& server_info) const;
	void broadcast_games() const;
	void broadcast_game(const game_server_info& server_info) const;
private:
	void broadcast_db_core(const dynamic_array<game_server_info>& servers) const;
	void broadcast_game_core(const dynamic_array<game_server_info>& servers) const;
	void unicast_to_game(const game_process_info& process_info) const;
	void unicast_to_gate(const game_process_info& process_info) const;

public:
	void create_entity(TSocketIndex_t socket_index, TServerID_t server_id, const dynamic_string& stub_name);
	void register_entity(TSocketIndex_t socket_index, const dynamic_string& stub_name, const game_process_info& process_info);

private:
	virtual void on_connect(TSocketIndex_t socket_index) override;
	virtual void on_disconnect(TSocketIndex_t socket_index) override;

private:
	bool m_broadcast_flag;
	TProcessNum_t m_process_num[MAX_PROCESS_TYPE_NUM];
	TProcessNum_t m_desire_process_num[MAX_PROCESS_TYPE_NUM];
};

#define DGameManager singleton<game_manager>::get_instance()

#endif // !_GAME_MANAGER_H_
