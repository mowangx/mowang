
#ifndef _DB_STRUCT_H_
#define _DB_STRUCT_H_

#include <map>

#include "base_util.h"

struct db_opt_info
{
	uint8 opt_type;		// 1: select, 2: update, 3: insert, 4: delete
	TOptID_t opt_id;
	TSocketIndex_t socket_index;
	std::string table_name;
	std::string condition; // need split by or and ?
	std::string fields;
	//std::map<std::string, std::string> key_2_value;
	db_opt_info() {
		clean_up();
	}

	void clean_up() {
		opt_type = 0;
		opt_id = INVALID_OPT_ID;
		socket_index = INVALID_SOCKET_INDEX;
		table_name = "";
		condition = "";
		fields = "";
	}
};

#endif // !_DB_STRUCT_H_

