
#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <mutex>

template <class T>
class Singleton
{
public:
	T* getInstance()
	{
		if (NULL != m_instance) {
			return m_instance;
		}

		m_mutex.lock();
		if (NULL != m_instance) {
			m_instance = new T();
		}
		m_mutex.unlock()
		return m_instance
	}

private:
	Singleton(const Singleton&) = 0;
	Singleton operator = (const Singlton&) = 0;

private:
	static T* m_instance;
	std::mutex m_mutex;
};

#endif