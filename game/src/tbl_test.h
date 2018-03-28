
#ifndef _TBL_TEST_H_
#define _TBL_TEST_H_

#include "types_def.h"
#include "xml_loader.h"
#include "singleton.h"

class tbl_test_config
{
public:
	bool on_after_load()	{ return true; }

	uint32 get_key() { return id; }

public:
	uint32	id;
	uint32	value;
};

class tbl_test_loader : public config_loader<tbl_test_config>, public singleton<tbl_test_loader>
{
public:
	virtual bool read_row(ConfigRow* row, sint32 count, tbl_test_config* test)
	{
		int val;
		DReadConfigInt(id, val, test);
		DReadConfigInt(value, val, test);

		if (!add(test)) {
			log_error("read test config failed! line = %d", count);
			return false;
		}

		return true;
	}
};

#define DTblTestMgr	singleton<tbl_test_loader>::get_instance()

#endif