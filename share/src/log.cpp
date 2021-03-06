
#include "log.h"

#include <cstdarg>
#include <string>

#ifdef OS_WINDOWS
#	define NOMINMAX
#	include <io.h>
#else
# include <unistd.h>
#endif // OS_WINDOWS

#include "auto_lock.h"
#include "time_manager.h"

static const char* LogTypeString[] = {
	"",
	"ERROR",
	"WARNING",
	"INFO",
	"DEBUG"
};

log_wrapper::log_wrapper()
{
	m_file = (FILE*)1;
}

log_wrapper::~log_wrapper()
{
	if (m_file > (FILE*)1) {
		fclose(m_file);
		m_file = NULL;
	}
}

bool log_wrapper::init(const std::string& filename)
{
	m_filename = filename;
	std::string cur_filename = m_filename + ".log";
	m_file = fopen(cur_filename.c_str(), "at");
	if (NULL == m_file) {
		printf("Can't open log file '%s': %s\n", filename.c_str(), strerror(errno));
		return false;
	}
	check_rename_file();
	return true;
}

void log_wrapper::display(ELogType type, const char* filename, const char* funcname, sint32 line, const std::thread::id& thread_info, const char* format, ...)
{
	char content[2048];
	va_list args;
	va_start(args, format);
	int _res = vsnprintf(content, 2047, format, args);
	if (_res == -1 || _res == 2047) {
		content[2047] = '\0';
	}
	va_end(args);

	time_t date = time(NULL);
	char cstime[25];
	tm tmp_tm;
	tm* tms = time_manager::LocalTime((time_t*)&date, &tmp_tm);
	if (NULL != tms) {
		strftime(cstime, 25, "%Y/%m/%d %H:%M:%S", tms);
	}
	else {
		sprintf(cstime, "bad date %d", (uint32)date);
	}

	char buf[2048];
	memset(buf, 0, 2048);
	sprintf(buf, "%s %s ThreadID: [%" I64_FMT "u] %s:%d %s() ", cstime, LogTypeString[type], *(uint64*)&thread_info, parse_filename(filename).c_str(), line, funcname);
	std::string str = buf;
	str += content;
	str += "\n";

	auto_lock lock(&m_mutex);
	m_logs.push_back(str);
}

void log_wrapper::flush()
{
	if (m_logs.empty()) {
		return;
	}
	std::vector<std::string> logs;
	{
		auto_lock lock(&m_mutex);
		logs.insert(logs.end(), m_logs.begin(), m_logs.end());
		m_logs.clear();
	}
	
	for (auto s : logs) {
		fwrite(s.c_str(), s.size(), 1, m_file);
	}
	fflush(m_file);
	check_rename_file();
}

std::string log_wrapper::parse_filename(const std::string& filename) const
{
	std::string::size_type pos = filename.find_last_of('/');
	if (pos == std::string::npos) {
		pos = filename.find_last_of('\\');
		if (pos == std::string::npos) {
			pos = filename.find_last_of('@');
		}
	}

	if (pos != std::string::npos) {
		return filename.substr(pos + 1);
	}
	else {
		return filename;
	}
}

void log_wrapper::check_rename_file()
{
	// 10485760 = 10 * 1024 * 1024
	if (ftell(m_file) < 10485760) {
		
		return;
	}

	fclose(m_file);

	std::string cur_filename = m_filename + ".log";
	for (int i = 0; i < 9999; ++i) {
		char filename[256];
		sprintf(filename, "%s_%d.log", m_filename.c_str(), i);
#ifdef OS_WINDOWS
		if (_access(filename, 00) == 0 && _access(filename, 06) == -1){
			continue;
		}
#else
		if (access(filename, F_OK) == 0){
			continue;
		}
#endif
		else {
			rename(cur_filename.c_str(), filename);
		}
	}

	m_file = fopen(cur_filename.c_str(), "at");
}

