
#ifndef _BASE_UTIL_H_
#define _BASE_UTIL_H_

#include "types_def.h"

/// ���Ͷ���
typedef uint32 TThreadID_t;             // �߳�
static const TThreadID_t INVALID_THREAD_ID = 0;

typedef uint64 TSocketIndex_t;          // Ψһ��ʶ
const TSocketIndex_t INVALID_SOCKET_INDEX = INVALID_UINT64_NUM;

static const uint32 LOG_SINGLE_FILE_SIZE = 5 * 1024 * 1024;

static const sint32 MAX_PACKET_READ_SIZE = 30000;		// һ�ζ�ȡ�����ݵ��ֽ���
static const sint32 MAX_PACKET_BUFFER_SIZE = 196608;	// 196608 = 65536 * 3
static const sint32 MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE = 100000;	// MAX_PACKET_BUFFER_SIZE - 65535(�����������ֽ���) - MAX_PACKET_READ_SIZE, ��Ҫע��MAX_PACKET_BUFFER_MOVE_FORWARD_SIZE + MAX_PACKET_READ_SIZE  * 2 < MAX_PACKET_BUFFER_SIZE

static const uint32 MAX_WEB_SOCKET_OUTPUT_BUFFER_SIZE = 655360; // 10 * 65536, websocket���ͻ����ֽ���

static const uint32 MAX_ATTR_NUM = 6;	// ��������

static const uint32 MAX_RESOURCE_TYPE_NUM = 5;	// ��Դ��������

static const uint32 MAX_SOLDIER_TYPE_NUM = 16;	// ��������

static const uint32 MAX_TECHNOLOGY_NUM = 20;	// �Ƽ���������

static const uint32 USER_ID_LEN = 64;	// �û�ID����

static const uint32 MAX_PROCESS_TYPE_NUM = 4; // ������������

static const uint32 MAX_SEQUENCE_TYPE_NUM = 8;	// ����ID����

static const uint32 MAX_INI_VALUE = 1024;

static const uint32 DELAY_KICK_SOCKET_TIME = 1; // �ӳ�����socket��ʱ��

static const uint32 STUB_NAME_LEN = 32;	// stub����

static const uint64 MAX_RESOURCE_INDEX = 64; // ��������

static const char START_CHARACTER = 0x2; // ��ʼ�ַ���ʶ
static const char END_CHARACTER = 0x3;	 // �����ַ���ʶ

typedef uint32 TTime_t;					// ϵͳʱ��
typedef uint32 TAppTime_t;				// �������еĺ�����

typedef uint16 TPacketLen_t;			// Э��ĳ���
const TPacketLen_t INVALID_PACKET_LEN = 0;

typedef uint8 TPacketID_t;				// Э���id
const TPacketID_t INVALID_PACKET_ID = 0;

typedef uint32 TSeedType_t;				// �������
const TSeedType_t INVALID_SEED_TYPE = 0;
static const TSeedType_t RandomMax = 0xFFFFFFFFLU;

typedef uint16 TRpcIndex_t;				// rpc����
const TRpcIndex_t INVALID_RPC_INDEX = 0;

typedef uint8 TParamIndex_t;			// ��������
const TParamIndex_t INVALID_PARAM_INDEX = 0;

typedef uint16 TServerID_t;				// ����ID
const TServerID_t INVALID_SERVER_ID = 0;

typedef uint8 TProcessType_t;			// �������� (�μ�process_type)
const TProcessType_t INVALID_PROCESS_TYPE = 0;

typedef uint16 TProcessID_t;			// ����id
const TProcessID_t INVALID_PROCESS_ID = 0;

typedef sint16 TProcessNum_t;			// ��������
const TProcessNum_t INVALID_PROCESS_NUM = 0;

typedef uint8 TAttrType_t;				// ��������
const TAttrType_t INVALID_ATTR_TYPE = 0;

typedef sint16 TAttrValue_t;			// ����ֵ
const TAttrValue_t INVALID_ATTR_VALUE = 0;

typedef uint8 TResourceType_t;			// ��Դ����
const TResourceType_t INVALID_RESOURCE_TYPE = 0;

typedef uint8 TResourceIndex_t;			// ��Դ����
const TResourceIndex_t INVALID_RESOURCE_INDEX = INVALID_UINT8_NUM;

typedef sint32 TResourceNum_t;			// ��Դ����
const TResourceNum_t INVALID_RESOURCE_NUM = 0;

typedef sint8 TLevel_t;					// �ȼ�
const TLevel_t INVALID_LEVEL = 0;

typedef uint32 TGameTime_t;				// ��Ϸʱ��
const TGameTime_t INVALID_GAME_TIME = 0;

typedef uint16 TCityIndex_t;			// ��������
const TCityIndex_t INVALID_CITY_INDEX = 0;

typedef uint16 TNpcIndex_t;				// npc����
const TNpcIndex_t INVALID_NPC_INDEX = 0;

typedef sint16 TPosValue_t;				// ����
const TPosValue_t INVALID_POS_VALUE = 0;

typedef uint8 TSoldierType_t;			// ������
const TSoldierType_t INVALID_SOLDIER_TYPE = 0;

typedef sint32 TSoldierNum_t;			// ������
const TSoldierNum_t INVALID_SOLDIER_NUM = 0;

typedef uint8 TConsumeType_t;			// ��������
const TConsumeType_t INVALID_CONSUME_TYPE = 0;

typedef uint8 TTechnologyType_t;		// �Ƽ�����
const TTechnologyType_t INVALID_TECHNOLOGY_TYPE = 0;

typedef sint32 TRmbNum_t;				// Ԫ������
const TRmbNum_t INVALID_RMB_NUM = 0;

typedef sint32 THonorNum_t;				// ����
const THonorNum_t INVALID_HONOR_NUM = 0;

typedef uint8 TPlatformID_t;			// ƽ̨ID
const TPlatformID_t INVALID_PLATFORM_ID = 0;

typedef std::array<char, USER_ID_LEN> TUserID_t;	// �û�ID

typedef std::array<char, STUB_NAME_LEN> TStubName_t;

typedef uint64 TEntityID_t;				// entity id
const TEntityID_t INVALID_ENTITY_ID = 0;

typedef uint64 TRoleID_t;				// ��ɫid
const TRoleID_t INVALID_ROLE_ID = 0;

typedef uint32 TSequenceID_t;			// ����ID
const TSequenceID_t INVALID_SEQUENCE_ID = 0;

typedef uint64 TDbOptID_t;				// db����ID
const TDbOptID_t INVALID_DB_OPT_ID = 0;

typedef uint64 TTimerID_t;				// ��ʱ��ID
const TTimerID_t INVALID_TIMER_ID = 0;

typedef uint8 TDbVersion_t;				// ���ݿ�汾��
const TDbVersion_t INVALID_DB_VERSION = 0;

//����ָ��ֵɾ���ڴ�
#ifndef DSafeDelete
#define DSafeDelete(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif

//����ָ��ֵɾ�����������ڴ�
#ifndef DSafeDeleteArray
#define DSafeDeleteArray(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif

#endif