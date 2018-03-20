
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
typedef struct timeval		TTimeVal_t;      // 时间
typedef struct event        TSocketEvent_t;
typedef struct event_base   SocketEventBase_t;


static const sint32 SOCKET_ERROR_WOULDBLOCK = -100;                     // 阻塞
static const uint32 SOCKADDR_IN_LEN = sizeof(SOCKADDR_IN);              // 地址长度

/// 网络错误码定义
#ifdef OS_WINDOWS
#define gx_socket_errno WSAGetLastError()
#elif defined(OS_UNIX)
#define gx_socket_errno errno
#endif

//初始化的接收缓存长度
#define DEFAULT_SOCKET_INPUT_BUFFER_SIZE 64*1024
//最大可以允许的缓存长度，如果超过此数值，则断开连接
#define DISCONNECT_SOCKET_INPUT_SIZE 10*1024*1024
// 警告值
#define WARNING_SOCKET_INPUT_SIZE 5*1024*1024

//初始化的发送缓存长度
#define DEFAULT_SOCKET_OUTPUT_BUFFER_SIZE 64*1024
//最大可以允许的缓存长度，如果超过此数值，则断开连接
#define DISCONNECT_SOCKET_OUTPUT_SIZE 10*1024*1024
// 警告值
#define WARNING_SOCKET_OUTPUT_SIZE 5*1024*1024

// 服务器socket接收缓存长度
#define SERVER_SOCKET_INPUT_BUFFER_SIZE 50*1024*1024
// 服务器socket接收缓存最大可以允许长度, 如果超过此数值, 则断开连接
#define DISCONNECT_SERVER_SOCKET_INPUT_SIZE 200*1024*1024

// 服务器socket发送缓存长度
#define SERVER_SOCKET_OUTPUT_BUFFER_SIZE 50*1024*1024
// 服务器socket发送缓存最大可以允许长度, 如果超过此数值, 则断开连接
#define DISCONNECT_SERVER_SOCKET_OUTPUT_SIZE 200*1024*1024

// @todo 等待socket关闭的时间
#define DEFAULT_CLOSE_SOCKET_WAIT_SECS 0

// @todo 写成配置文件
#define PROFILE_SOCKET_SECONDS 10
#define PROFILE_LOOP_SOCKET_SECONDS 1

// 一帧发送数据的缓冲区最大长度
#define SOCKET_HANDLER_BUFF_LEN 1024*100

// 每次Accept时接收的最大连接数
#define ACCEPT_ONCE_NUM 50

// 最大可以等待的时间
#define MAX_CLOSE_SOCK_WAIT_SECS 20

// 最大的Socket线程个数
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