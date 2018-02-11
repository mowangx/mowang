
#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <mutex>

template <class T>
class Singleton
{
public:
	static T& getInstance()
	{
		if (NULL != m_instance) {
			return *m_instance;
		}

		m_mutex.lock();
		// maybe other thread has create the instance
		if (NULL == m_instance) {
			m_instance = new T();
		}
		m_mutex.unlock();
		return *m_instance;
	}

protected:
	Singleton()	{}

private:
	Singleton(const Singleton&) {}
	Singleton operator = (const Singleton&) {}

private:
	static T* m_instance;
	static std::mutex m_mutex;
};

template <class T>
T* Singleton<T>::m_instance = NULL;

template <class T>
std::mutex Singleton<T>::m_mutex;

#endif