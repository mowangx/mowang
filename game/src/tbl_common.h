
#ifndef _TBL_COMMON_H_
#define _TBL_COMMON_H_

#include <map>
#include "types_def.h"
#include "xml_loader.h"
#include "singleton.h"

class tbl_common_config
{
public:
	bool on_after_load() { return true; }

public:
	uint32 get_key() { return id; }

public:
	int id;
	std::vector<TResourceNum_t> value;
};

class tbl_common_loader : public config_loader<tbl_common_config>, public singleton<tbl_common_loader>
{
public:
	virtual bool read_row(ConfigRow* row, sint32 count, tbl_common_config* common)
	{
		std::string field_names = "";
		if (!load_field_int(common->id, row, "id")) {
			field_names += " id ";
		}

		if (!load_field_ary<TResourceNum_t>(common->value, row, "value", ",")) {
			field_names += " value ";
		}

		if (field_names.empty() && add(common)) {
			return true;
		}

		log_error("read common config, feild names: %s failed! line: %d", field_names.c_str(), count);
		return false;
	}
};

#define DTblCommonMgr	singleton<tbl_common_loader>::get_instance()

#endif // !_TBL_COMMON_H_

