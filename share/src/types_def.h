#ifndef _TYPES_DEF_H_
#define _TYPES_DEF_H_

// Wrapper for the clib time function
#define c_time      time
#define c_mktime    mktime
#define c_gmtime    gmtime
#define c_localtime localtime
#define c_difftime  difftime

/**
* FINAL_VERSION 最终版本
*/
#ifdef FINAL_VERSION
// If the FINAL_VERSION is defined externally, check that the value is 0 or 1
#	if FINAL_VERSION != 1 && FINAL_VERSION != 0
#		error "Bad value for FINAL_VERSION, it must be 0 or 1"
#	endif
#else
// If you want to compile in final version just put 1 instead of 0
// WARNING: never comment this #define
#	define FINAL_VERSION 0
#endif // FINAL_VERSION

// This way we know about _HAS_TR1 and _STLPORT_VERSION
#include <array>
#include <string.h>

// Operating systems definition
#ifdef OS_WIN32
#	define strcasecmp _stricmp
#	define OS_WINDOWS				// windows
#	define LITTLE_ENDIAN			// 大小端
#	define CPU_INTEL				// Intel CPU
#   ifndef _WIN32_WINNT
#		define _WIN32_WINNT 0x0500	// Minimal OS = Windows 2000 (This lib is not supported on Windows 95/98)
#   endif
#	if _MSC_VER >= 1500
#		define COMP_VC9
#		ifndef _STLPORT_VERSION // STLport doesn't depend on MS STL features
#			if defined(_HAS_TR1) && (_HAS_TR1 + 0) // VC9 TR1 feature pack
#				define ISO_STDTR1_AVAILABLE
#				define ISO_STDTR1_HEADER(header) <header>
#			endif
#		endif
#	endif
// #	ifdef _DEBUG
// #   error "Debug mod"
// #		define LIB_DEBUG
// #	else
// #		define LIB_RELEASE
// #	endif
#	ifdef _WIN64
#		define OS_WIN64
#	endif
#else
#	ifdef WORDS_BIGENDIAN
#		define BIG_ENDIAN
#	else 
#		ifndef LITTLE_ENDIAN
#			define LITTLE_ENDIAN
#		endif
#	endif
// these define are set the GNU/Linux
#	define OS_UNIX
#	define COMP_GCC
#endif

// gcc 3.4 introduced ISO C++ with tough template rules
//
// ISO_SYNTAX can be used using #if ISO_SYNTAX or #if !ISO_SYNTAX
//
#if defined(OS_WINDOWS)
#	define ISO_SYNTAX 0
#else
#	define ISO_SYNTAX 1
#endif

// gcc 4.1+ provides std::tr1
#ifdef COMP_GCC
#	define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	if GCC_VERSION > 40100
#		define ISO_STDTR1_AVAILABLE
#		define ISO_STDTR1_HEADER(header) <tr1/header>
#	endif
#endif

// Remove stupid Visual C++ warnings
#ifdef OS_WINDOWS
#	pragma warning (disable : 4503)			// STL: Decorated name length exceeded, name was truncated
#	pragma warning (disable : 4786)			// STL: too long identifier
#	pragma warning (disable : 4290)			// throw() not implemented warning
#	pragma warning (disable : 4250)			// inherits via dominance (informational warning).
#	pragma warning (disable : 4390)			// don't warn in empty block "if(exp) ;"
#	pragma warning (disable : 4996)			// 'vsnprintf': This function or variable may be unsafe. Consider using vsnprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#   pragma warning (disable : 4819)         // 该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失
// Visual C++ warning : ebp maybe modified
#	pragma warning(disable:4731)
// Debug : Sept 01 2006
#	if defined(COMP_VC9)
#		pragma warning (disable : 4005)			// don't warn on redefinitions caused by xp platform sdk
#	endif COMP_VC9
#endif // OS_WINDOWS

// Standard include

#include <exception>

// Define this if you want to remove all assert, debug code...
// You should never need to define this since it's always good to have assert, even in release mode


// Standard types

/*
* correct numeric types:	sint8, uint8, sint16, uint16, sint32, uint32, sint64, uint64, sint, uint
* correct char types:		char, string, ucchar, ucstring
* correct misc types:		void, bool, float, double
*/

/**
* \typedef uint8
* An unsigned 8 bits integer (use char only as \b character and not as integer)
**/

/**
* \typedef sint8
* An signed 8 bits integer (use char only as \b character and not as integer)
*/

/**
* \typedef uint16
* An unsigned 16 bits integer (don't use short)
**/

/**
* \typedef sint16
* An signed 16 bits integer (don't use short)
*/

/**
* \typedef uint32
* An unsigned 32 bits integer (don't use int or long)
**/

/**
* \typedef sint32
* An signed 32 bits integer (don't use int or long)
*/

/**
* \typedef uint64
* An unsigned 64 bits integer (don't use long long or __int64)
**/

/**
* \typedef sint64
* An signed 64 bits integer (don't use long long or __int64)
*/

/**
* \typedef uint
* An unsigned integer, at least 32 bits (used only for internal loops or speedy purpose, processor dependent)
**/

/**
* \typedef sint
* An signed integer at least 32 bits (used only for internal loops or speedy purpose, processor dependent)
*/

/**
* \def I64_FMT
* Used to display a int64 in a platform independent way with printf like functions.
\code
sint64 myint64 = SINT64_CONSTANT(0x123456781234);
printf("This is a 64 bits int: %"I64_FMT"u", myint64);
\endcode
*/

#ifdef OS_WINDOWS

typedef	signed		__int8		sint8;
typedef	unsigned	__int8		uint8;
typedef	signed		__int16		sint16;
typedef	unsigned	__int16		uint16;
typedef	signed		__int32		sint32;
typedef	unsigned	__int32		uint32;
typedef	signed		__int64		sint64;
typedef	unsigned	__int64		uint64;

typedef				int			sint32;			// at least 32bits (depend of processor)
typedef	unsigned	int			uint32;			// at least 32bits (depend of processor)

// int64输出格式
#define	I64_FMT "I64"

#elif defined (OS_UNIX)

#include <sys/types.h>
#include <stdint.h>
#include <climits>

typedef	int8_t		sint8;
typedef	uint8_t		uint8;
typedef	int16_t		sint16;
typedef	uint16_t	uint16;
typedef	int32_t		sint32;
typedef	uint32_t	uint32;
typedef	int64_t		sint64;
typedef	uint64_t	uint64;

typedef				int			sint32;			// at least 32bits (depend of processor)
typedef	unsigned	int			uint32;			// at least 32bits (depend of processor)

#if __SIZEOF_LONG__ == 8
#	define	I64_FMT "l"
#else
#	define	I64_FMT "ll"
#endif // __SIZEOF_LONG__ == 8

#endif // OS_UNIX


/// 常量定义
static const sint64 MAX_SINT64_NUM = LLONG_MAX;
static const sint64 INVALID_SINT64_NUM = MAX_SINT64_NUM;
static const uint64 MAX_UINT64_NUM = ULLONG_MAX;
static const uint64 INVALID_UINT64_NUM = MAX_UINT64_NUM;
//    static const sint32 MAX_SINT32_NUM         = std::numeric_limits<sint32>::max();
static const sint32 MAX_SINT32_NUM = INT_MAX;
static const sint32 INVALID_SINT32_NUM = MAX_SINT32_NUM;
//    static const uint32 MAX_UINT32_NUM         = std::numeric_limits<uint32>::max();
static const uint32 MAX_UINT32_NUM = UINT_MAX;
static const uint32 INVALID_UINT32_NUM = MAX_UINT32_NUM;
//    static const uint16 MAX_UINT16_NUM         = std::numeric_limits<uint16>::max();
static const uint16 MAX_UINT16_NUM = USHRT_MAX;
static const uint16 INVALID_UINT16_NUM = MAX_UINT16_NUM;
static const sint16 MAX_SINT16_NUM = SHRT_MAX;
static const sint16 INVALID_SINT16_NUM = MAX_SINT16_NUM;
static const uint8  MAX_UINT8_NUM = UCHAR_MAX;
static const uint8  INVALID_UINT8_NUM = MAX_UINT8_NUM;
static const sint8  MAX_SINT8_NUM = SCHAR_MAX;
static const sint8  INVALID_SINT8_NUM = MAX_SINT8_NUM;

/**
* \typedef ucchar
* An Unicode character (16 bits)
*/
typedef	uint16	ucchar;


// To define a 64bits constant; ie: UINT64_CONSTANT(0x123456781234)
#ifdef OS_WINDOWS
#	if defined(COMP_VC9)
#		define INT64_CONSTANT(c)	(c##LL)
#		define SINT64_CONSTANT(c)	(c##LL)
#		define UINT64_CONSTANT(c)	(c##LL)
#	else
#		define INT64_CONSTANT(c)	(c)
#		define SINT64_CONSTANT(c)	(c)
#		define UINT64_CONSTANT(c)	(c)
#	endif
#else
#	define INT64_CONSTANT(c)		(c##LL)
#	define SINT64_CONSTANT(c)	    (c##LL)
#	define UINT64_CONSTANT(c)	    (c##ULL)
#endif

// 使用最大路径
#if !defined(MAX_PATH) && !defined(OS_WINDOWS)
#	define MAX_PATH 255
#endif


#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#elif defined(OS_UNIX)
#endif

#ifdef OS_WINDOWS
#define SIGKILL SIGABRT          // 终止信号
#endif

// 系统调用, 凡是调用系统函数或标准C++函数都必须加此前缀, 如: SystemCall::time()
#define SystemCall    

// 标准常量
// #ifdef K
// #undef K
// #define K 1024
// #else
// #define K 1024
// #endif
// 
// #ifdef M
// #undef M
// #define M 1024*K
// #else
// #define M 1024*K
// #endif
// 
// #ifdef G
// #undef G
// #define G 1024*M
// #else
// #define G 1024*M
// #endif

// 信号量类型
typedef uint32 TSigno_t;

#endif // TYPES_DEF_H
