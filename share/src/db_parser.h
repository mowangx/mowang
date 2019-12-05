
#ifndef _DB_PARSER_H_
#define _DB_PARSER_H_

#include "base_util.h"
#include "debug.h"

static char	number_2_ascii(uint8 in_value)
{
	static const char number_2_asccii_array[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	if (in_value > 15) {
		gxAssert(false);
		return '?';
	}
	return number_2_asccii_array[in_value];
}

static uint8 ascii_2_number(char in_value)
{
	sint32 ascii_value = (sint32)in_value;
	if (ascii_value >= '0' && ascii_value <= '9') {
		return in_value - '0';	// 把字符‘0’到‘9’转化为相应的数字0-9
	}
	else if (ascii_value >= 'A' && ascii_value <= 'F') {
		return in_value - 55;	// 把字符‘A’到‘F’转化为相应的数字10－15
	}
	else {
		gxAssert(false);
		return '?';
	}
}

inline uint8 binary_char_2_value(int& index, const char* bstr)
{
	index += 2;
	return (ascii_2_number(bstr[index - 1]) << 4) + ascii_2_number(bstr[index - 2]);
}

inline void value_2_binary_char(uint8 n, char* buffer, int& index)
{
	for (uint32 i = 0; i<2; ++i) {
		// 高四位存储在低四位的后面，比如18，是把1存储在后4位中，2存储在前4位中,即0010 0001
		buffer[index] = number_2_ascii((n >> (4 * i)) & 0xF);
		++index;
	}
}

static void	change_2_hex(char* out_buff, const char* in_buff, uint32 len)
{
	uint32 index = 0;
	for (; index<len; ++index) {
		uint8 high_index = in_buff[index] >> 4;
		uint8 low_index = in_buff[index] & 0xF;
		out_buff[2 * index] = number_2_ascii(high_index);
		out_buff[2 * index + 1] = number_2_ascii(low_index);
	}
	out_buff[2 * index] = '\0';
}

template<class T>
inline bool parse_bstr(T& data, int index, uint32 len, const char* bstr)
{
	if (sizeof(data) != len) {
		log_error("Binary string length is not equal the struct size!!!");
		return false;
	}
	char* temp_data = (char*)&data;
	for (uint32 i = 0; i<len; ++i) {
		temp_data[i] = binary_char_2_value(index, bstr);
	}
	return true;
}

template<class T>
inline bool parse_dynamic_bstr(std::vector<T>& data, int index, uint32 len, const char* bstr)
{
	uint16 data_size = 0;
	if (len < sizeof(data_size)) {
		log_error("Binary string length is less than the struct size!!!");
		return false;
	}
	len -= sizeof(data_size);
	char* str_data_size = (char*)&data_size;
	for (int i = 0; i < sizeof(data_size); ++i) {
		str_data_size[i] = binary_char_2_value(index, bstr);
	}

	if (len < data_size * sizeof(T)) {
		log_error("Binary string length is less than the total dynamic struct size!!!");
		return false;
	}
	for (int i = 0; i < data_size; ++i) {
		T tmp_data;
		char* str_tmp_data = (char*)&tmp_data;
		for (uint32 i = 0; i<sizeof(tmp_data); ++i) {
			str_tmp_data[i] = binary_char_2_value(index, bstr);
		}
		data.push_back(tmp_data);
	}
	return true;
}

template <class T>
inline TDbVersion_t global_get_struct_version(const T& data) {
	// typeid(T) == typeid(XXX_struct)
	return 0;
}

template <class T>
inline bool global_bstr_2_struct(T& data, TDbVersion_t db_version, int index, uint16 len, const char* bstr) {
	return true;
}

#endif // !_DB_PARSER_H_

