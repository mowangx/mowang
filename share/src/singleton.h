
#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <mutex>

template <class T>
class singleton
{
public:
	static T& get_instance()
	{
		return *(get_instance_ptr());
	}

	static T* get_instance_ptr() {
		if (NULL != m_instance) {
			return m_instance;
		}

		m_mutex.lock();
		// maybe other thread has create the instance
		if (NULL == m_instance) {
			m_instance = new T();
		}
		m_mutex.unlock();
		return m_instance;
	}

protected:
	singleton()	{}

private:
	singleton(const singleton&) {}
	singleton& operator = (const singleton&) {}

private:
	static T* m_instance;
	static std::mutex m_mutex;
};

template <class T>
T* singleton<T>::m_instance = NULL;

template <class T>
std::mutex singleton<T>::m_mutex;

#endif