
#ifndef _INI_FILE_H_
#define _INI_FILE_H_

#include "base_util.h"
#include "string_common.h"

class ini_file
{
public:
	ini_file();
	ini_file(const char* filename);
	ini_file(void* dataBuff, sint32 len);
    virtual ~ini_file();

public:
    char* get_data();
    sint32 get_lines(sint32);
    sint32 get_lines();

    bool open(const char* filename);
	bool open(void* dataBuff, sint32 len);
    void close();
    bool save(char *filename=NULL);
    sint32 find_index(const char *);

private:
    void init_index();
    sint32 find_data(sint32, const char *);
    sint32 goto_next_line(sint32);
    char* read_data_name(sint32 &);
    char* read_text(sint32);

    bool add_index(const char *);
    bool add_data(sint32, const char *, const char *);
    bool modity_data(sint32, const char *, const char *);
    sint32 goto_last_line(const char *section);

public:
    sint32 read_int(const char *section, const char *key);
    bool read_int_if_exist(const char *section, const char *key, sint32& nResult);

    template<typename T>
    bool read_type_if_exist(const char* section, const char*key, T& result);

    sint32 read_int(const char *section, sint32 lines);
    char* read_text(const char *section, const char *key, char* str, sint32 size);
    bool read_text_if_exist(const char *section, const char *key, char* str, sint32 size);
    bool read_text_if_exist(const char *section, const char *key, std::string& str);
    char* read_text(const char *section, sint32 lines, char* str, sint32 size);
    char* read_caption(const char *section, sint32 lines, char* str, sint32 size);
    bool write(const char *section, const char *key, sint32 num);
    bool write(const char *section, const char *key, char *string);
    sint32 get_continue_data_num(const char *section);	
    char* read_one_line(sint32);
    sint32 find_one_line(sint32);
    sint32 return_line_num(const char*);

private:
	template <class T>
	T* realloc_array(T* data, uint32 src_len, uint32 dest_len);

private:
	char m_filename[MAX_PATH];		//文件名
	sint32 m_data_len;					//文件长度
	char* m_data;						//文件内容
	sint32 m_index_num;					//索引数目([]的数目)
	sint32* m_index_list;					//索引点位置列表
	sint32 m_point;						//当前指针
	sint32 m_line, m_word;				//当前行列, 临时值
	char m_value[MAX_INI_VALUE];		//	值
	char m_ret[MAX_INI_VALUE];		//	返回值
};

template<typename T>
T* ini_file::realloc_array(T* data, uint32 src_len, uint32 dest_len)
{
	T* temp = new T[dest_len];
	memset(temp, 0, sizeof(dest_len));
	memcpy(temp, data, src_len * sizeof(T));
	return temp;
}

template<typename T>
bool ini_file::read_type_if_exist( const char* section, const char* key, T& result )
{
	sint32 n = find_index(section);
	if( n == -1 ) {
		return false;
	}

	sint32 m = find_data(n, key);
	if( m == -1 ) {
		return false;
	}

	char* str = read_text(m);
	from_string<T>::convert(str, result);

	return true;
}

#endif // !_INI_FILE_H_