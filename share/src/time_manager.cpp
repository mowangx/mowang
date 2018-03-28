
#include "time_manager.h"
#include "types_def.h"

time_manager::time_manager()
{
	m_current_time = 0;
	m_start_time = 0;
	init();
}

time_manager::~time_manager()
{
}

void time_manager::init()
{
#if defined(OS_WINDOWS)
	m_start_time = SystemCall::GetTickCount();
	m_current_time = SystemCall::GetTickCount();
#elif defined(OS_UNIX)
	_startTime = 0;
	_currentTime = 0;
	SystemCall::gettimeofday(&_TStart, &_TZ);
#endif

	local_time();

	memset(m_time_buffer, 0, 100);
}

TTime_t time_manager::now_sys_time()
{
	return m_set_time;
}

TTime_t time_manager::SysNowTime()
{
	return time_manager::AnsiToGxTime(time(NULL));
}

uint32 time_manager::current_date()
{
	local_time();
	uint32 Date;
	ConvertTU(&m_tm, Date);

	return Date;
}


void time_manager::local_time()
{
	time((time_t*)&m_set_time);
	tm tempTm;
	tm* ptm = time_manager::LocalTime((time_t*)&m_set_time, &tempTm);
	m_tm = *ptm;
}

// 得到标准时间
TTime_t time_manager::get_ansi_time()
{
	local_time();
	return m_set_time;
}

uint32 time_manager::time_2_number()
{
	local_time();

	uint32 uRet = 0;

	uRet += get_year();
	uRet -= 2000;
	uRet = uRet * 100;

	uRet += get_month() + 1;
	uRet = uRet * 100;

	uRet += get_day();
	uRet = uRet * 100;

	uRet += get_hour();
	uRet = uRet * 100;

	uRet += get_minute();

	return uRet;
}

uint32 time_manager::DiffTime(uint32 date1, uint32 date2)
{
	tm S_D1, S_D2;
	ConvertUT(date1, &S_D1);
	ConvertUT(date2, &S_D2);
	time_t t1, t2;
	t1 = mktime(&S_D1);
	t2 = mktime(&S_D2);
	uint32 dif = (uint32)(difftime(t2, t1) * 1000);
	return dif;
}

void time_manager::ConvertUT(uint32 Date, tm* TM)
{
	memset(TM, 0, sizeof(tm));
	TM->tm_year = (Date >> 26) & 0xf;
	TM->tm_mon = (Date >> 22) & 0xf;
	TM->tm_mday = (Date >> 17) & 0x1f;
	TM->tm_hour = (Date >> 12) & 0x1f;
	TM->tm_min = (Date >> 6) & 0x3f;
	TM->tm_sec = (Date)& 0x3f;
}

void time_manager::ConvertTU(tm* TM, uint32& Date)
{
	Date = 0;
	Date += (TM->tm_yday % 10) & 0xf;
	Date = (Date << 4);
	Date += TM->tm_mon & 0xf;
	Date = (Date << 4);
	Date += TM->tm_mday & 0x1f;
	Date = (Date << 5);
	Date += TM->tm_hour & 0x1f;
	Date = (Date << 5);
	Date += TM->tm_min & 0x3f;
	Date = (Date << 6);
	Date += TM->tm_sec & 0x3f;
}

uint32 time_manager::get_day_time()
{
	time_t st;
	time(&st);
	tm tempTm;
	tm* ptm = time_manager::LocalTime((time_t*)&st, &tempTm);
	uint32 uRet = 0;
	uRet = (ptm->tm_year - 100) * 1000;
	uRet += ptm->tm_yday;

	return uRet;
}

uint32 time_manager::get_today_time()
{
	tm tempTm;
	tm* ptm = time_manager::LocalTime((time_t*)&m_set_time, &tempTm);
	uint32 uRet = 0;
	uRet = ptm->tm_hour * 100;
	uRet += ptm->tm_min;

	return uRet;
}

bool time_manager::FormatTodayTime(uint32& nTime)
{
	bool ret = false;

	uint32 uHour = nTime / 100;
	uint32 uMin = nTime % 100;
	uint32 uAddHour = 0;
	if (uMin > 59)
	{
		uAddHour = uMin / 60;
		uMin = uMin % 60;
	}
	uHour += uAddHour;
	if (uHour > 23)
	{
		ret = true;
		uHour = uHour % 60;
	}

	return ret;
}

TAppTime_t time_manager::update()
{
	get_ansi_time();

#if defined(OS_WINDOWS)
	m_current_time = GetTickCount();
#elif defined(OS_UNIX)
	gettimeofday(&_TEnd, &_TZ);
	double t1, t2;
	t1 = (double)_TStart.tv_sec * 1000 + (double)_TStart.tv_usec / 1000;
	t2 = (double)_TEnd.tv_sec * 1000 + (double)_TEnd.tv_usec / 1000;
	_currentTime = (uint32)(t2 - t1);
#endif

	strftime(m_time_buffer, 100, "%Y-%m-%d %H:%M:%S", &m_tm);

	return m_current_time;
}

TTime_t time_manager::AnsiToGxTime(time_t times)
{
	return (TTime_t)times;
}

time_t time_manager::GxToAnsiTime(TTime_t times)
{
	return times;
}

void time_manager::FormatSystemTime(TTime_t times, std::string& str)
{
	static char cstime[100];
	time_t tt = time_manager::GxToAnsiTime(times);
	tm tempTm;
	tm* tms = time_manager::LocalTime((time_t*)&tt, &tempTm);
	if (tms)
	{
		strftime(cstime, 100, "%Y-%m-%d %H:%M:%S", tms);
	}
	else
	{
		sprintf(cstime, "bad date %d", (uint32)tt);
	}
	str = cstime;
}

sint32 time_manager::get_year()
{
	return m_tm.tm_year + 1900;
}

sint32 time_manager::get_month()
{
	return m_tm.tm_mon;
}

sint32 time_manager::get_day()
{
	return m_tm.tm_mday;
}

sint32 time_manager::get_hour()
{
	return m_tm.tm_hour;
}

sint32 time_manager::get_minute()
{
	return m_tm.tm_min;
}

sint32 time_manager::get_second()
{
	return m_tm.tm_sec;
}

sint32 time_manager::get_week()
{
	return m_tm.tm_wday;
}

struct tm * time_manager::LocalTime(const time_t *timep, struct tm *result)
{
#if defined(OS_WINDOWS)
	localtime_s(result, timep);
	return result;
#elif defined(OS_UNIX)
	return localtime_r(timep, result);
#endif
}