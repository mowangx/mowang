
#ifndef _GAME_STRUCT_H_
#define _GAME_STRUCT_H_

#include "base_util.h"
#include "dynamic_array.h"

struct resource_info
{
	TResourceType_t resource_type;
	TResourceNum_t resource_num;
	resource_info() {
		clean_up();
	}

	bool operator == (TResourceType_t type) const {
		return resource_type == type;
	}

	void clean_up() {
		resource_type = INVALID_RESOURCE_TYPE;
		resource_num = INVALID_RESOURCE_NUM;
	}
};

struct building_info
{
	TBuildingType_t building_type;
	TBuildingIndex_t building_index;
	TLevel_t building_level;
	resource_info resource_data;
	TGameTime_t start_time;
	TGameTime_t income_time;
	building_info() {
		clean_up();
	}

	bool operator == (TBuildingIndex_t index) const {
		return building_index == index;
	}

	void clean_up() {
		building_type = INVALID_BUILDING_TYPE;
		building_index = INVALID_BUILDING_INDEX;
		building_level = INVALID_LEVEL;
		resource_data.clean_up();
		start_time = INVALID_GAME_TIME;
		income_time = INVALID_GAME_TIME;
	}
};

struct role_info
{
	TRoleID_t role_id;
	TAccountID_t account_id;
	TLevel_t level;
	TSex_t sex;
	TRoleName_t role_name;
	std::vector<building_info> buildings;
	std::vector<resource_info> resources;
	role_info() {
		clean_up();
	}

	role_info& operator = (const role_info& rhs) {
		role_id = rhs.role_id;
		account_id = rhs.account_id;
		level = rhs.level;
		sex = rhs.sex;
		memcpy(role_name.data(), rhs.role_name.data(), ROLE_NAME_LEN);
		buildings = rhs.buildings;
		resources = rhs.resources;
		return *this;
	}

	void clean_up() {
		role_id = INVALID_ROLE_ID;
		account_id = INVALID_ACCOUNT_ID;
		level = INVALID_LEVEL;
		sex = INVALID_SEX;
		memset(role_name.data(), 0, ROLE_NAME_LEN);
		buildings.clear();
		resources.clear();
	}
};


#endif // !_GAME_STRUCT_H_

