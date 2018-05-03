
#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include "base_util.h"
#include "log.h"

#pragma pack(push, 1)

template <class T>
class dynamic_array
{
public:
	dynamic_array() {
		m_len = 0;
		m_max_len = 16;
		m_data = new T[m_max_len];
	}

	dynamic_array(const dynamic_array& rhs) {
		m_len = rhs.size();
		m_max_len = m_len + 1;
		m_data = new T[m_max_len];
		memcpy(m_data, rhs.data(), sizeof(T) * m_len);
	}

	dynamic_array& operator = (const dynamic_array& rhs) {
		if (this != &rhs) {
			delete[] m_data;
			m_len = rhs.size();
			m_max_len = m_len + 1;
			m_data = new T[m_max_len];
			memcpy(m_data, rhs.data(), sizeof(T) * m_len);
		}
		return *this;
	}

	~dynamic_array() {
		if (NULL != m_data) {
			delete[] m_data;
			m_data = NULL;
		}
	}

public:
	T* data() {
		return m_data;
	}

	const T* data() const {
		return m_data;
	}

	T& operator [] (size_t pos) {
		if (pos >= m_len) {
			log_error("dynamic array operator [] pos is larger than len! pos = %d, len = %d", pos, m_len);
			pos = 0;
		}
		return m_data[pos];
	}

	const T& operator [] (size_t pos) const {
		if (pos >= m_len) {
			log_error("dynamic array operator [] pos is larger than len! pos = %d, len = %d", pos, m_len);
			pos = 0;
		}
		return m_data[pos];
	}

	bool push_back(const T& data) {
		if ((m_len + 1) >= m_max_len) {
			T* tmp = new T[2 * m_max_len];
			memcpy(tmp, m_data, m_len * sizeof(T));
			delete[] m_data;
			m_data = tmp;
		}
		m_data[m_len] = data;
		m_len += 1;
		return true;
	}

	bool empty() const {
		return size() == 0;
	}

	uint16 size() const {
		return m_len;
	}

private:
	uint16 m_len;
	uint16 m_max_len;
	T* m_data;
};


class dynamic_string
{
public:
	dynamic_string() {
		m_len = 0;
		m_max_len = 64;
		m_data = new char[m_max_len];
		m_data[m_len] = '\0';
	}

	dynamic_string(char* s) {
		m_len = (uint16)strlen(s);
		m_max_len = m_len + 1;
		m_data = new char[m_max_len];
		memcpy(m_data, s, sizeof(char) * m_len);
		m_data[m_len] = '\0';
	}

	dynamic_string(const dynamic_string& rhs) {
		m_len = rhs.size();
		m_max_len = m_len + 1;
		m_data = new char[m_max_len];
		memcpy(m_data, rhs.data(), sizeof(char) * m_len);
		m_data[m_len] = '\0';
	}

	dynamic_string& operator = (const dynamic_string& rhs) {
		if (this != &rhs) {
			delete[] m_data;
			m_len = rhs.size();
			m_max_len = m_len + 1;
			m_data = new char[m_max_len];
			memcpy(m_data, rhs.data(), sizeof(char) * m_len);
			m_data[m_len] = '\0';
		}
		return *this;
	}

	~dynamic_string() {
		if (NULL != m_data) {
			delete[] m_data;
			m_data = NULL;
		}
	}

public:
	char * data() {
		return m_data;
	}

	const char* data() const {
		return m_data;
	}

	bool push_back(char data) {
		if ((m_len + 1) >= m_max_len) {
			char* tmp = new char[2 * m_max_len];
			memcpy(tmp, m_data, m_len * sizeof(char));
			delete[] m_data;
			m_data = tmp;
		}
		m_data[m_len] = data;
		m_len += 1;
		m_data[m_len] = '\0';
		return true;
	}

	bool empty() const {
		return size() == 0;
	}

	uint16 size() const {
		return m_len;
	}

private:
	uint16 m_len;
	uint16 m_max_len;
	char* m_data;
};

#pragma pack(pop)

#endif // !_DYNAMIC_ARRAY_H_

