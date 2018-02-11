
#ifndef _DEBUG_H_
#define _DBUG_H_

#include "base_util.h"

enum EExitCode
{
	EXIT_CODE_NORMAL = 0,
	EXIT_CODE_CRASH,
	EXIT_CODE_LOG_ERREX,
	EXIT_CORE_BREAKPOINT
};

void gxSetDumpHandler(const std::string& moduleName);

std::string gxGetDumpName();

void gxExit(EExitCode code);

TThreadID_t gxGetThreadID();

#if defined (OS_WINDOWS) && defined (LIB_DEBUG)
#define GXMISC_BREAKPOINT __debugbreak();
#elif defined (OS_UNIX) && defined (LIB_DEBUG)
#define GXMISC_BREAKPOINT GXMISC::gxExit(EXIT_CORE_BREAKPOINT);
#else
#define GXMISC_BREAKPOINT
#endif

#ifdef OS_WINDOWS
LONG CrashHandler(ULONG code, EXCEPTION_POINTERS *pException);
#elif defined(OS_UNIX)
#endif

#endif