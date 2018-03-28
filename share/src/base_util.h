
#ifndef _BASE_UTIL_H_
#define _BASE_UTIL_H_

#include "types_def.h"

/// ���Ͷ���
typedef uint32 TThreadID_t;             // �߳�
static const TThreadID_t INVALID_THREAD_ID = 0;

typedef uint64 TSocketIndex_t;          // Ψһ��ʶ
const TSocketIndex_t INVALID_UNIQUE_INDEX = INVALID_UINT64_NUM;

static const uint32 LOG_SINGLE_FILE_SIZE = 5 * 1024 * 1024;

static const uint32 MAX_PACKET_READ_SIZE = 128;			// һ�ζ�ȡ�����ݵ��ֽ���
static const uint32 MAX_PACKET_BUFFER_SIZE = 2048;		// 196608 = 65536 * 3
static const uint32 MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE = 1024;	// MAX_PACKET_BUFFER_SIZE - 65535(�����������ֽ���) - MAX_PACKET_READ_SIZE, ��Ҫע��MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE + MAX_PACKET_READ_SIZE  * 2 < MAX_PACKET_BUFFER_SIZE

typedef uint32 TTime_t;					// ϵͳʱ��
typedef uint32 TAppTime_t;				// �������еĺ�����

typedef uint16 TPacketLen_t;			// Э��ĳ���
const TPacketLen_t INVALID_PACKET_LEN = 0;

typedef uint8 TPacketID_t;				// Э���id
const TPacketID_t INVALID_PACKET_ID = 0;

typedef uint32 TSeedType_t;				// �������
const TSeedType_t INVALID_SEED_TYPE = 0;
static const TSeedType_t RandomMax = 0xFFFFFFFFLU;

typedef uint16	TRpcIndex_t;			// rpc����
const TRpcIndex_t INVALID_RPC_INDEX = 0;

typedef uint8	TParamIndex_t;			// ��������
const TParamIndex_t INVALID_PARAM_INDEX = 0;

//����ָ��ֵɾ���ڴ�
#ifndef DSafeDelete
#define DSafeDelete(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif

//����ָ��ֵɾ�����������ڴ�
#ifndef DSafeDeleteArray
#define DSafeDeleteArray(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif

#endif