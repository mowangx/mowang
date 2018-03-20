
#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <mutex>

template <class T>
class CSingleton
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
	CSingleton()	{}

private:
	CSingleton(const CSingleton&) {}
	CSingleton operator = (const CSingleton&) {}

private:
	static T* m_instance;
	static std::mutex m_mutex;
};

template <class T>
T* CSingleton<T>::m_instance = NULL;

template <class T>
std::mutex CSingleton<T>::m_mutex;

#endif