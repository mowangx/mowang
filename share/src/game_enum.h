
#ifndef _GAME_ENUM_H_
#define _GAME_ENUM_H_

enum tls_mode {
	MOZILLA_INTERMEDIATE = 1,
	MOZILLA_MODERN = 2
};

// 进程类型
enum game_process_type
{
	PROCESS_GATE = 1,
	PROCESS_GAME,
	PROCESS_DB,
	PROCESS_GAME_MANAGER,
	PROCESS_HTTP_CLIENT,
	PROCESS_ROBOT,
};

enum logout_type
{
	LOGOUT_RELAY = 1,
};

enum db_operation_type
{
	DB_OPT_QUERY = 1,
	DB_OPT_UPDATE,
	DB_OPT_INSERT,
	DB_OPT_DELETE,
};

enum sequence_type
{
	SEQUENCE_ACCOUNT = 1,
	SEQUENCE_ROLE = 2,
};

enum entity_type
{
	ENTITY_ROLL_STUB = 1,
};

#endif // !_GAME_ENUM_H_
