
#ifndef _GAME_MANAGER_H_
#define _GAME_MANAGER_H_

#include "singleton.h"
#include "service.h"

class game_manager : public service, public singleton<game_manager>
{
	typedef service TBaseType_t;

public:
	game_manager();
	~game_manager();
	
public:
	bool init(TProcessID_t process_id);

private:
	bool check_all_process_start() const;

	void broadcast_dbs() const;
	void broadcast_db(const game_server_info& server_info) const;
	void broadcast_games() const;
	void broadcast_game(const game_server_info& server_info) const;

private:
	virtual void on_connect(TSocketIndex_t socket_index) override;
	virtual void on_disconnect(TSocketIndex_t socket_index) override;

private:
	TProcessNum_t m_process_num[MAX_PROCESS_TYPE_NUM];
};

#define DGameManager singleton<game_manager>::get_instance()

#endif // !_GAME_MANAGER_H_
