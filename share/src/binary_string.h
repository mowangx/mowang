
#ifndef _BINARY_STRING_H_
#define _BINARY_STRING_H_

#include "base_util.h"
#include "debug.h"
#include "time_manager.h"

struct db_struct_base
{
	TDbVersion_t db_version;

	db_struct_base() : db_version(INVALID_DB_VERSION)
	{

	}
};

template<typename T>
struct db_struct_parse
{
	bool parse_version(T& data, TDbVersion_t real_version, uint16 cur_index, uint16 total_index, const char* buffer)
	{
		return false;
	}
};

struct binary_string_head
{
	sint8 month;
	sint8 day;
	sint8 hour;
	sint8 minute;
	sint8 second;
	TDbVersion_t db_version;
	uint16 real_len;

	binary_string_head()
	{
		cleanUp();
	}

	void cleanUp()
	{
		month = 0;
		day = 0;
		hour = 0;
		minute = 0;
		second = 0;
		db_version = INVALID_DB_VERSION;
		real_len = 0;
	}
};


#define	GET_BINARY_MAX_LENGTH(leng)	(((leng + sizeof(binary_string_head)) << 1) + 1)

template <typename T>
struct binary_string : public std::array<char, GET_BINARY_MAX_LENGTH(sizeof(T))>
{
};

template <typename T, bool is_array>
struct parse_binary_string
{
	static bool change_struct_2_bstr(binary_string<T>& out_data, const T& data, bool is_empty);
};

template <typename T>
struct parse_binary_string<T, false>
{
	static bool change_struct_2_bstr(binary_string<T>& out_data, const T& data, bool is_empty)
	{
		uint32 index = 0;
		binary_string_head head;
		head.month = DTimeMgr.get_month() + 1;
		head.day = DTimeMgr.get_day();
		head.hour = DTimeMgr.get_hour();
		head.minute = DTimeMgr.get_minute();
		head.second = DTimeMgr.get_second();
		head.db_version = data.db_version;
		head.real_len = sizeof(data);
		uint32 head_len = sizeof(head);
		char* str_head = (char*)&head;
		for (uint32 i = 0; i<head_len; ++i) {
			number_2_binary_char(str_head[i], out_data.data(), index);
		}
		char* in = (char*)&data;
		for (uint32 i = 0; i<str_head.real_len; ++i) {
			number_2_binary_char(in[i], out_data.data(), index);
		}
		out_data[index] = '\0';
		return true;
	}
};

template <typename T>
struct parse_binary_string<T, true>
{
	static bool change_struct_2_bstr(binary_string<T>& out_data, const T& data, bool is_empty)
	{
		if (is_empty) {
			return true;
		}
		return parse_binary_string<T, false>::change_struct_2_bstr(out_data, data, is_empty);
	}
};

// �����ݿ���������ַ���ת������Ӧ�Ľṹ������
template<typename T>
inline bool bstr_2_struct(T& data, const char* bstr)
{
	if (strlen(bstr) == 0) {
		return true;
	}
	binary_string_head head;
	uint32 head_len = sizeof(head);
	if (strlen(bstr) <= head_len)
	{
		log_error("Binary string length is too short!!! It must larger than %u!", head_len);
		gxAssert(false);
		return false;	// ���ַ����ĳ���һ���ô��ڵ����ַ���ͷ�ĳ���
	}
	uint32 index = 0;
	char* str_head = (char*)&head;
	for (uint32 i = 0; i<head_len; ++i) {
		str_head[i] = BinaryCharToNumber(index, pBStr);
	}
	if (str_head.db_version == data.db_version)
	{
		return parse_bstr(data, index, str_head.real_len, bstr);
	}
	if (str_head.db_version > data.db_version)
	{
		gxError("Binary string database version is larger than current version!!! old version = %u, new version = %u", str_head.db_version, data.db_version);
		gxAssert(false);
		return false;
	}
	db_struct_parse<T> db_parse;
	return db_parse.parse_version(data, str_head.db_version, index, str_head.real_len, bstr);
}

// �ѽṹ��ת������Ӧ�Ķ������ַ���,isArrayΪtrue����ṹ������ֻ��һ���ɱ��б�(��CArray)
template <typename T, bool is_array>
inline bool	struct_2_bstr(binary_string<T>& out_data, const T& data, bool is_empty = false)
{
	return parse_binary_string<T, is_array>::change_struct_2_bstr(outData, data, is_empty);
}

inline uint8 binary_char_2_number(uint32& index, const char* bstr)
{
	index += 2;
	return (ascii_2_value(bstr[index - 1]) << 4) + ascii_2_value(bstr[index - 2]);
}

inline void number_2_binary_char(uint8 n, char* ary, uint32& index)
{
	for (uint32 i = 0; i<2; ++i)
	{
		// ����λ�洢�ڵ���λ�ĺ��棬����18���ǰ�1�洢�ں�4λ�У�2�洢��ǰ4λ��,��0010 0001
		ary[index] = value_2_ascii((n >> (4 * i)) & 0xF);
		++index;
	}
}

static char	value_2_ascii(uint8 in_value)
{
	static const char value_2_asccii_array[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	if (in_value > 15)
	{
		gxAssert(false);
		return '?';
	}
	return value_2_asccii_array[in_value];
}

static uint8 ascii_2_value(char in_value)
{
	sint32 ascii_value = (sint32)in_value;
	if (ascii_value >= '0' && ascii_value <= '9')
	{
		return in_value - '0';	// ���ַ���0������9��ת��Ϊ��Ӧ������0-9
	}
	else if (ascii_value >= 'A' && ascii_value <= 'F')
	{
		return in_value - 55;	// ���ַ���A������F��ת��Ϊ��Ӧ������10��15
	}
	else
	{
		gxAssert(false);
		return '?';
	}
}

static void	change_2_hex(char* outBuff, const char* inBuff, uint32 len)
{
	uint32 index = 0;
	for (; index<len; ++index)
	{
		uint8 highIndex = inBuff[index] >> 4;
		uint8 lowIndex = inBuff[index] & 0xF;
		outBuff[2 * index] = value_2_ascii(highIndex);
		outBuff[2 * index + 1] = value_2_ascii(lowIndex);
	}
	outBuff[2 * index] = '\0';
}

template<typename T>
inline bool parse_bstr(T& data, uint32 index, uint32 len, const char* bstr)
{
	if (sizeof(data) != len)
	{
		log_error("Binary string length is not equal the struct size!!!");
		return false;
	}
	char* temp_data = (char*)&data;
	for (uint32 i = 0; i<len; ++i)
	{
		temp_data[i] = binary_char_2_number(index, bstr);
	}
	return true;
}


#endif // !_BINARY_STRING_H_

