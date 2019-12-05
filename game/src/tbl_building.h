
#ifndef _TBL_BUILDING_H_
#define _TBL_BUILDING_H_

#include "types_def.h"
#include "xml_loader.h"
#include "singleton.h"

class tbl_building_config
{
public:
	bool on_after_load() { return true; }

public:
	uint32 get_key() { return building_id; }

public:
	TBuildingIndex_t building_id;
	std::vector<TResourceNum_t> unlock_need_money;
};

class tbl_building_loader : public config_loader<tbl_building_config>, public singleton<tbl_building_loader>
{
public:
	virtual bool read_row(ConfigRow* row, sint32 count, tbl_building_config* building)
	{
		std::string field_names = "";
		if (!load_field_int(building->building_id, row, "building_id")) {
			field_names += " building_id ";
		}

		if (!load_field_ary<TResourceNum_t>(building->unlock_need_money, row, "unlock_need_money", ";")) {
			field_names += " unlock_need_money ";
		}

		if ((building->unlock_need_money.size() % 2) > 0) {
			field_names += " unlock_need_money ";
		}

		if (field_names.empty() && add(building)) {
			return true;
		}

		log_error("read building config, feild names: %s failed! line: %d", field_names.c_str(), count);
		return false;
	}
};

#define DTblBuildingMgr	singleton<tbl_building_loader>::get_instance()

#endif // !_TBL_BUILDING_H_
