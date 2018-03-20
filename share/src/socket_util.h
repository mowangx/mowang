
#ifndef _SOCKET_UTIL_H_
#define _SOCKET_UTIL_H_

#if defined(OS_WINDOWS)
#elif defined(OS_UNIX)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#endif

#include "types_def.h"
#include "event2/event.h"
#include "event.h"


#define _ESIZE 256

#if defined(OS_UNIX)
typedef		sint32	TSocketFD_t;
#define         INVALID_SOCKET   -1
#define		    SOCKET_ERROR	 -1
#elif defined(OS_WINDOWS)
typedef     intptr_t TSocketFD_t;
typedef		sint32	 socklen_t;
#endif

typedef uint16	TPort_t;


#define IP_SIZE			24

typedef unsigned long       TIP_t;
typedef struct sockaddr     SOCKADDR;
typedef struct sockaddr_in  SOCKADDR_IN;
typedef struct timeval		TTimeVal_t;      // ʱ��
typedef struct event        TSocketEvent_t;
typedef struct event_base   SocketEventBase_t;


static const sint32 SOCKET_ERROR_WOULDBLOCK = -100;                     // ����
static const uint32 SOCKADDR_IN_LEN = sizeof(SOCKADDR_IN);              // ��ַ����

/// ��������붨��
#ifdef OS_WINDOWS
#define gx_socket_errno WSAGetLastError()
#elif defined(OS_UNIX)
#define gx_socket_errno errno
#endif

//��ʼ���Ľ��ջ��泤��
#define DEFAULT_SOCKET_INPUT_BUFFER_SIZE 64*1024
//����������Ļ��泤�ȣ������������ֵ����Ͽ�����
#define DISCONNECT_SOCKET_INPUT_SIZE 10*1024*1024
// ����ֵ
#define WARNING_SOCKET_INPUT_SIZE 5*1024*1024

//��ʼ���ķ��ͻ��泤��
#define DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE 64*1024
//����������Ļ��泤�ȣ������������ֵ����Ͽ�����
#define DISCONNECT_SOCKET_OUTPUT_SIZE 10*1024*1024
// ����ֵ
#define WARNING_SOCKET_OUTPUT_SIZE 5*1024*1024

// ������socket���ջ��泤��
#define SERVER_SOCKET_INPUT_BUFFER_SIZE 50*1024*1024
// ������socket���ջ���������������, �����������ֵ, ��Ͽ�����
#define DISCONNECT_SERVER_SOCKET_INPUT_SIZE 200*1024*1024

// ������socket���ͻ��泤��
#define SERVER_SOCKET_OUTPUT_BUFFER_SIZE 50*1024*1024
// ������socket���ͻ���������������, �����������ֵ, ��Ͽ�����
#define DISCONNECT_SERVER_SOCKET_OUTPUT_SIZE 200*1024*1024

// @todo �ȴ�socket�رյ�ʱ��
#define DEFAULT_CLOSE_SOCKET_WAIT_SECS 0

// @todo д�������ļ�
#define PROFILE_SOCKET_SECONDS 10
#define PROFILE_LOOP_SOCKET_SECONDS 1

// һ֡�������ݵĻ�������󳤶�
#define SOCKET_HANDLER_BUFF_LEN 1024*100

// ÿ��Acceptʱ���յ����������
#define ACCEPT_ONCE_NUM 50

// �����Եȴ���ʱ��
#define MAX_CLOSE_SOCK_WAIT_SECS 20

// ����Socket�̸߳���
#define MAX_SOCK_THREAD_NUM 20

class CSocket;
class CSocketWrapper;
class CSocketManager;

typedef struct SocketWrapperEventArg
{
	CSocketWrapper* s;
	CSocketManager* mgr;
	SocketWrapperEventArg() {
		clean_up();
	}

	void clean_up() {
		s = NULL;
		mgr = NULL;
	}
}TSocketWrapperEventArg_t;

typedef struct SocketEventArg
{
	CSocket* s;
	CSocketManager* mgr;
	SocketEventArg() {
		clean_up();
	}

	void clean_up() {
		s = NULL;
		mgr = NULL;
	}

}TSocketEventArg_t;

#endif