
#include "debug.h"

#include <thread>

#ifdef OS_WINDOWS
#	define _WIN32_WINDOWS	0x0410
#	define WINVER			0x0400
#	include <direct.h>
#	include <tchar.h>
//#	include <imagehlp.h>
#   include <excpt.h>
#	pragma comment(lib, "imagehlp.lib")
#	define getcwd(_a, _b) (_getcwd(_a,_b))
#	ifdef OS_WIN64
#		define DWORD_TYPE DWORD64
#	else
#		define DWORD_TYPE DWORD
#	endif // OS_WIN64
#elif defined OS_UNIX
#	include <unistd.h>
#	define IsDebuggerPresent() false
#	include <execinfo.h>
#	include <errno.h>
#   include "coredumper.h"
#endif

#include "log.h"
#include "time_manager.h"

static bool DumpFlag = false;
static std::string ModuleName = "";

// dump处理
#ifdef OS_WINDOWS

#include <DbgHelp.h>

static LONG WINAPI DumpMiniDump(PEXCEPTION_POINTERS excpInfo)
{
	if (DumpFlag) {
		return 0;
	}

	DumpFlag = true;
	HANDLE file = CreateFileA(gxGetDumpName().c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file) {
		HMODULE hm = LoadLibraryA("dbghelp.dll");
		if (hm) {
			BOOL(WINAPI *MiniDumpWriteDump)(
				HANDLE hProcess,
				DWORD ProcessId,
				HANDLE hFile,
				MINIDUMP_TYPE DumpType,
				PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
				PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
				PMINIDUMP_CALLBACK_INFORMATION CallbackParam
				);
			*(FARPROC*)&MiniDumpWriteDump = GetProcAddress(hm, "MiniDumpWriteDump");
			if (MiniDumpWriteDump) {
				_MINIDUMP_EXCEPTION_INFORMATION eInfo;
				eInfo.ThreadId = GetCurrentThreadId();
				eInfo.ExceptionPointers = excpInfo;
				eInfo.ClientPointers = FALSE;

				MiniDumpWriteDump(
					GetCurrentProcess(),
					GetCurrentProcessId(),
					file,
					//MiniDumpNormal,
					MiniDumpWithFullMemory,
					excpInfo ? &eInfo : NULL,
					NULL,
					NULL);
			}
			else {
				log_warning("Can't get proc MiniDumpWriteDump in dbghelp.dll");
			}
		}
		else {
			log_warning("Can't load dbghelp.dll");
		}

		CloseHandle(file);
	}
	else {
		log_warning("Can't create mini dump file");
	}

	log_warning("Create dump file!");
	
	gxExit(EXIT_CODE_CRASH);
	return 0;
}

LONG CrashHandler(ULONG code /*= GetExceptionCode()*/, EXCEPTION_POINTERS *pException /*= GetExceptionInformation()*/)
{
	DumpMiniDump(pException);
	exit(0);
	return 0;
}

#elif defined(OS_UNIX)	// OS_WINDOWS
/** 所有的信号列表及说明
01) SIGHUP
02) SIGINT
03) SIGQUIT
04) SIGILL
05) SIGTRAP
06) SIGABRT
07) SIGBUS
08) SIGFPE
09) SIGKILL
10) SIGUSR1
11) SIGSEGV
12) SIGUSR2
13) SIGPIPE
14) SIGALRM
15) SIGTERM
17) SIGCHLD
18) SIGCONT
19) SIGSTOP
20) SIGTSTP
21) SIGTTIN
22) SIGTTOU
23) SIGURG
24) SIGXCPU
25) SIGXFSZ
26) SIGVTALRM
27) SIGPROF
28) SIGWINCH
29) SIGIO
30) SIGPWR
31) SIGSYS
34) SIGRTMIN
35) SIGRTMIN+1
36) SIGRTMIN+2
37) SIGRTMIN+3
38) SIGRTMIN+4
39) SIGRTMIN+5
40) SIGRTMIN+6
41) SIGRTMIN+7
42) SIGRTMIN+8
43) SIGRTMIN+9
44) SIGRTMIN+10
45) SIGRTMIN+11
46) SIGRTMIN+12
47) SIGRTMIN+13
48) SIGRTMIN+14
49) SIGRTMIN+15
50) SIGRTMAX-14
51) SIGRTMAX-13
52) SIGRTMAX-12
53) SIGRTMAX-11
54) SIGRTMAX-10
55) SIGRTMAX-9
56) SIGRTMAX-8
57) SIGRTMAX-7
58) SIGRTMAX-6
59) SIGRTMAX-5
60) SIGRTMAX-4
61) SIGRTMAX-3
62) SIGRTMAX-2
63) SIGRTMAX-1
64) SIGRTMAX
*/
// @todo 处理dump生成
static void SigHandler(int signo)
{
	if (DumpFlag) {
		return;
	}

	DumpFlag = true;
	log_warning("signo = %d", signo);
	if (0 == WriteCoreDump(gxGetDumpName().c_str())) {
		std::cerr << "write dump success!" << std::endl;
	}
	else {
		std::cerr << "write dump failed!" << std::endl;
	}


	gxExit(EXIT_CODE_CRASH);
}

static void SetSignalHandle()
{
	SystemCall::signal(SIGSEGV, SigHandler);
	SystemCall::signal(SIGFPE, SigHandler);
	SystemCall::signal(SIGILL, SigHandler);
	SystemCall::signal(SIGINT, SigHandler);
	SystemCall::signal(SIGTERM, SigHandler);
	SystemCall::signal(SIGABRT, SigHandler);
	SystemCall::signal(SIGXFSZ, SigHandler);
}
#endif  // OS_UNIX

void gxSetDumpHandler(const std::string& moduleName)
{
	ModuleName = moduleName;
#ifdef OS_WINDOWS
	// 设置错误处理的安静模式
	// SetErrorMode(SEM_NOGPFAULTERRORBOX);
	SetUnhandledExceptionFilter(&DumpMiniDump);
#elif defined(OS_UNIX)
	SetSignalHandle();
#endif
}

std::string gxGetDumpName()
{
	// 生成dump文件名
	static char cstime[25];
	time_t tt = time(NULL);
	tm tempTm;
	tm* tms = time_manager::LocalTime((time_t*)&tt, &tempTm);
	if (tms)
		strftime(cstime, 25, "%Y_%m_%d_%H_%M_%S", tms);
	else
		sprintf(cstime, "bad date %d", (uint32)tt);

	std::string module_name = ModuleName;
	std::string fileName = module_name + "_" + cstime + ".dmp";
	return fileName;
}

void gxExit(EExitCode code)
{
	if (code == EXIT_CODE_CRASH) {
		exit(code);
	}
	else {
		char *temp = NULL;
		*temp = 1;
		exit(code);
	}
}

TThreadID_t gxGetThreadID()
{
	std::thread::id& thread_info = std::this_thread::get_id();
	return ((_Thrd_t*)&thread_info)->_Id;
}