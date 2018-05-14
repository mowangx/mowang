
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
			m_max_len <<= 1;
			T* tmp = new T[m_max_len];
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

	dynamic_string(const char* s, uint16 len = 0) {
		if (NULL != s) {
			if (len > 0) {
				m_len = len;
			}
			else {
				m_len = (uint16)strlen(s);
			}
			m_max_len = m_len + 1;
			m_data = new char[m_max_len];
			memcpy(m_data, s, sizeof(char) * m_len);
			m_data[m_len] = '\0';
		}
		else {
			m_len = 0;
			m_max_len = 64;
			m_data = new char[m_max_len];
			m_data[m_len] = '\0';
		}
	}

	dynamic_string(const dynamic_string& rhs) {
		m_len = rhs.size();
		m_max_len = m_len + 1;
		m_data = new char[m_max_len];
		memcpy(m_data, rhs.data(), sizeof(char) * m_len);
		m_data[m_len] = '\0';
	}

	dynamic_string& operator = (const dynamic_string& rhs) {
		if (&rhs != this) {
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
		m_len = 0;
		m_max_len = 0;
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
			m_max_len <<= 1;
			char* tmp = new char[m_max_len];
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

class dynamic_string_array
{
public:
	dynamic_string_array() {
		m_len = 0;
		m_real_len = 0;
		m_max_len = 16;
		m_data = new char[m_max_len];
	}

	dynamic_string_array(const dynamic_string_array& rhs) {
		m_len = rhs.size();
		m_real_len = rhs.real_len();
		m_max_len = m_real_len + 1;
		m_data = new char[m_max_len];
		memcpy(m_data, rhs.data(), sizeof(char) * m_real_len);
		m_data[m_real_len] = '\0';
	}

	dynamic_string_array& operator = (const dynamic_string_array& rhs) {
		if (&rhs != this) {
			delete[] m_data;
			m_len = rhs.size();
			m_real_len = rhs.real_len();
			m_max_len = m_real_len + 1;
			m_data = new char[m_max_len];
			memcpy(m_data, rhs.data(), sizeof(char) * m_real_len);
			m_data[m_real_len] = '\0';
		}
		return *this;
	}

	~dynamic_string_array() {
		m_len = 0;
		m_real_len = 0;
		m_max_len = 0;
		if (NULL != m_data) {
			delete[] m_data;
			m_data = NULL;
		}
	}

public:
	char* data() {
		return m_data;
	}

	const char* data() const {
		return m_data;
	}

	dynamic_string operator [] (size_t pos) const {
		uint16 len = 0;
		for (int i = 0; i < pos; ++i) {
			uint16 cur_len = *(uint16*)(m_data + len);
			len += cur_len;
		}
		uint16 cur_len = *(uint16*)(m_data + len);
		dynamic_string s((m_data + len), cur_len);
		return s;
	}

	bool push_back(const dynamic_string& data) {
		if (((uint32)m_real_len + data.size()) >= m_max_len) {
			m_max_len = m_real_len + data.size() + 1;
			char* tmp = new char[m_max_len];
			memcpy(tmp, m_data, m_real_len);
			delete[] m_data;
			m_data = tmp;
		}
		memcpy(m_data + m_real_len, data.data(), data.size());
		m_len += 1;
		m_real_len += data.size();
		m_data[m_len] = '\0';
		return true;
	}

	bool empty() const {
		return size() == 0;
	}

	uint16 size() const {
		return m_len;
	}

	uint16 real_len() const {
		return m_real_len;
	}

private:
	uint16 m_len;
	uint16 m_real_len;
	uint32 m_max_len;
	char* m_data;
};

#pragma pack(pop)

#endif // !_DYNAMIC_ARRAY_H_

