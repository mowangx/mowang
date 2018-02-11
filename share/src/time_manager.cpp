
#include "time_manager.h"
#include "types_def.h"

CTimeManager::CTimeManager()
{
	_currentTime = 0;
	_startTime = 0;
	init();
}

CTimeManager::~CTimeManager()
{
}

void CTimeManager::init()
{
#if defined(OS_WINDOWS)
	_startTime = SystemCall::GetTickCount();
	_currentTime = SystemCall::GetTickCount();
#elif defined(OS_UNIX)
	_startTime = 0;
	_currentTime = 0;
	SystemCall::gettimeofday(&_TStart, &_TZ);
#endif

	localTime();

	memset(_timeBuffer, 0, 100);
}

TTime_t CTimeManager::nowSysTime()
{
	return _setTime;
}

TTime_t CTimeManager::SysNowTime()
{
	return CTimeManager::AnsiToGxTime(time(NULL));
}

uint32 CTimeManager::currentDate()
{
	localTime();
	uint32 Date;
	ConvertTU(&_tm, Date);

	return Date;
}


void CTimeManager::localTime()
{
	time((time_t*)&_setTime);
	tm tempTm;
	tm* ptm = CTimeManager::LocalTime((time_t*)&_setTime, &tempTm);
	_tm = *ptm;
}

// 得到标准时间
TTime_t CTimeManager::getANSITime()
{
	localTime();
	return _setTime;
}

uint32 CTimeManager::time2Number()
{
	localTime();

	uint32 uRet = 0;

	uRet += getYear();
	uRet -= 2000;
	uRet = uRet * 100;

	uRet += getMonth() + 1;
	uRet = uRet * 100;

	uRet += getDay();
	uRet = uRet * 100;

	uRet += getHour();
	uRet = uRet * 100;

	uRet += getMinute();

	return uRet;
}

uint32 CTimeManager::DiffTime(uint32 date1, uint32 date2)
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

void CTimeManager::ConvertUT(uint32 Date, tm* TM)
{
	memset(TM, 0, sizeof(tm));
	TM->tm_year = (Date >> 26) & 0xf;
	TM->tm_mon = (Date >> 22) & 0xf;
	TM->tm_mday = (Date >> 17) & 0x1f;
	TM->tm_hour = (Date >> 12) & 0x1f;
	TM->tm_min = (Date >> 6) & 0x3f;
	TM->tm_sec = (Date)& 0x3f;
}

void CTimeManager::ConvertTU(tm* TM, uint32& Date)
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

uint32 CTimeManager::getDayTime()
{
	time_t st;
	time(&st);
	tm tempTm;
	tm* ptm = CTimeManager::LocalTime((time_t*)&st, &tempTm);
	uint32 uRet = 0;
	uRet = (ptm->tm_year - 100) * 1000;
	uRet += ptm->tm_yday;

	return uRet;
}

uint32 CTimeManager::getTodayTime()
{
	tm tempTm;
	tm* ptm = CTimeManager::LocalTime((time_t*)&_setTime, &tempTm);
	uint32 uRet = 0;
	uRet = ptm->tm_hour * 100;
	uRet += ptm->tm_min;

	return uRet;
}

bool CTimeManager::FormatTodayTime(uint32& nTime)
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

TAppTime_t CTimeManager::update()
{
	getANSITime();

#if defined(OS_WINDOWS)
	_currentTime = GetTickCount();
#elif defined(OS_UNIX)
	gettimeofday(&_TEnd, &_TZ);
	double t1, t2;
	t1 = (double)_TStart.tv_sec * 1000 + (double)_TStart.tv_usec / 1000;
	t2 = (double)_TEnd.tv_sec * 1000 + (double)_TEnd.tv_usec / 1000;
	_currentTime = (uint32)(t2 - t1);
#endif

	strftime(_timeBuffer, 100, "%Y-%m-%d %H:%M:%S", &_tm);

	return _currentTime;
}

TTime_t CTimeManager::AnsiToGxTime(time_t times)
{
	return (TTime_t)times;
}

time_t CTimeManager::GxToAnsiTime(TTime_t times)
{
	return times;
}

void CTimeManager::FormatSystemTime(TTime_t times, std::string& str)
{
	static char cstime[100];
	time_t tt = CTimeManager::GxToAnsiTime(times);
	tm tempTm;
	tm* tms = CTimeManager::LocalTime((time_t*)&tt, &tempTm);
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

sint32 CTimeManager::getYear()
{
	return _tm.tm_year + 1900;
}

sint32 CTimeManager::getMonth()
{
	return _tm.tm_mon;
}

sint32 CTimeManager::getDay()
{
	return _tm.tm_mday;
}

sint32 CTimeManager::getHour()
{
	return _tm.tm_hour;
}

sint32 CTimeManager::getMinute()
{
	return _tm.tm_min;
}

sint32 CTimeManager::getSecond()
{
	return _tm.tm_sec;
}

sint32 CTimeManager::getWeek()
{
	return _tm.tm_wday;
}

struct tm * CTimeManager::LocalTime(const time_t *timep, struct tm *result)
{
#if defined(OS_WINDOWS)
	localtime_s(result, timep);
	return result;
#elif defined(OS_UNIX)
	return localtime_r(timep, result);
#endif
}