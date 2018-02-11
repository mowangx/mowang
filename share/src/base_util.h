
#ifndef _BASE_UTIL_H_
#define _BASE_UTIL_H_

#include "types_def.h"

/// ���Ͷ���
typedef uint32 TThreadID_t;             // �߳�
static const TThreadID_t INVALID_THREAD_ID = 0;

typedef uint64 TUniqueIndex_t;          // Ψһ��ʶ
const TUniqueIndex_t INVALID_UNIQUE_INDEX = INVALID_UINT64_NUM;

static const uint32 LOG_SINGLE_FILE_SIZE = 5 * 1024 * 1024;

typedef uint32 TTime_t;					// ϵͳʱ��
typedef uint32 TAppTime_t;				// �������еĺ�����

//����ָ��ֵɾ���ڴ�
#ifndef DSafeDelete
#define DSafeDelete(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif

//����ָ��ֵɾ�����������ڴ�
#ifndef DSafeDeleteArray
#define DSafeDeleteArray(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif

#endif