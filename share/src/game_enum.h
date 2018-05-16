
#ifndef _GAME_ENUM_H_
#define _GAME_ENUM_H_

// ��������
enum game_process_type
{
	PROCESS_GATE = 1,
	PROCESS_GAME,
	PROCESS_DB,
	PROCESS_GAME_MANAGER,
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

enum entity_type
{
	ENTITY_ROLL_STUB = 1,
};

enum attr_type
{
	ATTR_AFFAIRS = 1,
	ATTR_FEAT = 2,
	ATTR_STRATEGY = 3,
	ATTR_LEAD = 4,
};

enum resource_type
{
	RESOURCE_FOOD = 1,
	RESOURCE_WOOD,
	RESOURCE_STONE,
	RESOURCE_IRON,
	RESOURCE_GOLD,
	RESOURCE_RMB,

	RESOURCE_HOUSE_GOVERNMENT,
	RESOURCE_HOUSE_WALL,
	RESOURCE_HOUSE_LIVE,
	RESOURCE_HOUSE_RECRUIT,
	RESOURCE_HOUSE_SOLDIER,
	RESOURCE_HOUSE_MARKET,
	RESOURCE_HOUSE_WARNING,
	RESOURCE_HOUSE_DRILL_GROUND,
	RESOURCE_HOUSE_UNION,
	RESOURCE_HOUSE_GRANARY,
	RESOURCE_HOUSE_TECHNOLOGY,
	RESOURCE_HOUSE_BLACKSMITH,

};

#endif // !_GAME_ENUM_H_
