
#ifndef _ROLE_H_
#define _ROLE_H_

#include <vector>

#include "server_entity.h"
#include "server_struct.h"
#include "packet_struct.h"
#include "dynamic_array.h"
#include "game_struct.h"

class role : public server_entity
{
	typedef server_entity TBaseType_t;

public:
	role();
	virtual ~role();

public:
	virtual bool init(TEntityID_t entity_id, TProcessID_t gate_id, TSocketIndex_t client_id) override;

public:
	void on_relay_ready(const mailbox_info& mailbox);
	void on_relay_login(const proxy_info& proxy);
	void register_role();
	void on_register_role(bool status);
	void kick_role();
	void disconnect_client();
private:
	void on_login_success();
	void notify_update_resource();

public:
	void income_resource(TBuildingIndex_t index);
private:
	int check_income_resource(TBuildingIndex_t index);
	void on_income_resource(TBuildingIndex_t index);

public:
	void build_building(TBuildingIndex_t index, TBuildingType_t building_type);
private:
	int check_build_building(TBuildingIndex_t index, TBuildingType_t building_type);
	void on_build_building_success(TBuildingIndex_t index, TBuildingType_t building_type);

public:
	void up_building_level(TBuildingIndex_t index);
private:
	int check_up_building_level(TBuildingIndex_t index);
	void on_up_building_level_success(TBuildingIndex_t index);

public:
	bool check_resource(TResourceType_t resource_type, TResourceNum_t resource_num);
	void desc_resource(TResourceType_t resource_type, TResourceNum_t resource_num);
private:
	void on_building_changed(building_info& building_data);
	void get_resource_speed(resource_info& resource_data, TBuildingType_t building_type, TLevel_t level) const;
	void notify_update_building(const building_info& building_data);

public:
	TLevel_t get_level() const;
	void set_level(TLevel_t lvl);
	void add_level(TLevel_t lvl);

	void set_role_data(const role_info& role_data);

public:
	const TRoleName_t& get_role_name() const;
	TAccountID_t get_account_id() const;
	TRoleID_t get_role_id() const;

private:
	void save();

private:
	void logout(bool need_unregister);
	void clean_up();

public:
	uint8 m_status;
	role_info m_role_data;
};

#endif // !_ROLE_H_

