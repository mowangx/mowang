
#ifndef _BINARY_STRING_H_
#define _BINARY_STRING_H_

#include "db_parser.h"
#include "time_manager.h"
#include "dynamic_array.h"

struct binary_string_head
{
	sint8 month;
	sint8 day;
	sint8 hour;
	sint8 minute;
	sint8 second;
	TDbVersion_t db_version;
	uint16 real_len;

	binary_string_head() {
		cleanUp();
	}

	void cleanUp() {
		month = 0;
		day = 0;
		hour = 0;
		minute = 0;
		second = 0;
		db_version = INVALID_DB_VERSION;
		real_len = 0;
	}
};

static dynamic_string* allocate_binary_string(uint16 size) {
	size = ((size + sizeof(binary_string_head)) << 1) + 1;
	return new dynamic_string(NULL, size);
}

static void create_db_head(char* buffer, int& index, TDbVersion_t db_version, uint16 len) {
	binary_string_head head;
	head.month = DTimeMgr.get_month() + 1;
	head.day = DTimeMgr.get_day();
	head.hour = DTimeMgr.get_hour();
	head.minute = DTimeMgr.get_minute();
	head.second = DTimeMgr.get_second();
	head.db_version = db_version;
	head.real_len = len;
	char* str_head = (char*)&head;
	for (uint32 i = 0; i<sizeof(head); ++i) {
		value_2_binary_char(str_head[i], buffer, index);
	}
}

template <class T>
static void flat_struct_2_bstr(char* buffer, const T& data)
{
	uint16 len = sizeof(data);
	TDbVersion_t db_version = global_get_struct_version(data); 
	int index = 0;
	create_db_head(buffer, index, db_version, len);
	char* in = (char*)&data;
	for (uint16 i = 0; i<len; ++i) {
		value_2_binary_char(in[i], buffer, index);
	}
	buffer[index] = '\0';
	++index;
}

template <class T>
static void dynamic_struct_2_bstr(char* buffer, const dynamic_array<T>& data)
{
	if (data.empty()) {
		return ;
	}

	int index = 0;
	TDbVersion_t db_version = global_get_struct_version(data);
	uint16 len = sizeof(uint16) + sizeof(T) * data.size();
	create_db_head(buffer, index, db_version, len);
	for (uint32 i = 0; i < data.size(); ++i) {
		char* in = (char*)&data[i];
		for (int j = 0; j < sizeof(data[i]); ++j) {
			value_2_binary_char(in[j], buffer, index);
		}
	}
	buffer[index] = '\0';
	++index;
}

static bool parse_db_head(binary_string_head& head, const char* bstr, int& index) {
	uint32 head_len = sizeof(head);
	if (strlen(bstr) <= head_len) {
		log_error("Binary string length is too short!!! It must larger than %u!", head_len);
		gxAssert(false);
		return false;	// 该字符串的长度一定得大于等于字符串头的长度
	}
	char* str_head = (char*)&head;
	for (uint32 i = 0; i<head_len; ++i) {
		str_head[i] = binary_char_2_value(index, bstr);
	}
	return true;
}

// 把数据库读到到的字符串转化成相应的结构体数据
template<class T>
inline bool bstr_2_flat_struct(T& data, const char* bstr)
{
	if (strlen(bstr) == 0) {
		return true;
	}
	int index = 0;
	binary_string_head head;
	parse_db_head(head, bstr, index);
	TDbVersion_t db_version = global_get_struct_version(data);
	if (head.db_version == db_version) {
		return parse_bstr(data, index, head.real_len, bstr);
	}
	if (head.db_version > db_version) {
		log_error("Binary string database version is larger than current version!!! old version = %u, new version = %u", head.db_version, db_version);
		gxAssert(false);
		return false;
	}
	return global_bstr_2_struct(data, head.db_version, index, head.real_len, bstr);
}

// 把数据库读到到的字符串转化成相应的结构体数据
template<class T>
inline bool bstr_2_dynamic_struct(dynamic_array<T>& data, const char* bstr)
{
	if (strlen(bstr) == 0) {
		return true;
	}
	int index = 0;
	binary_string_head head;
	parse_db_head(head, bstr, index);
	TDbVersion_t db_version = global_get_struct_version(data);
	if (head.db_version == db_version) {
		return parse_dynamic_bstr(data, index, head.real_len, bstr);
	}
	if (head.db_version > db_version) {
		log_error("Binary string database version is larger than current version!!! old version = %u, new version = %u", head.db_version, db_version);
		gxAssert(false);
		return false;
	}
	return global_bstr_2_struct(data, head.db_version, index, head.real_len, bstr);
}

#endif // !_BINARY_STRING_H_

