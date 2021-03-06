
#ifndef _BASE_UTIL_H_
#define _BASE_UTIL_H_

#include "types_def.h"

/// 类型定义
typedef uint32 TThreadID_t;             // 线程
static const TThreadID_t INVALID_THREAD_ID = 0;

typedef uint64 TSocketIndex_t;          // 唯一标识
const TSocketIndex_t INVALID_SOCKET_INDEX = INVALID_UINT64_NUM;

static const uint32 IP_LEN = 24; // IP长度

static const uint32 LOG_SINGLE_FILE_SIZE = 5 * 1024 * 1024;

static const sint32 MAX_PACKET_READ_SIZE = 30000;		// 一次读取的数据的字节数
static const sint32 MAX_PACKET_BUFFER_SIZE = 196608;	// 196608 = 65536 * 3
static const sint32 MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE = 100000;	// MAX_PACKET_BUFFER_SIZE - 65535(单个包最大的字节数) - MAX_PACKET_READ_SIZE, 需要注意MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE + MAX_PACKET_READ_SIZE  * 2 < MAX_PACKET_BUFFER_SIZE

static const uint32 MAX_WEB_SOCKET_OUTPUT_BUFFER_SIZE = 655360; // 10 * 65536, websocket发送缓存字节数

static const uint32 MAX_ATTR_NUM = 6;	// 属性数量

static const uint32 USER_ID_LEN = 64;	// 用户ID长度

static const uint32 ROLE_NAME_LEN = 64; // 角色名长度

static const uint32 TOKEN_LEN = 128;	// token长度

static const uint32 NAMESPACE_LEN = 64;	// namespace长度

static const uint32 MAX_PROCESS_TYPE_NUM = 6; // 进程类型数量

static const uint32 MAX_SEQUENCE_TYPE_NUM = 8;	// 序列ID数量

static const uint32 MAX_INI_VALUE = 1024;

static const uint32 DELAY_KICK_SOCKET_TIME = 1; // 延迟清理socket的时间

static const uint32 ENTITY_NAME_LEN = 32;	// entity长度

static const uint32 TAG_NAME_LEN = 32;	// tag长度

static const uint32 MAX_ETCD_ENTITY_NUM = 10;	// 每个进程最多的etcd entity数量

static const char START_CHARACTER = 0x2; // 开始字符标识
static const char END_CHARACTER = 0x3;	 // 结束字符标识

typedef uint32 TTime_t;					// 系统时间
typedef uint32 TAppTime_t;				// 程序运行的毫秒数

typedef uint16 TPacketLen_t;			// 协议的长度
const TPacketLen_t INVALID_PACKET_LEN = 0;

typedef uint8 TPacketID_t;				// 协议的id
const TPacketID_t INVALID_PACKET_ID = 0;

typedef std::array<char, IP_LEN> TIP_t; // ip

typedef uint32 TSeedType_t;				// 随机种子
const TSeedType_t INVALID_SEED_TYPE = 0;
static const TSeedType_t RandomMax = 0xFFFFFFFFLU;

typedef uint16 TRpcIndex_t;				// rpc索引
const TRpcIndex_t INVALID_RPC_INDEX = 0;

typedef uint8 TParamIndex_t;			// 参数索引
const TParamIndex_t INVALID_PARAM_INDEX = 0;

typedef uint16 TServerID_t;				// 区服ID
const TServerID_t INVALID_SERVER_ID = 0;

typedef uint8 TProcessType_t;			// 进程类型 (参见process_type)
const TProcessType_t INVALID_PROCESS_TYPE = 0;

typedef uint16 TProcessID_t;			// 进程id
const TProcessID_t INVALID_PROCESS_ID = 0;

typedef sint16 TProcessNum_t;			// 进程数量
const TProcessNum_t INVALID_PROCESS_NUM = 0;

typedef uint8 TAttrType_t;				// 属性类型
const TAttrType_t INVALID_ATTR_TYPE = 0;

typedef sint16 TAttrValue_t;			// 属性值
const TAttrValue_t INVALID_ATTR_VALUE = 0;

typedef sint16 TLevel_t;				// 等级
const TLevel_t INVALID_LEVEL = 0;

typedef uint32 TGameTime_t;				// 游戏时间
const TGameTime_t INVALID_GAME_TIME = 0;

typedef uint8 TPlatformID_t;			// 平台ID
const TPlatformID_t INVALID_PLATFORM_ID = 0;

typedef std::array<char, USER_ID_LEN> TUserID_t;	// 用户ID

typedef std::array<char, ROLE_NAME_LEN> TRoleName_t; // 角色名

typedef std::array<char, TOKEN_LEN> TTokenID_t;		// token

typedef std::array<char, ENTITY_NAME_LEN> TEntityName_t;

typedef std::array<char, NAMESPACE_LEN> TNamespace_t;

typedef uint64 TEntityID_t;				// entity id
const TEntityID_t INVALID_ENTITY_ID = 0;

typedef std::array<char, TAG_NAME_LEN> TTagName_t;

typedef uint64 TAccountID_t;				// 账号ID
const TAccountID_t INVALID_ACCOUNT_ID = 0;

typedef uint64 TRoleID_t;				// 角色id
const TRoleID_t INVALID_ROLE_ID = 0;

typedef uint8 TSex_t;					// 性别
const TSex_t INVALID_SEX = 0;

typedef uint32 TSequenceID_t;			// 序列ID
const TSequenceID_t INVALID_SEQUENCE_ID = 0;

typedef uint64 TOptID_t;				// 操作ID
const TOptID_t INVALID_OPT_ID = 0;

typedef uint64 TTimerID_t;				// 计时器ID
const TTimerID_t INVALID_TIMER_ID = 0;

typedef uint8 TDbVersion_t;				// 数据库版本号
const TDbVersion_t INVALID_DB_VERSION = 0;

typedef uint32 TWaitIndex_t;			// etcd索引
const TWaitIndex_t INVALID_WAIT_INDEX = 0;

typedef uint16 TBuildingType_t;			// 建筑类型
const TBuildingType_t INVALID_BUILDING_TYPE = 0;

typedef uint16 TBuildingIndex_t;		// 建筑位置
const TBuildingIndex_t INVALID_BUILDING_INDEX = 0;

typedef uint8 TResourceType_t;			// 资源类型
const TResourceType_t INVALID_RESOURCE_TYPE = 0;

typedef sint32 TResourceNum_t;			// 资源数量
const TResourceNum_t INVALID_RESOURCE_NUM = 0;

//根据指针值删除内存
#ifndef DSafeDelete
#define DSafeDelete(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif

//根据指针值删除数组类型内存
#ifndef DSafeDeleteArray
#define DSafeDeleteArray(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif

#endif