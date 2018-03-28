
#ifndef _LOG_H_
#define _LOG_H_

#include <string>
#include <mutex>
#include <thread>
#include <deque>

#include "types_def.h"
#include "singleton.h"


class log_wrapper : public singleton<log_wrapper>
{
public:
	enum ELogType
	{
		LOG_ERROR = 1,
		LOG_WARNING = 2,
		LOG_INFO = 3,
		LOG_DEBUG = 4
	};

	log_wrapper();
	~log_wrapper();

public:
	bool init(const std::string& filename);
	void display(ELogType type, const char* filename, const char* funcname, sint32 line, const std::thread::id& thread_info, const char* format, ...);
	void flush();

private:
	std::string parse_filename(const std::string& filename) const;
	void check_rename_file();

private:
	FILE* m_file;
	std::mutex m_mutex;
	std::string m_filename;
	std::deque<std::string> m_logs;
};

#define DLogMgr							singleton<log_wrapper>::get_instance()

#define log_core(type, fmt, ...)        DLogMgr.display(type, __FILE__, __FUNCTION__, __LINE__, std::this_thread::get_id(), fmt, ##__VA_ARGS__)
#define log_debug(fmt, ...)				log_core(log_wrapper::LOG_DEBUG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)				log_core(log_wrapper::LOG_INFO, fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...)			log_core(log_wrapper::LOG_WARNING, fmt, ##__VA_ARGS__)
#define log_error(fmt, ...)				log_core(log_wrapper::LOG_ERROR, fmt, ##__VA_ARGS__)


#endif