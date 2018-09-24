
#ifndef _ROLE_H_
#define _ROLE_H_

#include <vector>

#include "server_entity.h"
#include "game_struct.h"
#include "packet_struct.h"
#include "dynamic_array.h"

class city;

class role : public server_entity
{
	typedef server_entity TBaseType_t;

public:
	role();
	virtual ~role();

public:
	virtual bool init(TServerID_t server_id, TProcessID_t game_id, TEntityID_t entity_id) override;

public:
	void test(uint8 param_1, uint16 param_2);

public:
	void login(TPlatformID_t platform_id, const TTokenID_t& token);
	void logout();
private:
	void logout_core(bool need_unregister_flag);

public:
	void on_register_account(bool status, const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id);
	void on_register_role(bool status, const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id);
	void on_relay_logout();

private:
	void create_role();
	void on_load_account_callback(bool status, const binary_data& result);
	void on_load_role_callback(bool status, const binary_data& result);
	void on_relay_success(const proxy_info& proxy, const mailbox_info& mailbox, TSocketIndex_t test_client_id);
	void on_account_login_success();
	void on_role_login_success();

public:
	void enter_random();
	void create_room(const dynamic_string& pwd);
	void on_create_room(TEntityID_t entity_id, TRoomID_t room_id);
	void enter_room(TRoomID_t room_id, const dynamic_string& pwd);
	void on_enter_room(const mailbox_info& mailbox);

public:
	void ready_start();
	void pop_cards(const dynamic_array<TCardIndex_t>& cards);

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

	TSex_t get_sex() const;
	void set_sex(TSex_t sex);

	THonorNum_t get_honor() const;
	void add_honor(THonorNum_t num);

	TRmbNum_t get_rmb() const;
	void add_rmb(TRmbNum_t num);

public:
	void set_test_client_id(TSocketIndex_t client_id);
	TSocketIndex_t get_test_client_id() const;

	void set_role_name(const TRoleName_t& role_name);
	const TRoleName_t& get_role_name() const;
	
	void set_role_id(TRoleID_t role_id);
	TRoleID_t get_role_id() const;

	void set_platform_id(TPlatformID_t platform_id);
	TPlatformID_t get_platform_id() const;

	void set_user_id(const TUserID_t& user_id);
	const TUserID_t& get_user_id() const;

private:
	void save();

private:
	void destroy();
	void clean_up();

public:
	bool m_login_success;
	bool m_destroy_flag;
	TSocketIndex_t m_test_client_id;
	TPlatformID_t m_platform_id;
	TUserID_t m_user_id;
	TRoleID_t m_role_id;
	TLevel_t m_level;
	TSex_t m_sex;
	THonorNum_t m_honor;
	TRmbNum_t m_rmb;
	TRoleName_t m_role_name;
	mailbox_info m_room_mailbox;
	std::vector<city*> m_cities;
	std::vector<TNpcIndex_t> m_fight_npc_ids;	// don't need save db
};

#endif // !_ROLE_H_

