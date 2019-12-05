
#ifndef _STRING_PARSE_H_
#define _STRING_PARSE_H_

#include <vector>
#include "base_util.h"
#include "string_common.h"

template <class T>
class string_parse
{
public:
	typedef std::vector<T> value_type;

public:
	string_parse(const std::string& s, const std::string& split) {
		m_split = split;
		m_fields.clear();
		char* split_token = strtok((char*)s.c_str(), m_split.c_str());
		while (NULL != split_token)
		{
			std::string field(split_token);
			m_fields.push_back(field);
			split_token = strtok(NULL, m_split.c_str());
		}
	}

public:
	bool convert(value_type& value) {
		for (int i = 0; i < m_fields.size(); ++i) {
			T cur_val;
			from_string<T>::convert(m_fields[i], cur_val);
			value.push_back(cur_val);
		}
		return value.empty() ? false: true;
	}

	uint32 size() const {
		return m_fields.size();
	}

	const std::string& operator [](int index) const {
		return m_fields[index];
	}

private:
	std::string m_split;
	std::vector<std::string> m_fields;
};

template <class T>
class string_parse_2
{
private:
	typedef std::vector<T> value_type_1;
public:
	typedef std::vector<value_type_1> value_type;

public:
	string_parse_2(const std::string& s, const std::string& split_1, const std::string& split_2) {
		m_split_1 = split_1;
		m_split_2 = split_2;
		m_fields.clear();
		string_parse<T> field(s, m_split_1);
		for (int i = 0; i < field.size(); ++i) {
			m_fields.push_back(field[i]);
		}
	}

public:
	bool convert(value_type& value) {
		for (int i = 0; i < m_fields.size(); ++i) {
			string_parse<T> field(m_fields[i], m_split_2);
			value_type_1 ary;
			field.convert(ary);
			value.push_back(ary);
			if (i > 0 && ary.size() != value[0].size()) {
				return false;
			}
		}
		return value.empty() ? false : true;
	}

	uint32 size() const {
		return m_fields.size();
	}

	const std::string& operator [](int index) const {
		return m_fields[index];
	}

private:
	std::string m_split_1;
	std::string m_split_2;
	std::vector<std::string> m_fields;
};


template <class T>
class string_parse_3
{
private:
	typedef std::vector<T> value_type_1;
	typedef std::vector<value_type_1> value_type_2;

public:
	typedef std::vector<value_type_2> value_type;
public:
	string_parse_3(const std::string& s, const std::string& split_1, const std::string& split_2, const std::string& split_3) {
		m_split_1 = split_1;
		m_split_2 = split_2;
		m_split_3 = split_3;
		m_fields.clear();
		string_parse<T> field(s, m_split_1);
		for (int i = 0; i < field.size(); ++i) {
			m_fields.push_back(field[i]);
		}
	}

public:
	bool convert(value_type& value) {
		for (int i = 0; i < m_fields.size(); ++i) {
			value_type_2 ary_2;
			string_parse<T> field_2(m_fields[i], m_split_2);
			for (int j = 0; j < field_2.size(); ++j) {
				string_parse<T> field_1(field_2[j], m_split_3);
				value_type_1 ary_1;
				field_1.convert(ary_1);
				ary_2.push_back(ary_1);
				if (j > 0 && ary_1.size() != ary_2[0].size()) {
					return false;
				}
			}
			value.push_back(ary_2);
			if (i > 0 && ary_2.size() != value[0].size()) {
				return false;
			}
		}
		return value.empty() ? false : true;
	}

	uint32 size() const {
		return m_fields.size();
	}

	const std::string& operator [](int index) const {
		return m_fields[index];
	}

private:
	std::string m_split_1;
	std::string m_split_2;
	std::string m_split_3;
	std::vector<std::string> m_fields;
};


#endif // !_STRING_PARSE_H_

