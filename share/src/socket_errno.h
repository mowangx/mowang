#ifndef _SOCKET_ERRNO_H_
#define _SOCKET_ERRNO_H_

#ifdef OS_WINDOWS
// error code mapping for windows

// VC++ 10 has these error codes defined in errno.h,
// So we undef these error codes if they are wrongly defined
#   if defined (EWOULDBLOCK) && (EWOULDBLOCK != WSAEWOULDBLOCK)
#     undef EWOULDBLOCK
#   endif /* EWOULDBLOCK */
#   if defined (EINPROGRESS) && (EINPROGRESS != WSAEINPROGRESS)
#     undef EINPROGRESS
#   endif /* EINPROGRESS */
#   if defined (EALREADY) && (EALREADY != WSAEALREADY)
#     undef EALREADY
#   endif /* EALREADY */
#   if defined (ENOTSOCK) && (ENOTSOCK != WSAENOTSOCK)
#     undef ENOTSOCK
#   endif /* ENOTSOCK */
#   if defined (EDESTADDRREQ) && (EDESTADDRREQ != WSAEDESTADDRREQ)
#     undef EDESTADDRREQ
#   endif /* EDESTADDRREQ */
#   if defined (EMSGSIZE) && (EMSGSIZE != WSAEMSGSIZE)
#     undef EMSGSIZE
#   endif /* EMSGSIZE */
#   if defined (EPROTOTYPE) && (EPROTOTYPE != WSAEPROTOTYPE)
#     undef EPROTOTYPE
#   endif /* EPROTOTYPE */
#   if defined (ENOPROTOOPT) && (ENOPROTOOPT != WSAENOPROTOOPT)
#     undef ENOPROTOOPT
#   endif /* ENOPROTOOPT */
#   if defined (EPROTONOSUPPORT) && (EPROTONOSUPPORT != WSAEPROTONOSUPPORT)
#     undef EPROTONOSUPPORT
#   endif /* EPROTONOSUPPORT */
#   if defined (ESOCKTNOSUPPORT) && (ESOCKTNOSUPPORT != WSAESOCKTNOSUPPORT)
#     undef ESOCKTNOSUPPORT
#   endif /* ESOCKTNOSUPPORT */
#   if defined (EOPNOTSUPP) && (EOPNOTSUPP != WSAEOPNOTSUPP)
#     undef EOPNOTSUPP
#   endif /* EOPNOTSUPP */
#   if defined (EPFNOSUPPORT) && (EPFNOSUPPORT != WSAEPFNOSUPPORT)
#     undef EPFNOSUPPORT
#   endif /* EPFNOSUPPORT */
#   if defined (EAFNOSUPPORT) && (EAFNOSUPPORT != WSAEAFNOSUPPORT)
#     undef EAFNOSUPPORT
#   endif /* EAFNOSUPPORT */
#   if defined (EADDRINUSE) && (EADDRINUSE != WSAEADDRINUSE)
#     undef EADDRINUSE
#   endif /* EADDRINUSE */
#   if defined (EADDRNOTAVAIL) && (EADDRNOTAVAIL != WSAEADDRNOTAVAIL)
#     undef EADDRNOTAVAIL
#   endif /* EADDRNOTAVAIL */
#   if defined (ENETDOWN) && (ENETDOWN != WSAENETDOWN)
#     undef ENETDOWN
#   endif /* ENETDOWN */
#   if defined (ENETUNREACH) && (ENETUNREACH != WSAENETUNREACH)
#     undef ENETUNREACH
#   endif /* ENETUNREACH */
#   if defined (ENETRESET) && (ENETRESET != WSAENETRESET)
#     undef ENETRESET
#   endif /* ENETRESET */
#   if defined (ECONNABORTED) && (ECONNABORTED != WSAECONNABORTED)
#     undef ECONNABORTED
#   endif /* ECONNABORTED */
#   if defined (ECONNRESET) && (ECONNRESET != WSAECONNRESET)
#     undef ECONNRESET
#   endif /* ECONNRESET */
#   if defined (ENOBUFS) && (ENOBUFS != WSAENOBUFS)
#     undef ENOBUFS
#   endif /* ENOBUFS */
#   if defined (EISCONN) && (EISCONN != WSAEISCONN)
#     undef EISCONN
#   endif /* EISCONN */
#   if defined (ENOTCONN) && (ENOTCONN != WSAENOTCONN)
#     undef ENOTCONN
#   endif /* ENOTCONN */
#   if defined (ESHUTDOWN) && (ESHUTDOWN != WSAESHUTDOWN)
#     undef ESHUTDOWN
#   endif /* ESHUTDOWN */
#   if defined (ETOOMANYREFS) && (ETOOMANYREFS != WSAETOOMANYREFS)
#     undef ETOOMANYREFS
#   endif /* ETOOMANYREFS */
#   if defined (ETIMEDOUT) && (ETIMEDOUT != WSAETIMEDOUT)
#     undef ETIMEDOUT
#   endif /* ETIMEDOUT */
#   if defined (ECONNREFUSED) && (ECONNREFUSED != WSAECONNREFUSED)
#     undef ECONNREFUSED
#   endif /* ECONNREFUSED */
#   if defined (ELOOP) && (ELOOP != WSAELOOP)
#     undef ELOOP
#   endif /* ELOOP */
//#   if defined (ENAMETOOLONG) && (ENAMETOOLONG != WSAENAMETOOLONG)
//#      undef ENAMETOOLONG
//#   endif /* ENAMETOOLONG */
#   if defined (EHOSTDOWN) && (EHOSTDOWN != WSAEHOSTDOWN)
#     undef EHOSTDOWN
#   endif /* EHOSTDOWN */
#   if defined (EHOSTUNREACH) && (EHOSTUNREACH != WSAEHOSTUNREACH)
#     undef EHOSTUNREACH
#   endif /* EHOSTUNREACH */
//#   if defined (ENOTEMPTY) && (ENOTEMPTY != WSAENOTEMPTY)
//#      undef ENOTEMPTY
//#   endif /* ENOTEMPTY */
#   if defined (EPROCLIM) && (EPROCLIM != WSAEPROCLIM)
#     undef EPROCLIM
#   endif /* EPROCLIM */
#   if defined (EUSERS) && (EUSERS != WSAEUSERS)
#     undef EUSERS
#   endif /* EUSERS */
#   if defined (EDQUOT) && (EDQUOT != WSAEDQUOT)
#     undef EDQUOT
#   endif /* EDQUOT */
#   if defined (ESTALE) && (ESTALE != WSAESTALE)
#     undef ESTALE
#   endif /* ESTALE */
#   if defined (EREMOTE) && (EREMOTE != WSAEREMOTE)
#     undef EREMOTE
#   endif /* EREMOTE */

#   if !defined (ETIME)
#     define ETIME                  ERROR_SEM_TIMEOUT
#   endif /* !ETIME */
#   if !defined (EWOULDBLOCK)
#     define EWOULDBLOCK             WSAEWOULDBLOCK
#   endif /* !EWOULDBLOCK */
#   if !defined (EINPROGRESS)
#     define EINPROGRESS             WSAEINPROGRESS
#   endif /* !EINPROGRESS */
#   if !defined (EALREADY)
#     define EALREADY                WSAEALREADY
#   endif /* !EALREADY */
#   if !defined (ENOTSOCK)
#     define ENOTSOCK                WSAENOTSOCK
#   endif /* !ENOTSOCK */
#   if !defined (EDESTADDRREQ)
#     define EDESTADDRREQ            WSAEDESTADDRREQ
#   endif /* !EDESTADDRREQ */
#   if !defined (EMSGSIZE)
#     define EMSGSIZE                WSAEMSGSIZE
#   endif /* !EMSGSIZE */
#   if !defined (EPROTOTYPE)
#     define EPROTOTYPE              WSAEPROTOTYPE
#   endif /* !EPROTOTYPE */
#   if !defined (ENOPROTOOPT)
#     define ENOPROTOOPT             WSAENOPROTOOPT
#   endif /* !ENOPROTOOPT */
#   if !defined (EPROTONOSUPPORT)
#     define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#   endif /* !EPROTONOSUPPORT */
#   if !defined (ESOCKTNOSUPPORT)
#     define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#   endif /* !ESOCKTNOSUPPORT */
#   if !defined (EOPNOTSUPP)
#     define EOPNOTSUPP              WSAEOPNOTSUPP
#   endif /* !EOPNOTSUPP */
#   if !defined (EPFNOSUPPORT)
#     define EPFNOSUPPORT            WSAEPFNOSUPPORT
#   endif /* !EPFNOSUPPORT */
#   if !defined (EAFNOSUPPORT)
#     define EAFNOSUPPORT            WSAEAFNOSUPPORT
#   endif /* !EAFNOSUPPORT */
#   if !defined (EADDRINUSE)
#     define EADDRINUSE              WSAEADDRINUSE
#   endif /* !EADDRINUSE */
#   if !defined (EADDRNOTAVAIL)
#     define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#   endif /* !EADDRNOTAVAIL */
#   if !defined (ENETDOWN)
#     define ENETDOWN                WSAENETDOWN
#   endif /* !ENETDOWN */
#   if !defined (ENETUNREACH)
#     define ENETUNREACH             WSAENETUNREACH
#   endif /* !ENETUNREACH */
#   if !defined (ENETRESET)
#     define ENETRESET               WSAENETRESET
#   endif /* !ENETRESET */
#   if !defined (ECONNABORTED)
#     define ECONNABORTED            WSAECONNABORTED
#   endif /* !ECONNABORTED */
#   if !defined (ECONNRESET)
#     define ECONNRESET              WSAECONNRESET
#   endif /* !ECONNRESET */
#   if !defined (ENOBUFS)
#     define ENOBUFS                 WSAENOBUFS
#   endif /* !ENOBUFS */
#   if !defined (EISCONN)
#     define EISCONN                 WSAEISCONN
#   endif /* !EISCONN */
#   if !defined (ENOTCONN)
#     define ENOTCONN                WSAENOTCONN
#   endif /* !ENOTCONN */
#   if !defined (ESHUTDOWN)
#     define ESHUTDOWN               WSAESHUTDOWN
#   endif /* !ESHUTDOWN */
#   if !defined (ETOOMANYREFS)
#     define ETOOMANYREFS            WSAETOOMANYREFS
#   endif /* !ETOOMANYREFS */
#   if !defined (ETIMEDOUT)
#     define ETIMEDOUT               WSAETIMEDOUT
#   endif /* !ETIMEDOUT */
#   if !defined (ECONNREFUSED)
#     define ECONNREFUSED            WSAECONNREFUSED
#   endif /* !ECONNREFUSED */
#   if !defined (ELOOP)
#     define ELOOP                   WSAELOOP
#   endif /* !ELOOP */
#   if !defined (EHOSTDOWN)
#     define EHOSTDOWN               WSAEHOSTDOWN
#   endif /* !EHOSTDOWN */
#   if !defined (EHOSTUNREACH)
#     define EHOSTUNREACH            WSAEHOSTUNREACH
#   endif /* !EHOSTUNREACH */
#   if !defined (EPROCLIM)
#     define EPROCLIM                WSAEPROCLIM
#   endif /* !EPROCLIM */
#   if !defined (EUSERS)
#     define EUSERS                  WSAEUSERS
#   endif /* !EUSERS */
#   if !defined (EDQUOT)
#     define EDQUOT                  WSAEDQUOT
#   endif /* !EDQUOT */
#   if !defined (ESTALE)
#     define ESTALE                  WSAESTALE
#   endif /* !ESTALE */
#   if !defined (EREMOTE)
#     define EREMOTE                 WSAEREMOTE
#   endif /* !EREMOTE */

// Grrr! ENAMETOOLONG and ENOTEMPTY are already defined by the horrible
// 'standard' library.
// #define ENAMETOOLONG            WSAENAMETOOLONG
#   if !defined (EADDRINUSE)
#     define EADDRINUSE              WSAEADDRINUSE
#   endif /* EADDRINUSE*/

// CE needs this...
#  if !defined (EPERM)
#    define EPERM                 ERROR_ACCESS_DENIED
#  endif

#endif      // OS_WINDOWS

#endif