
#ifndef _ROLE_H_
#define _ROLE_H_

#include <vector>

#include "entity.h"
#include "game_struct.h"
#include "packet_struct.h"

class city;

class role : public entity
{
public:
	role();
	virtual ~role();

public:
	bool init() override;
	void update(TGameTime_t diff);

public:
	void login(TPlatformID_t platform_id, const TUserID_t& user_id);
	void login_with_index(TSocketIndex_t socket_index, TPlatformID_t platform_id, const TUserID_t& user_id);

public:
	void add_city(const game_pos& pos, TLevel_t lvl);
	void del_city(const game_pos& pos);

public:
	TLevel_t get_level() const;
	void add_level(TLevel_t lvl);

	THonorNum_t get_honor() const;
	void add_honor(THonorNum_t num);

	TRmbNum_t get_rmb() const;
	void add_rmb(TRmbNum_t num);

public:
	void set_server_id(TServerID_t server_id);
	void set_gate_id(TProcessID_t gate_id);
	void set_game_id(TProcessID_t game_id);
	void set_client_id(TSocketIndex_t client_id);
	void set_role_id(TRoleID_t role_id);

public:
	const proxy_info& get_proxy_info() const;
	const mailbox_info& get_mailbox_info() const;

private:
	void clean_up();

public:
	proxy_info m_proxy_info;
	mailbox_info m_mailbox_info;
	TPlatformID_t m_platform_id;
	TUserID_t m_user_id;
	TLevel_t m_lvl;
	THonorNum_t m_honor;
	TRmbNum_t m_rmb;
	std::vector<city*> m_cities;
};

#endif // !_ROLE_H_

