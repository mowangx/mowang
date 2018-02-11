
#ifndef _SOCKET_API_H_
#define _SOCKET_API_H_

#include "socket_errno.h"
#include "socket_util.h"

namespace SOCKET_API
{
	bool gx_lib_init();

	void gx_lib_cleanup();

	TSocketIndex_t gx_socket(sint32 domain, sint32 type, sint32 protocol);

	bool gx_bind(TSocketIndex_t s, const struct sockaddr* name, sint32 namelen);

	bool gx_connect(TSocketIndex_t s, const struct sockaddr* name, sint32 namelen);
	// 可以指定超时时间
	bool gx_connect2(TSocketIndex_t s, const struct sockaddr* name, sint32 namelen, sint32 diff);

	bool gx_listen(TSocketIndex_t s, sint32 backlog);

	TSocketIndex_t gx_accept(TSocketIndex_t s, struct sockaddr* addr, uint32* addrlen);

	bool gx_getsockopt(TSocketIndex_t s, sint32 level, sint32 optname, void* optval, uint32* optlen);

	sint32 gx_getsockopt2(TSocketIndex_t s, sint32 level, sint32 optname, void* optval, uint32* optlen);

	bool gx_setsockopt(TSocketIndex_t s, sint32 level, sint32 optname, const void* optval, sint32 optlen);

	sint32 gx_send(TSocketIndex_t s, const void* buf, sint32 len, sint32 flags);

	sint32 gx_sendto(TSocketIndex_t s, const void* buf, sint32 len, sint32 flags, const struct sockaddr* to, sint32 tolen);

	sint32 gx_recv(TSocketIndex_t s, void* buf, sint32 len, sint32 flags);

	sint32 gx_recvfrom(TSocketIndex_t s, void* buf, sint32 len, sint32 flags, struct sockaddr* from, uint32* fromlen);

	bool gx_closesocket(TSocketIndex_t s);

	bool gx_ioctlsocket(TSocketIndex_t s, long cmd, unsigned long* argp);

	bool gx_getsocketnonblocking(TSocketIndex_t s);

	bool gx_setsocketnonblocking(TSocketIndex_t s, bool on);

	sint32 gx_availablesocket(TSocketIndex_t s);

	bool gx_shutdown(TSocketIndex_t s, sint32 how);

	sint32 gx_select(TSocketIndex_t maxfdp1, fd_set* readset, fd_set* writeset, fd_set* exceptset, TTimeVal_t* timeout);
}

#endif