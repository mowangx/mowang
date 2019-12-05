
#ifndef _XML_LOADER_H_
#define _XML_LOADER_H_

#include <string>
#include <unordered_map>

#include "xpath_static.h"
#include "log.h"
#include "string_parse.h"

#define ConfigRow TiXmlElement

template <class T>
class config_loader
{
public:
	config_loader()
	{
		m_data.clear();
	}

public:
	T* get(uint32 key_id)
	{
		auto itr = m_data.find(key_id);
		if (itr != m_data.end()) {
			return itr->second;
		}
		return nullptr;
	}

	bool add(T* value)
	{
		if (nullptr == value) {
			return false;
		}

		if (m_data.find(value->get_key()) != m_data.end()) {
			log_error("Repeat key! key = %u", value->get_key());
			return false;
		}

		m_data[value->get_key()] = value;
		return true;
	}

public:
	bool load(const std::string& name)
	{
		TiXmlDocument*XPRoot = new TiXmlDocument();
		if (!XPRoot->LoadFile(name.c_str())) {
			log_error("Can't open xml file! name = %s", name.c_str());
			return false;
		}

		TiXmlElement* element = XPRoot->RootElement()->FirstChildElement();
		uint32 count = 0;
		while (nullptr != element) {
			T* row = new T();
			if (nullptr == row) {
				log_error("Can't new %s! Line=%u", typeid(T).name(), count);
				return false;
			}

			if (!read_row(element, count, row)) {
				return false;
			}

			if (false == row->on_after_load()) {
				return false;
			}

			element = element->NextSiblingElement();
			count++;
		}

		return on_after_load();
	}

	template <class ValueType>
	bool load_field_int(ValueType& value, ConfigRow* row, const std::string field_name) {
		int cur_val;
		if (nullptr == row->Attribute(field_name.c_str())) {
			return false;
		}
		if (nullptr == row->Attribute(field_name.c_str(), (int*)&cur_val)) {
			return false; 
		}
		value = cur_val;
		return true;
	}

	template <class ValueType>
	bool load_field_ary(typename string_parse<ValueType>::value_type& value, ConfigRow* row, const std::string field_name, const std::string& split) {
		string_parse<ValueType> parser(row->Attribute(field_name.c_str()), split);
		return parser.convert(value);
	}


	template <class ValueType>
	bool load_field_ary_2(typename string_parse_2<ValueType>::value_type& value, ConfigRow* row, const std::string field_name, const std::string& split_1, const std::string& split_2) {
		string_parse_2<ValueType> parser(row->Attribute(field_name.c_str()), split_1, split_2);
		return parser.convert(value);
	}


	template <class ValueType>
	bool load_field_ary_3(typename string_parse_3<ValueType>::value_type& value, ConfigRow* row, const std::string field_name, const std::string& split_1, const std::string& split_2, const std::string& split_3) {
		string_parse_3<ValueType> parser(row->Attribute(field_name.c_str()), split_1, split_2, split_3);
		return parser.convert(value);
	}

	virtual bool on_after_load()
	{
		return true;
	}

	virtual bool read_row(ConfigRow* row, sint32 count, T* val) = 0;

private:
	std::unordered_map<uint32, T*> m_data;
};

#endif