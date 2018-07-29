
#ifndef _AUTO_LOCK_H_
#define _AUTO_LOCK_H_

#include <mutex>
#include <queue>
#include <condition_variable>

class auto_lock
{
public:
	auto_lock(std::mutex* m): m_mutex(m) {
		m_mutex->lock();
	}

	~auto_lock() {
		m_mutex->unlock();
	}
	

private:
	std::mutex* m_mutex;
};

template <class T>
class share_queue
{
public:
	void push(const T& value) {
		std::lock_guard<std::mutex> lock(&m_mutex);
		m_queue.push(value);
		m_condition.notify_one();
	}

	void push(T&& value) {
		std::lock_guard<std::mutex> lock(&m_mutex);
		m_queue.push(std::move(value));
		m_condition.notify_one();
	}

	void pop(T& value) {
		std::lock_guard<std::mutex> lock(&m_mutex);
		m_condition.wait(lock, [this] {
			return !m_queue.empty();
		});
		value = std::move(m_queue.front());
		m_queue.pop();
	}

	bool try_pop(T& value) {
		std::lock_guard<std::mutex> lock(&m_mutex);
		if (m_queue.empty()) {
			return false;
		}
		value = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}

	bool empty() {
		std::lock_guard<std::mutex> lock(&m_mutex);
		return m_queue.empty();
	}

	bool size() {
		std::lock_guard<std::mutex> lock(&m_mutex);
		return m_queue.size();
	}

private:
	std::mutex m_mutex;
	std::condition_variable m_condition;
	std::queue<T> m_queue;
};

#endif