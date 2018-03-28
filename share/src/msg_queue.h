#ifndef _MSG_QUEUE_H_
#define _MSG_QUEUE_H_

#include <mutex>
#include <deque>
#include <list>

#include "types_def.h"
#include "auto_lock.h"

template<typename T>
class msg_queue
{
private:
	//! Lock access to the queue.
	std::mutex             _mutex;

	//! Storage backing the queue.
	std::deque<T>           _queue;

	//
	sint32					_queSize;
public:

	msg_queue()
	{
		_queSize = 0;

	}

	virtual ~msg_queue()
	{
	}

	void push(std::list<T>& tempList, sint32 num = MAX_SINT32_NUM)
	{
		if (num == 0 || tempList.empty())
		{
			return;
		}

		do
		{
			auto_lock lock(&_mutex);
			typename std::list<T>::iterator iter = tempList.begin();
			sint32 count = 0;
			for (; iter != tempList.end() && count < num; count++, iter++)
			{
				_queue.push_back(*iter);
			}
			_queSize = _queue.size();
		} while (false);
	}

	void pop(std::list<T>& lst, sint32 num = MAX_SINT32_NUM)
	{
		if (empty())
		{
			return;
		}

		auto_lock lock(&_mutex);
		sint32 nCount = 0;
		while (!empty() && nCount < num)
		{
			nCount++;
			lst.push_back(_queue.front());
			_queue.pop_front();
			_queSize = (sint32)_queue.size();
		}
	}

	void push(const T& msg)
	{
		auto_lock lock(&_mutex);
		_queue.push_back(msg);
		_queSize = (sint32)_queue.size();
	}

	void push_front(const T& msg)
	{
		auto_lock lock(&_mutex);
		_queue.push_front(msg);
		_queSize = _queue.size();
	}

	bool pop(T& msg)
	{
		if (empty())
		{
			return false;
		}

		auto_lock lock(&_mutex);
		if (!empty())
		{
			msg = _queue.front();
			_queue.pop_front();
			_queSize = _queue.size();
			return true;
		}

		return false;
	}

	inline uint32 size()
	{
		return _queSize;
	}

	bool empty()
	{
		return _queue.empty();
	}
};

#endif