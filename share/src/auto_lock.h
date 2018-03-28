
#ifndef _AUTO_LOCK_H_
#define _AUTO_LOCK_H_

#include <mutex>

class auto_lock
{
public:
	auto_lock(std::mutex* m): m_mutex(m)
	{
		m_mutex->lock();
	}

	~auto_lock()
	{
		m_mutex->unlock();
	}
	

private:
	std::mutex* m_mutex;
};

#endif