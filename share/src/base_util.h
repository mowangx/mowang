
#ifndef _BASE_UTIL_H_
#define _BASE_UTIL_H_

#include "types_def.h"

/// 类型定义
typedef uint32 TThreadID_t;             // 线程
static const TThreadID_t INVALID_THREAD_ID = 0;

typedef uint64 TUniqueIndex_t;          // 唯一标识
const TUniqueIndex_t INVALID_UNIQUE_INDEX = INVALID_UINT64_NUM;

static const uint32 LOG_SINGLE_FILE_SIZE = 5 * 1024 * 1024;

typedef uint32 TTime_t;					// 系统时间
typedef uint32 TAppTime_t;				// 程序运行的毫秒数

//根据指针值删除内存
#ifndef DSafeDelete
#define DSafeDelete(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif

//根据指针值删除数组类型内存
#ifndef DSafeDeleteArray
#define DSafeDeleteArray(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif

#endif