
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "base_util.h"

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

#if defined (OS_WINDOWS) && defined (LIB_DEBUG)
#define GXMISC_BREAKPOINT __debugbreak();
#elif defined (OS_UNIX) && defined (LIB_DEBUG)
#define GXMISC_BREAKPOINT gxExit(EXIT_CORE_BREAKPOINT);
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