
#ifndef _BASE_UTIL_H_
#define _BASE_UTIL_H_

#include "types_def.h"

/// 类型定义
typedef uint32 TThreadID_t;             // 线程
static const TThreadID_t INVALID_THREAD_ID = 0;

typedef uint64 TSocketIndex_t;          // 唯一标识
const TSocketIndex_t INVALID_UNIQUE_INDEX = INVALID_UINT64_NUM;

static const uint32 LOG_SINGLE_FILE_SIZE = 5 * 1024 * 1024;

static const uint32 MAX_PACKET_READ_SIZE = 128;			// 一次读取的数据的字节数
static const uint32 MAX_PACKET_BUFFER_SIZE = 2048;		// 196608 = 65536 * 3
static const uint32 MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE = 1024;	// MAX_PACKET_BUFFER_SIZE - 65535(单个包最大的字节数) - MAX_PACKET_READ_SIZE, 需要注意MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE + MAX_PACKET_READ_SIZE  * 2 < MAX_PACKET_BUFFER_SIZE

typedef uint32 TTime_t;					// 系统时间
typedef uint32 TAppTime_t;				// 程序运行的毫秒数

typedef uint16 TPacketLen_t;			// 协议的长度
const TPacketLen_t INVALID_PACKET_LEN = 0;

typedef uint8 TPacketID_t;				// 协议的id
const TPacketID_t INVALID_PACKET_ID = 0;

typedef uint32 TSeedType_t;				// 随机种子
const TSeedType_t INVALID_SEED_TYPE = 0;
static const TSeedType_t RandomMax = 0xFFFFFFFFLU;

typedef uint16	TRpcIndex_t;			// rpc索引
const TRpcIndex_t INVALID_RPC_INDEX = 0;

typedef uint8	TParamIndex_t;			// 参数索引
const TParamIndex_t INVALID_PARAM_INDEX = 0;

//根据指针值删除内存
#ifndef DSafeDelete
#define DSafeDelete(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif

//根据指针值删除数组类型内存
#ifndef DSafeDeleteArray
#define DSafeDeleteArray(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif

#endif