
#ifndef _AUTO_LOCK_H_
#define _AUTO_LOCK_H_

#include <mutex>

class CLock
{
public:
	CLock(std::mutex* m): m_mutex(m)
	{
		m_mutex->lock();
	}

	~CLock()
	{
		m_mutex->unlock();
	}
	

private:
	std::mutex* m_mutex;
};

#endif