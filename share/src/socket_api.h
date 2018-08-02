
#ifndef _SOCKET_API_H_
#define _SOCKET_API_H_

#include "socket_errno.h"
#include "socket_util.h"

namespace SOCKET_API
{
	bool gx_lib_init();

	void gx_lib_cleanup();

	TSocketFD_t gx_socket(sint32 domain, sint32 type, sint32 protocol);

	bool gx_bind(TSocketFD_t s, const struct sockaddr* name, sint32 namelen);

	bool gx_connect(TSocketFD_t s, const struct sockaddr* name, sint32 namelen);
	// 可以指定超时时间
	bool gx_connect2(TSocketFD_t s, const struct sockaddr* name, sint32 namelen, sint32 diff);

	bool gx_listen(TSocketFD_t s, sint32 backlog);

	TSocketFD_t gx_accept(TSocketFD_t s, struct sockaddr* addr, uint32* addrlen);

	bool gx_getsockopt(TSocketFD_t s, sint32 level, sint32 optname, void* optval, uint32* optlen);

	sint32 gx_getsockopt2(TSocketFD_t s, sint32 level, sint32 optname, void* optval, uint32* optlen);

	bool gx_setsockopt(TSocketFD_t s, sint32 level, sint32 optname, const void* optval, sint32 optlen);

	sint32 gx_send(TSocketFD_t s, const void* buf, sint32 len, sint32 flags);

	sint32 gx_sendto(TSocketFD_t s, const void* buf, sint32 len, sint32 flags, const struct sockaddr* to, sint32 tolen);

	sint32 gx_recv(TSocketFD_t s, void* buf, sint32 len, sint32 flags);

	sint32 gx_recvfrom(TSocketFD_t s, void* buf, sint32 len, sint32 flags, struct sockaddr* from, uint32* fromlen);

	bool gx_closesocket(TSocketFD_t s);

	bool gx_ioctlsocket(TSocketFD_t s, long cmd, unsigned long* argp);

	bool gx_getsocketnonblocking(TSocketFD_t s);

	bool gx_setsocketnonblocking(TSocketFD_t s, bool on);

	sint32 gx_availablesocket(TSocketFD_t s);

	bool gx_shutdown(TSocketFD_t s, sint32 how);

	sint32 gx_select(TSocketFD_t maxfdp1, fd_set* readset, fd_set* writeset, fd_set* exceptset, TTimeVal_t* timeout);

	void gx_ioctl(sint32 fd, sint32 request, void * argp);
	sint32 gx_availablefile(sint32 fd);
	sint32 gx_fcntl(sint32 fd, sint32 cmd);
	sint32 gx_fcntl(sint32 fd, sint32 cmd, sint32 arg);
}

#endif