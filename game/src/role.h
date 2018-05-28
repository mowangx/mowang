
#ifndef _ROLE_H_
#define _ROLE_H_

#include <vector>

#include "entity.h"
#include "game_struct.h"
#include "packet_struct.h"
#include "dynamic_array.h"

class city;

class role : public entity
{
public:
	role();
	virtual ~role();

public:
	virtual bool init() override;

public:
	void login(TPlatformID_t platform_id, const TUserID_t& user_id);
	void logout();
private:
	void logout_core(bool need_unregister_flag);

public:
	void on_register_account(bool status, const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id);
	void on_register_role(bool status, const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id);
	void on_relay_logout();

private:
	void create_role();
	void on_load_account_callback(bool status, const dynamic_string_array2& result);
	void on_load_role_callback(bool status, const dynamic_string_array2& result);
	void on_relay_success(const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id);
	void on_account_login_success();
	void on_role_login_success();

public:
	void fight(TNpcIndex_t npc_id, dynamic_array<soldier_info>& soldiers, const game_pos& src_pos, const game_pos& dest_pos);
	void gather(TNpcIndex_t npc_id, dynamic_array<soldier_info>& soldiers, const game_pos& src_pos, const game_pos& dest_pos);

public:
	void add_city(const game_pos& pos, TLevel_t lvl);
	void del_city(const game_pos& pos);
	city* get_city(const game_pos& pos);

public:
	void set_login_success(bool login_flag);
	bool get_login_success() const;

	void set_destroy_flag(bool destroy_flag);
	bool get_destroy_flag() const;

	TLevel_t get_level() const;
	void add_level(TLevel_t lvl);

	THonorNum_t get_honor() const;
	void add_honor(THonorNum_t num);

	TRmbNum_t get_rmb() const;
	void add_rmb(TRmbNum_t num);

public:
	void set_test_client_id(TSocketIndex_t client_id);
	TSocketIndex_t get_test_client_id() const;

	void set_server_id(TServerID_t server_id);
	TServerID_t get_server_id() const;

	void set_gate_id(TProcessID_t gate_id);
	TProcessID_t get_gate_id() const;
	
	void set_game_id(TProcessID_t game_id);
	TProcessID_t get_game_id() const;
	
	void set_client_id(TSocketIndex_t client_id);
	TSocketIndex_t get_client_id() const;

	void set_entity_id(TEntityID_t entity_id);
	TEntityID_t get_entity_id() const;
	
	void set_role_id(TRoleID_t role_id);
	TRoleID_t get_role_id() const;

	void set_platform_id(TPlatformID_t platform_id);
	TPlatformID_t get_platform_id() const;

	void set_user_id(const TUserID_t& user_id);
	const TUserID_t& get_user_id() const;

public:
	const proxy_info& get_proxy_info() const;
	const mailbox_info& get_mailbox_info() const;

private:
	void save();

private:
	void destroy();
	void clean_up();

public:
	bool m_login_success;
	bool m_destroy_flag;
	proxy_info m_proxy_info;
	mailbox_info m_mailbox_info;
	TSocketIndex_t m_test_client_id;
	TPlatformID_t m_platform_id;
	TUserID_t m_user_id;
	TRoleID_t m_role_id;
	TLevel_t m_lvl;
	THonorNum_t m_honor;
	TRmbNum_t m_rmb;
	std::vector<city*> m_cities;
};

#endif // !_ROLE_H_

