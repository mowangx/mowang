
#include "stdcore.h"
#include "socket_api.h"
#include "log.h"

#if defined(OS_WINDOWS)
#elif defined(OS_UNIX)
#include <sys/types.h>			// for accept()
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>			// for inet_xxx()
#include <netinet/in.h>
#include <errno.h>				// for errno
#endif

#ifdef OS_UNIX
extern sint32 errno;
#endif

char Error[_ESIZE];

bool SOCKET_API::gx_lib_init()
{
#ifdef OS_WINDOWS
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		printf("socket err = %d", err);
		return false;
	}
#endif
	return true;
}

void SOCKET_API::gx_lib_cleanup()
{
#ifdef OS_WINDOWS
	WSACleanup();
#endif
}

//////////////////////////////////////////////////////////////////////
// exception version of socket()
//
// Parameters
//     domain - AF_INET(internet socket), AF_UNIX(internal socket), ...
//	   type  - SOCK_STREAM(TCP), SOCK_DGRAM(UDP), ...
//     protocol - 0
//
// Return 
//     socket descriptor
//
//
//////////////////////////////////////////////////////////////////////
TSocketIndex_t SOCKET_API::gx_socket(sint32 domain, sint32 type, sint32 protocol)
{
	TSocketIndex_t s = ::socket(domain, type, protocol);

	if (s == INVALID_SOCKET)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EPROTONOSUPPORT:
		case EMFILE:
		case ENFILE:
		case EACCES:
		case ENOBUFS:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}
#elif defined( OS_WINDOWS )
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEAFNOSUPPORT:
			strncpy(Error, "WSAEAFNOSUPPORT", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEMFILE:
			strncpy(Error, "WSAEMFILE", _ESIZE);
			break;
		case WSAENOBUFS:
			strncpy(Error, "WSAENOBUFS", _ESIZE);
			break;
		case WSAEPROTONOSUPPORT:
			strncpy(Error, "WSAEPROTONOSUPPORT", _ESIZE);
			break;
		case WSAEPROTOTYPE:
			strncpy(Error, "WSAEPROTOTYPE", _ESIZE);
			break;
		case WSAESOCKTNOSUPPORT:
			strncpy(Error, "WSAESOCKTNOSUPPORT", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};
#endif
	}

	return s;
}


//////////////////////////////////////////////////////////////////////
// exception version of bind()
//
// Parameters
//     s       - socket descriptor 
//     addr    - socket address structure ( normally struct sockaddr_in )
//     addrlen - length of socket address structure
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_bind(TSocketIndex_t s, const struct sockaddr * addr, sint32 addrlen)
{
	if (bind(s, addr, addrlen) == SOCKET_ERROR)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EADDRINUSE:
		case EINVAL:
		case EACCES:
		case ENOTSOCK:
		case EBADF:
		case EROFS:
		case EFAULT:
		case ENAMETOOLONG:
		case ENOENT:
		case ENOMEM:
		case ENOTDIR:
		case ELOOP:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch
#elif defined( OS_WINDOWS )
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSAESOCKTNOSUPPORT", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEADDRINUSE:
			strncpy(Error, "WSAEADDRINUSE", _ESIZE);
			break;
		case WSAEADDRNOTAVAIL:
			strncpy(Error, "WSAEADDRNOTAVAIL", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAENOBUFS:
			strncpy(Error, "WSAENOBUFS", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch
#endif

		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
// exception version of connect() system call
//
// Parameters
//     s       - socket descriptor
//     addr    - socket address structure
//     addrlen - length of socket address structure
//
// Return
//     none
//
//////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_connect(TSocketIndex_t s, const struct sockaddr * addr, sint32 addrlen)
{
	if (connect(s, addr, addrlen) == SOCKET_ERROR)
	{
#ifdef OS_UNIX
		switch (errno) {
		case EALREADY:
		case EINPROGRESS:
		case ECONNREFUSED:
		case EISCONN:
		case ETIMEDOUT:
		case ENETUNREACH:
		case EADDRINUSE:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch
#elif defined( OS_WINDOWS )
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEADDRINUSE:
			strncpy(Error, "WSAEADDRINUSE", _ESIZE);
			break;
		case WSAEINTR:
			strncpy(Error, "WSAEINTR", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEALREADY:
			strncpy(Error, "WSAEALREADY", _ESIZE);
			break;
		case WSAEADDRNOTAVAIL:
			strncpy(Error, "WSAEADDRNOTAVAIL", _ESIZE);
			break;
		case WSAEAFNOSUPPORT:
			strncpy(Error, "WSAEAFNOSUPPORT", _ESIZE);
			break;
		case WSAECONNREFUSED:
			strncpy(Error, "WSAECONNREFUSED", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAEISCONN:
			strncpy(Error, "WSAEISCONN", _ESIZE);
			break;
		case WSAENETUNREACH:
			strncpy(Error, "WSAENETUNREACH", _ESIZE);
			break;
		case WSAENOBUFS:
			strncpy(Error, "WSAENOBUFS", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		case WSAETIMEDOUT:
			strncpy(Error, "WSAETIMEDOUT", _ESIZE);
			break;
		case WSAEWOULDBLOCK:
			strncpy(Error, "WSAEWOULDBLOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch
#endif
		return false;
	}

	return true;
}

bool SOCKET_API::gx_connect2(TSocketIndex_t sockfd, const struct sockaddr* addr, sint32 addrlen, sint32 timeout)
{
	sint32 fd_num;
	TTimeVal_t select_timeval;
	fd_set readfds, writefds, expfds;

	if (NULL == addr || addrlen <= 0 || timeout < 0) return false;
	if (!gx_setsocketnonblocking(sockfd, true))
	{
		log_error("set noblocking! errno = %u", gx_socket_errno);
		return false;
	}

	if (connect(sockfd, addr, addrlen) == SOCKET_ERROR)
	{
		sint32 err = gx_socket_errno;
		if (err == EINPROGRESS || err == EWOULDBLOCK)
		{
			select_timeval.tv_sec = timeout / 1000;
			select_timeval.tv_usec = (timeout % 1000) * 1000;
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_ZERO(&expfds);

			FD_SET(sockfd, &readfds);
			FD_SET(sockfd, &writefds);
			FD_SET(sockfd, &expfds);
			fd_num = gx_select(sockfd + 1, &readfds, &writefds, &expfds, &select_timeval);
			if (fd_num < 0)
			{
				gx_setsocketnonblocking(sockfd, false);
				log_error("select failed! errno = %u", gx_socket_errno);
				return false;
			}
			else if (0 == fd_num)
			{
				gx_setsocketnonblocking(sockfd, false);
				log_error("time out!");
				return false;
			}

			sint32 ret;
			uint32 len = sizeof(ret);
			if (false == gx_getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &ret, &len))
			{
				gx_setsocketnonblocking(sockfd, false);
				log_error("get connect socket opt error: %d, ret=%u", gx_socket_errno, ret);
				return false;
			}

			if (ret != 0)
			{
				gx_setsocketnonblocking(sockfd, false);
				log_error("connect socket error: %d, ret=%u", gx_socket_errno, ret);
				return false;
			}

			gx_setsocketnonblocking(sockfd, false);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////
// exception version of listen()
//
// Parameters
//     s       - socket descriptor
//     backlog - waiting queue length
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_listen(TSocketIndex_t s, sint32 backlog)
{
	if (listen(s, backlog) == SOCKET_ERROR)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EBADF:
		case ENOTSOCK:
		case EOPNOTSUPP:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch
#elif defined( OS_WINDOWS )
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEADDRINUSE:
			strncpy(Error, "WSAEADDRINUSE", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAEISCONN:
			strncpy(Error, "WSAEISCONN", _ESIZE);
			break;
		case WSAEMFILE:
			strncpy(Error, "WSAEMFILE", _ESIZE);
			break;
		case WSAENOBUFS:
			strncpy(Error, "WSAENOBUFS", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		case WSAEOPNOTSUPP:
			strncpy(Error, "WSAEOPNOTSUPP", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch
#endif

		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
// exception version of accept()
//
// Parameters
//     s       - socket descriptor
//     addr    - socket address structure
//     addrlen - length of socket address structure
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
TSocketIndex_t SOCKET_API::gx_accept(TSocketIndex_t s, struct sockaddr * addr, uint32 * addrlen)
{
#ifdef OS_UNIX
	TSocketIndex_t client = accept(s, addr, (socklen_t*)addrlen);
#elif defined( OS_WINDOWS )
	TSocketIndex_t client = accept(s, addr, (int*)addrlen);
#endif

	if (client == INVALID_SOCKET)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EWOULDBLOCK:
		case ECONNRESET:
		case ECONNABORTED:
		case EPROTO:
		case EINTR:
			// from UNIX Network Programming 2nd, 15.6
			// with nonblocking-socket, ignore above errors
		case EBADF:
		case ENOTSOCK:
		case EOPNOTSUPP:
		case EFAULT:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch
#elif defined( OS_WINDOWS )
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAEINTR:
			strncpy(Error, "WSAEINTR", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAEMFILE:
			strncpy(Error, "WSAEMFILE", _ESIZE);
			break;
		case WSAENOBUFS:
			strncpy(Error, "WSAENOBUFS", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		case WSAEOPNOTSUPP:
			strncpy(Error, "WSAEOPNOTSUPP", _ESIZE);
			break;
		case WSAEWOULDBLOCK:
			strncpy(Error, "WSAEWOULDBLOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch
#endif
	}
	else {
	}

	return client;
}


//////////////////////////////////////////////////////////////////////
// exception version of getsockopt()
//
// Parameters
//     s       - socket descriptor
//     level   - socket option level ( SOL_SOCKET , ... )
//     optname - socket option name ( SO_REUSEADDR , SO_LINGER , ... )
//     optval  - pointer to contain option value
//     optlen  - length of optval
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_getsockopt(TSocketIndex_t s, sint32 level, sint32 optname, void * optval, uint32 * optlen)
{
#ifdef OS_UNIX
	if (getsockopt(s, level, optname, optval, (socklen_t*)optlen) == SOCKET_ERROR)
	{
		switch (errno)
		{
		case EBADF:
		case ENOTSOCK:
		case ENOPROTOOPT:
		case EFAULT:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch

		return false;
	}
#elif defined( OS_WINDOWS )
	if (getsockopt(s, level, optname, (char*)optval, (int*)optlen) == SOCKET_ERROR)
	{
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAENOPROTOOPT:
			strncpy(Error, "WSAENOPROTOOPT", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch

		return false;
	}
#endif

	return true;
}

sint32 SOCKET_API::gx_getsockopt2(TSocketIndex_t s, sint32 level, sint32 optname, void * optval, uint32 * optlen)
{
#ifdef OS_UNIX
	if (getsockopt(s, level, optname, optval, (socklen_t*)optlen) == SOCKET_ERROR)
	{
		switch (errno)
		{
		case EBADF:
			return 1;
		case ENOTSOCK:
			return 2;
		case ENOPROTOOPT:
			return 3;
		case EFAULT:
			return 4;
		default:
			return 5;
		}//end of switch
	}
	return 0;

#elif defined( OS_WINDOWS )
	if (getsockopt(s, level, optname, (char*)optval, (int*)optlen) == SOCKET_ERROR)
	{
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAENOPROTOOPT:
			strncpy(Error, "WSAENOPROTOOPT", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		}
		}

		return iErr;
	}
#endif
	return 0;
}

//////////////////////////////////////////////////////////////////////
// exception version of setsockopt()
//
// Parameters
//     s       - socket descriptor
//     level   - socket option level ( SOL_SOCKET , ... )
//     optname - socket option name ( SO_REUSEADDR , SO_LINGER , ... )
//     optval  - pointer to contain option value
//     optlen  - length of optval
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_setsockopt(TSocketIndex_t s, sint32 level, sint32 optname, const void * optval, sint32 optlen)
{
#ifdef OS_UNIX
	if (setsockopt(s, level, optname, optval, optlen) == SOCKET_ERROR)
	{
		switch (errno)
		{
		case EBADF:
		case ENOTSOCK:
		case ENOPROTOOPT:
		case EFAULT:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch

		return false;
	}
#elif defined( OS_WINDOWS )
	if (setsockopt(s, level, optname, (char*)optval, optlen) == SOCKET_ERROR)
	{
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAENETRESET:
			strncpy(Error, "WSAENETRESET", _ESIZE);
			break;
		case WSAENOPROTOOPT:
			strncpy(Error, "WSAENOPROTOOPT", _ESIZE);
			break;
		case WSAENOTCONN:
			strncpy(Error, "WSAENOTCONN", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch

		return false;
	}
#endif

	return true;
}


//////////////////////////////////////////////////////////////////////
// exception version of send()
// 
// Parameters 
//     s     - socket descriptor
//     buf   - input buffer
//     len   - input data length
//     flags - send flag (MSG_OOB,MSG_DONTROUTE)
// 
// Return 
//     length of bytes sent
// 
// 
//////////////////////////////////////////////////////////////////////
sint32 SOCKET_API::gx_send(TSocketIndex_t s, const void * buf, sint32 len, sint32 flags)
{
	sint32 nSent;
#ifdef OS_UNIX
	nSent = send(s, buf, len, flags);
#elif defined( OS_WINDOWS )
	nSent = send(s, (const char *)buf, len, flags);
#endif

	if (nSent == SOCKET_ERROR)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EWOULDBLOCK:
			//			case EAGAIN :
		{
							return SOCKET_ERROR_WOULDBLOCK;
		}
		case ECONNRESET:
		case EPIPE:
		case EBADF:
		case ENOTSOCK:
		case EFAULT:
		case EMSGSIZE:
		case ENOBUFS:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch
#elif defined( OS_WINDOWS )
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEACCES:
			strncpy(Error, "WSAEACCES", _ESIZE);
			break;
		case WSAEINTR:
			strncpy(Error, "WSAEINTR", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAENETRESET:
			strncpy(Error, "WSAENETRESET", _ESIZE);
			break;
		case WSAENOBUFS:
			strncpy(Error, "WSAENOBUFS", _ESIZE);
			break;
		case WSAENOTCONN:
			strncpy(Error, "WSAENOTCONN", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		case WSAEOPNOTSUPP:
			strncpy(Error, "WSAEOPNOTSUPP", _ESIZE);
			break;
		case WSAESHUTDOWN:
			strncpy(Error, "WSAESHUTDOWN", _ESIZE);
			break;
		case WSAEWOULDBLOCK:
			//			strncpy( Error, "WSAEWOULDBLOCK", _ESIZE ) ;
			return SOCKET_ERROR_WOULDBLOCK;
			break;
		case WSAEMSGSIZE:
			strncpy(Error, "WSAEMSGSIZE", _ESIZE);
			break;
		case WSAEHOSTUNREACH:
			strncpy(Error, "WSAEHOSTUNREACH", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAECONNABORTED:
			strncpy(Error, "WSAECONNABORTED", _ESIZE);
			break;
		case WSAECONNRESET:
			strncpy(Error, "WSAECONNRESET", _ESIZE);
			break;
		case WSAETIMEDOUT:
			strncpy(Error, "WSAETIMEDOUT", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch
#endif
	}
	else if (nSent == 0)
	{
	}

	return nSent;
}


sint32 SOCKET_API::gx_sendto(TSocketIndex_t s, const void * buf, sint32 len, sint32 flags, const struct sockaddr * to, sint32 tolen)
{
#ifdef OS_UNIX
	sint32 nSent = sendto(s, buf, len, flags, to, tolen);
#elif defined( OS_WINDOWS )
	sint32 nSent = sendto(s, (const char *)buf, len, flags, to, tolen);
#endif

	if (nSent == SOCKET_ERROR)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EWOULDBLOCK:
			//			case EAGAIN :
		{
							return SOCKET_ERROR_WOULDBLOCK;
		}
		case ECONNRESET:
		case EPIPE:
		case EBADF:
		case ENOTSOCK:
		case EFAULT:
		case EMSGSIZE:
		case ENOBUFS:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}
#elif defined( OS_WINDOWS )
#endif
	}

	return nSent;
}


//////////////////////////////////////////////////////////////////////
// exception version of recv()
//
// Parameters 
//     s     - socket descriptor
//     buf   - input buffer
//     len   - input data length
//     flags - send flag (MSG_OOB,MSG_DONTROUTE)
// 
// Return 
//     length of bytes received
// 
//
//////////////////////////////////////////////////////////////////////
sint32 SOCKET_API::gx_recv(TSocketIndex_t s, void * buf, sint32 len, sint32 flags)
{
#ifdef OS_UNIX
	sint32 nrecv = recv(s, buf, len, flags);
#elif defined( OS_WINDOWS )
	sint32 nrecv = recv(s, (char*)buf, len, flags);
#endif
	if (nrecv == SOCKET_ERROR)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EWOULDBLOCK:
			//			case EAGAIN:
		{
							return SOCKET_ERROR_WOULDBLOCK;
		}
		case ECONNRESET:
		case EPIPE:
		case EBADF:
		case ENOTCONN:
		case ENOTSOCK:
		case EINTR:
		case EFAULT:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch

#elif defined( OS_WINDOWS )
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSAEWOULDBLOCK:
			return SOCKET_ERROR_WOULDBLOCK;
			break;
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAENOTCONN:
			strncpy(Error, "WSAENOTCONN", _ESIZE);
			break;
		case WSAEINTR:
			strncpy(Error, "WSAEINTR", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAENETRESET:
			strncpy(Error, "WSAENETRESET", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		case WSAEOPNOTSUPP:
			strncpy(Error, "WSAEOPNOTSUPP", _ESIZE);
			break;
		case WSAESHUTDOWN:
			strncpy(Error, "WSAESHUTDOWN", _ESIZE);
			break;
		case WSAEMSGSIZE:
			strncpy(Error, "WSAEMSGSIZE", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAECONNABORTED:
			strncpy(Error, "WSAECONNABORTED", _ESIZE);
			break;
		case WSAETIMEDOUT:
			strncpy(Error, "WSAETIMEDOUT", _ESIZE);
			break;
		case WSAECONNRESET:
			strncpy(Error, "WSAECONNRESET", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch
#endif
	}
	else if (nrecv == 0)
	{
	}

	return nrecv;
}

sint32 SOCKET_API::gx_recvfrom(TSocketIndex_t s, void * buf, sint32 len, sint32 flags, struct sockaddr * from, uint32 * fromlen)
{
#ifdef OS_UNIX
	sint32 nReceived = recvfrom(s, buf, len, flags, from, (socklen_t*)fromlen);

#elif defined( OS_WINDOWS )
	sint32 nReceived = recvfrom(s, (char*)buf, len, flags, from, (int*)fromlen);
#endif

	if (nReceived == SOCKET_ERROR)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EWOULDBLOCK:
			//			case EAGAIN :
		{
							return SOCKET_ERROR_WOULDBLOCK;
		}
		case ECONNRESET:
		case EPIPE:
		case EBADF:
		case ENOTCONN:
		case ENOTSOCK:
		case EINTR:
		case EFAULT:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}//end of switch
#elif defined( OS_WINDOWS )
#endif
	}

	return nReceived;
}


/////////////////////////////////////////////////////////////////////
// exception version of closesocket()
//
// Parameters
//     s - socket descriptor
//
// Return
//     none
//
//
/////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_closesocket(TSocketIndex_t s)
{
	gx_shutdown(s, 0x02);
#ifdef OS_UNIX
	close(s);
#elif defined( OS_WINDOWS )
	if (closesocket(s) == SOCKET_ERROR)
	{
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAEINTR:
			strncpy(Error, "WSAEINTR", _ESIZE);
			break;
		case WSAEWOULDBLOCK:
			strncpy(Error, "WSAEWOULDBLOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};//end of switch

		return false;
	}
#endif

	return true;
}

bool SOCKET_API::gx_ioctlsocket(TSocketIndex_t s, long cmd, unsigned long * argp)
{
#ifdef OS_UNIX
#elif defined( OS_WINDOWS )
	if (ioctlsocket(s, cmd, argp) == SOCKET_ERROR)
	{
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};

		return false;
	}
#endif

	return true;
}


//////////////////////////////////////////////////////////////////////
// check if this socket is nonblocking mode
//
// Parameters
//     s - socket descriptor
//
// Return
//     TRUE if nonblocking, FALSE if blocking
//
//
//////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_getsocketnonblocking(TSocketIndex_t s)
{
#ifdef OS_UNIX
	sint32 flags = FILE_API::gx_fcntl(s, F_GETFL, 0);
	return flags | O_NONBLOCK;
#elif defined( OS_WINDOWS )
	return false;
#endif
}


//////////////////////////////////////////////////////////////////////
// make this socket blocking/nonblocking
//
// Parameters
//     s  - socket descriptor
//     on - TRUE if nonblocking, FALSE if blocking
//
// Return
//     none
//
//////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_setsocketnonblocking(TSocketIndex_t s, bool on)
{
#ifdef OS_UNIX
	sint32 flags = FILE_API::gx_fcntl(s, F_GETFL, 0);
	if (on)
	{
		// make nonblocking fd
		flags |= O_NONBLOCK;
	}
	else
	{
		// make blocking fd
		flags &= ~O_NONBLOCK;
	}

	FILE_API::gx_fcntl(s, F_SETFL, flags);
	return true;
#elif defined( OS_WINDOWS )

	unsigned long argp = (on == true) ? 1 : 0;
	return gx_ioctlsocket(s, FIONBIO, &argp);

#endif
}

//////////////////////////////////////////////////////////////////////
// get amount of data in socket input buffer
//
// Parameters
//    s - socket descriptor
//
// Return
//    amount of data in socket input buffer
//
//
//////////////////////////////////////////////////////////////////////
sint32 SOCKET_API::gx_availablesocket(TSocketIndex_t s)
{
#ifdef OS_UNIX
	return FILE_API::gx_availablefile(s);
#elif defined( OS_WINDOWS )
	unsigned long argp = 0;
	gx_ioctlsocket(s, FIONREAD, &argp);
	return argp;
#endif
	return 0;
}


//////////////////////////////////////////////////////////////////////
// shutdown all or part of connection of socket
//
// Parameters
//     s   - socket descriptor
//     how - how to close ( all , send , receive )
//
// Return
//     none
//
//
//////////////////////////////////////////////////////////////////////
bool SOCKET_API::gx_shutdown(TSocketIndex_t s, sint32 how)
{
	if (shutdown(s, how) < 0)
	{
#ifdef OS_UNIX
		switch (errno)
		{
		case EBADF:
		case ENOTSOCK:
		case ENOTCONN:
		default:
		{
				   gxCondLogB(_g_LibConfig.isConfig(LIB_CONFIG_DBG_SOCK_ERR), "Socket error!%u", errno);
				   break;
		}
		}
#elif defined( OS_WINDOWS )
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAENOTCONN:
			strncpy(Error, "WSAENOTCONN", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};
#endif

		return false;
	}

	return true;
}

sint32 SOCKET_API::gx_select(TSocketIndex_t maxfdp1, fd_set * readset, fd_set * writeset, fd_set * exceptset, TTimeVal_t * timeout)
{

	sint32 result;
	result = select((sint32)maxfdp1, readset, writeset, exceptset, timeout);
	if (result == SOCKET_ERROR)
	{
#ifdef OS_UNIX

#elif defined(OS_WINDOWS)
		sint32 iErr = WSAGetLastError();
		switch (iErr)
		{
		case WSANOTINITIALISED:
			strncpy(Error, "WSANOTINITIALISED", _ESIZE);
			break;
		case WSAEFAULT:
			strncpy(Error, "WSAEFAULT", _ESIZE);
			break;
		case WSAENETDOWN:
			strncpy(Error, "WSAENETDOWN", _ESIZE);
			break;
		case WSAEINVAL:
			strncpy(Error, "WSAEINVAL", _ESIZE);
			break;
		case WSAEINTR:
			strncpy(Error, "WSAEINTR", _ESIZE);
			break;
		case WSAEINPROGRESS:
			strncpy(Error, "WSAEINPROGRESS", _ESIZE);
			break;
		case WSAENOTSOCK:
			strncpy(Error, "WSAENOTSOCK", _ESIZE);
			break;
		default:
		{
				   strncpy(Error, "UNKNOWN", _ESIZE);
				   break;
		};
		};
#endif
	}   //end if

	return result;
}