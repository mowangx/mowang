
#ifndef _GAME_STRUCT_H_
#define _GAME_STRUCT_H_

#include "base_util.h"

struct role_info
{
	TRoleID_t role_id;
	TAccountID_t account_id;
	TLevel_t level;
	TSex_t sex;
	TRoleName_t role_name;
	role_info() {
		clean_up();
	}

	void clean_up() {
		role_id = INVALID_ROLE_ID;
		account_id = INVALID_ACCOUNT_ID;
		level = INVALID_LEVEL;
		sex = INVALID_SEX;
		memset(role_name.data(), 0, ROLE_NAME_LEN);
	}
};


#endif // !_GAME_STRUCT_H_

