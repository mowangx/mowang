
#ifndef _DEBUG_H_
#define _DBUG_H_

#include "base_util.h"

#define DConvertVargs(_dest,_format, _size) \
    char* _dest = NULL;  \
    char _cstring[_size]; \
    va_list _args; \
    va_start (_args, _format); \
    int _res = vsnprintf (_cstring, _size-1, _format, _args); \
    if (_res == -1 || _res == _size-1) \
    { \
    _cstring[_size-1] = '\0'; \
    } \
    va_end (_args); \
    _dest = _cstring

inline std::string toString(const char *format, ...)
{
	do
	{
		DConvertVargs(Result, format, 2048);
		return Result;
	} while (false);
	return NULL;
}

inline std::string gxToString(const char *fmt) { return toString(fmt); }

template <class T>
inline T to_base(const T& p) { return p; }

template <class T1>
inline std::string gxToString(const char *fmt, const T1& p1) { return toString(fmt, p1); }

template<class T> std::string gxToString(const T &obj)
{
	return obj.toString();
}
inline std::string gxToString(const uint8 &val) { return gxToString("%hu", (uint16)val); }
inline std::string gxToString(const sint8 &val) { return gxToString("%hd", (sint16)val); }
inline std::string gxToString(const uint16 &val) { return gxToString("%hu", val); }
inline std::string gxToString(const sint16 &val) { return gxToString("%hd", val); }
inline std::string gxToString(const uint32 &val) { return gxToString("%u", val); }
inline std::string gxToString(const sint32 &val) { return gxToString("%d", val); }
//inline std::string gxToString(const uint64 &val) { return gxToString("%"I64_FMT"u", val); }
//inline std::string gxToString(const sint64 &val) { return gxToString("%"I64_FMT"d", val); }


#if (SIZEOF_SIZE_T) == 8
inline std::string gxToString(const size_t &val) { return gxToString("%"I64_FMT"u", val); }
#endif

inline std::string gxToString(const float &val) { return gxToString("%f", val); }
inline std::string gxToString(const double &val) { return gxToString("%lf", val); }
inline std::string gxToString(const bool &val) { return gxToString("%u", val ? 1 : 0); }
inline std::string gxToString(const std::string &val) { return val; }

template<class T>
bool gxFromString(const std::string &str, T &obj)
{
	return obj.fromString(str);
}
inline bool gxFromString(const std::string &str, uint32 &val) { if (str.find('-') != std::string::npos) { val = 0; return false; } char *end; unsigned long v; errno = 0; v = strtoul(str.c_str(), &end, 10); if (errno || v > UINT_MAX || end == str.c_str()) { val = 0; return false; } else { val = (uint32)v; return true; } }
inline bool gxFromString(const std::string &str, sint32 &val) { char *end; long v; errno = 0; v = strtol(str.c_str(), &end, 10); if (errno || v > INT_MAX || v < INT_MIN || end == str.c_str()) { val = 0; return false; } else { val = (sint32)v; return true; } }
inline bool gxFromString(const std::string &str, uint8 &val) { char *end; long v; errno = 0; v = strtol(str.c_str(), &end, 10); if (errno || v > UCHAR_MAX || v < 0 || end == str.c_str()) { val = 0; return false; } else { val = (uint8)v; return true; } }
inline bool gxFromString(const std::string &str, sint8 &val) { char *end; long v; errno = 0; v = strtol(str.c_str(), &end, 10); if (errno || v > SCHAR_MAX || v < SCHAR_MIN || end == str.c_str()) { val = 0; return false; } else { val = (sint8)v; return true; } }
inline bool gxFromString(const std::string &str, uint16 &val) { char *end; long v; errno = 0; v = strtol(str.c_str(), &end, 10); if (errno || v > USHRT_MAX || v < 0 || end == str.c_str()) { val = 0; return false; } else { val = (uint16)v; return true; } }
inline bool gxFromString(const std::string &str, sint16 &val) { char *end; long v; errno = 0; v = strtol(str.c_str(), &end, 10); if (errno || v > SHRT_MAX || v < SHRT_MIN || end == str.c_str()) { val = 0; return false; } else { val = (sint16)v; return true; } }
//inline bool gxFromString(const std::string &str, uint64 &val) { bool ret = sscanf(str.c_str(), "%"I64_FMT"u", &val) == 1; if (!ret) val = 0; return ret; }
//inline bool gxFromString(const std::string &str, sint64 &val) { bool ret = sscanf(str.c_str(), "%"I64_FMT"d", &val) == 1; if (!ret) val = 0; return ret; }
inline bool gxFromString(const std::string &str, float &val) { bool ret = sscanf(str.c_str(), "%f", &val) == 1; if (!ret) val = 0.0f; return ret; }
inline bool gxFromString(const std::string &str, double &val) { bool ret = sscanf(str.c_str(), "%lf", &val) == 1; if (!ret) val = 0.0; return ret; }
inline bool gxFromString(const std::string &str, bool &val) { val = (str.length() == 1) && str[0] != '0'; return (str.length() == 1) && (str[0] == '0' || str[0] == '1'); }
inline bool gxFromString(const std::string &str, std::string &val) { val = str; return true; }

enum exit_code
{
	EXIT_CODE_NORMAL = 0,
	EXIT_CODE_CRASH,
	EXIT_CODE_LOG_ERREX,
	EXIT_CORE_BREAKPOINT
};

void gxSetDumpHandler(const std::string& moduleName);

std::string gxGetDumpName();

void gxExit(exit_code code);

TThreadID_t gxGetThreadID();

#if defined (OS_WINDOWS) && defined (LIB_DEBUG)
#define GXMISC_BREAKPOINT __debugbreak();
#elif defined (OS_UNIX) && defined (LIB_DEBUG)
#define GXMISC_BREAKPOINT GXMISC::gxExit(EXIT_CORE_BREAKPOINT);
#else
#define GXMISC_BREAKPOINT
#endif

#define gxAssert(exp) \
do { \
	bool exp_result = (exp) ? false : true; \
	if(exp_result) { \
		GXMISC_BREAKPOINT; \
    } \
} while(0)

#ifdef OS_WINDOWS
LONG CrashHandler(ULONG code, EXCEPTION_POINTERS *pException);
#elif defined(OS_UNIX)
#endif

#endif