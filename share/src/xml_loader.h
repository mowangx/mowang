
#ifndef _XML_LOADER_H_
#define _XML_LOADER_H_

#include <string>
#include <unordered_map>

#include "xpath_static.h"
#include "log.h"

#define ConfigRow TiXmlElement

#define DReadConfigInt(attr, val, tabRow) \
val = 0; \
if (NULL == row->Attribute(#attr)) { \
	log_error("Can't parse "#attr"!Line=%u", count); \
	return false; \
} \
if (NULL == row->Attribute(#attr, (int*)&val)) { \
	log_error("Can't parse "#attr"!Line=%u", count); \
	return false; \
} \
tabRow->attr = val;

template <class T>
class CConfigLoader
{
public:
	CConfigLoader()
	{
		m_data.clear();
	}

public:
	T* get(uint32 key_id)
	{
		std::unordered_map<uint32, T*>::iterator itr = m_data.find(key_id);
		if (itr != m_data.end()) {
			return itr->second;
		}
		return NULL;
	}

	bool add(T* value)
	{
		if (NULL == value) {
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
		while (NULL != element) {
			T* row = new T();
			if (NULL == row) {
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

	virtual bool on_after_load()
	{
		return true;
	}

	virtual bool read_row(ConfigRow* row, sint32 count, T* val) = 0;

private:
	std::unordered_map<uint32, T*> m_data;
};

#endif