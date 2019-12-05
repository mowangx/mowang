
#ifndef _TBL_BUILDING_UPGRADE_H_
#define _TBL_BULIDING_UPGRADE_H_

#include <map>
#include "types_def.h"
#include "xml_loader.h"
#include "singleton.h"

class tbl_building_upgrade_config
{
public:
	bool on_after_load() { return true; }

public:
	uint32 get_key() { return building_level; }

public:
	TLevel_t building_level;
	std::map<TBuildingType_t, std::vector<TResourceNum_t>> upgrade_need_money;
	std::map<TBuildingType_t, std::vector<TResourceNum_t>> product;
	std::map<TBuildingType_t, TGameTime_t> income_interval;
};

class tbl_building_upgrade_loader : public config_loader<tbl_building_upgrade_config>, public singleton<tbl_building_upgrade_loader>
{
public:
	virtual bool read_row(ConfigRow* row, sint32 count, tbl_building_upgrade_config* building)
	{
		std::string field_names = "";
		if (!load_field_int(building->building_level, row, "building_level")) {
			field_names += " building_level ";
		}

		for (int i = 1; i < 100; ++i) {
			std::string building_type_field_name = gx_to_string("building_id%d", i);
			if (nullptr == row->Attribute(building_type_field_name.c_str())) {
				break;
			}

			TBuildingType_t building_type;
			if (!load_field_int(building_type, row, building_type_field_name)) {
				field_names = field_names + " " + building_type_field_name +" ";
			}

			std::string upgrade_field_name = gx_to_string("upgrade_need_money%d", i);
			std::vector<TResourceNum_t> upgrade_ary;
			if (!load_field_ary<TResourceNum_t>(upgrade_ary, row, upgrade_field_name, ";")) {
				field_names = field_names + " " + upgrade_field_name +" ";
			}
			if ((upgrade_ary.size() % 2) > 0) {
				field_names = field_names + " " + upgrade_field_name + " ";
			}
			building->upgrade_need_money[building_type] = upgrade_ary;

			std::string product_field_name = gx_to_string("production_unit%d", i);
			std::vector<TResourceNum_t> product_ary;
			TResourceNum_t resource_num;
			if (!load_field_int(resource_num, row, product_field_name)) {
				field_names = field_names + " " + product_field_name +" ";
			}
			product_ary.push_back(upgrade_ary[0]);
			product_ary.push_back(resource_num);
			building->product[building_type] = product_ary;
			if ((product_ary.size() % 2) > 0) {
				field_names = field_names + " " + product_field_name + " ";
			}

			std::string income_field_name = gx_to_string("income_interval%d", i);
			TGameTime_t income_interval = INVALID_GAME_TIME;
			if (!load_field_int(income_interval, row, income_field_name)) {
				field_names = field_names + " " + income_field_name +" ";
			}
			building->income_interval[building_type] = income_interval;
		}

		if (field_names.empty() && add(building)) {
			return true;
		}

		log_error("read building upgrade config, feild names: %s failed! line: %d", field_names.c_str(), count);
		return false;
	}
};

#define DTblBuildingUpgradeMgr	singleton<tbl_building_upgrade_loader>::get_instance()

#endif // !_TBL_BUILDING_UPGRADE_H_

