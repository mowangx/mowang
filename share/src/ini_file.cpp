
#include "ini_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "debug.h"

#define ERROR_DATA      -99999999

//初始化
ini_file::ini_file()
{
	m_data_len = 0;
	m_data = NULL;
	m_index_num = 0;
	m_index_list = NULL;
}

//初始化
ini_file::ini_file(const char *filename)
{
	m_data_len = 0;
	m_data = NULL;
	m_index_num = 0;
	m_index_list = NULL;
	memset(m_filename, 0, MAX_PATH);
	memset(m_value, 0, MAX_INI_VALUE);
	memset(m_ret, 0, MAX_INI_VALUE);

	open(filename);
}

ini_file::ini_file(void* dataBuff, sint32 len)
{
	m_data_len = 0;
	m_data = NULL;
	m_index_num = 0;
	m_index_list = NULL;

	m_data = new char[len + 1];
	memset(m_data, 0, len + 1);
	memcpy(m_data, dataBuff, len);
	m_data_len = len;
	//初始化索引
	init_index();
}

//析构释放
ini_file::~ini_file()
{
	if (m_data_len != 0)
	{
		DSafeDeleteArray(m_data);
		m_data_len = 0;
	}

	if (m_index_num != 0)
	{
		DSafeDeleteArray(m_index_list);
		m_index_num = 0;
	}
}

//读入文件
bool ini_file::open(const char *filename)
{
	strcpy(m_filename, filename);

	DSafeDeleteArray(m_data);

	//获取文件长度
	FILE* fp;
	fp = fopen(filename, "rb");
	if (fp == 0)
	{
		m_data_len = -1;
	}
	else
	{
		fseek(fp, 0L, SEEK_END);
		m_data_len = ftell(fp);
		fclose(fp);
	}


	//文件存在
	if (m_data_len > 0)
	{
		m_data = new char[m_data_len];
		memset(m_data, 0, m_data_len);

		FILE *fp;
		fp = fopen(filename, "rb");
		gxAssert(fp != NULL);
		fread(m_data, m_data_len, 1, fp);		//读数据
		fclose(fp);

		//初始化索引
		init_index();
		return true;
	}
	else	// 文件不存在
	{
		// 找不到文件
		m_data_len = 1;
		m_data = new char[m_data_len];
		memset(m_data, 0, 1);
		init_index();
	}

	return false;
}

bool ini_file::open(void* dataBuff, sint32 len)
{
	m_data = new char[len + 1];
	memset(m_data, 0, len + 1);
	memcpy(m_data, dataBuff, len);
	m_data_len = len;
	//初始化索引
	init_index();
	return true;
}

//关闭文件
void ini_file::close()
{
	if (m_data_len != 0)
	{
		DSafeDeleteArray(m_data);
		m_data_len = 0;
	}

	if (m_index_num != 0)
	{
		DSafeDeleteArray(m_index_list);
		m_index_num = 0;
	}
}

//写入文件
bool ini_file::save(char *filename)
{
	if (filename == NULL)
	{
		filename = m_filename;
	}

	FILE *fp;
	fp = fopen(filename, "wb");
	gxAssert(fp != NULL);

	fwrite(m_data, m_data_len, 1, fp);
	fclose(fp);

	return true;
}

//返回文件内容
char *ini_file::get_data()
{
	return m_data;
}

//获得文件的行数
sint32 ini_file::get_lines(sint32 cur)
{
	sint32 n = 1;
	for (sint32 i = 0; i<cur; i++)
	{
		if (m_data[i] == '\n')
			n++;
	}
	return n;
}

//获得文件的行数
sint32 ini_file::get_lines()
{
	sint32		n = 0;
	sint32		i;
	for (i = 0; i<m_data_len; i++)
	{
		if (m_data[i] == '\n')
			n++;
	}

	if (i >= m_data_len)
	{
		return n + 1;
	}

	return n;
}

////////////////////////////////////////////////
// 内部函数
////////////////////////////////////////////////

//计算出所有的索引位置
void ini_file::init_index()
{
	m_index_num = 0;

	for (sint32 i = 0; i<m_data_len; i++)
	{
		//找到
		if (m_data[i] == '[' && (i == 0 || m_data[i - 1] == '\n'))
		{
			m_index_num++;
		}
	}

	//申请内存
	DSafeDeleteArray(m_index_list);
	if (m_index_num>0)
		m_index_list = new sint32[m_index_num];

	sint32 n = 0;

	for (sint32 i = 0; i<m_data_len; i++)
	{
		if (m_data[i] == '[' && (i == 0 || m_data[i - 1] == '\n'))
		{
			m_index_list[n] = i + 1;
			n++;
		}
	}
}

//返回指定标题位置
sint32 ini_file::find_index(const char *string)
{
	for (sint32 i = 0; i<m_index_num; i++)
	{
		char *str = read_text(m_index_list[i]);
		if (strcmp(string, str) == 0)
		{
			return m_index_list[i];
		}
	}
	return -1;
}

//返回指定数据的位置
sint32 ini_file::find_data(sint32 index, const char *string)
{
	sint32 p = index;	//指针

	while (1)
	{
		p = goto_next_line(p);
		char *name = read_data_name(p);
		if (strcmp(string, name) == 0)
		{
			DSafeDeleteArray(name);
			return p;
		}

		if (name[0] == '[')
		{
			DSafeDeleteArray(name);
			return -1;
		}

		DSafeDeleteArray(name);
		if (p >= m_data_len) return -1;
	}
	return -1;
}

//提行
sint32 ini_file::goto_next_line(sint32 p)
{
	sint32 i;
	for (i = p; i<m_data_len; i++)
	{
		if (m_data[i] == '\n')
			return i + 1;
	}
	return i;
}

//在指定位置读一数据名称
char *ini_file::read_data_name(sint32 &p)
{
	char chr;
	char *Ret;
	sint32 m = 0;

	Ret = new char[64];
	memset(Ret, 0, 64);

	for (sint32 i = p; i<m_data_len; i++)
	{
		chr = m_data[i];

		//结束
		if (chr == '\r')
		{
			p = i + 1;
			return Ret;
		}

		//结束
		if (chr == '=' || chr == ';')
		{
			p = i + 1;
			return Ret;
		}

		Ret[m] = chr;
		m++;
	}
	return Ret;
}

//在指定位置读一字符串
char *ini_file::read_text(sint32 p)
{
	char chr;
	char *Ret;
	sint32 n = p, m = 0;

	sint32 LineNum = goto_next_line(p) - p + 1;
	Ret = (char*)m_value;
	memset(Ret, 0, MAX_INI_VALUE);

	for (sint32 i = 0; i<m_data_len - p; i++)
	{
		chr = m_data[n];

		//结束
		if (chr == ';' || chr == '\r' || chr == '\t' || chr == ']')
		{
			return Ret;
		}

		Ret[m] = chr;
		m++;
		n++;
	}

	return Ret;
}

//加入一个索引
bool ini_file::add_index(const char *index)
{
	char str[256];
	memset(str, 0, 256);
	sint32 n = find_index(index);

	if (n == -1)	//新建索引
	{
		sprintf(str, "\r\n[%s]", index);
		m_data = realloc_array(m_data, m_data_len, m_data_len + (uint32)strlen(str));	//重新分配内存
		sprintf(&m_data[m_data_len], "%s", str);
		m_data_len += (uint32)(strlen(str));

		init_index();
		return true;
	}

	return false;	//已经存在
}

//在当前位置加入一个数据
bool ini_file::add_data(sint32 p, const char *name, const char *string)
{
	char *str;
	sint32 len = (sint32)(strlen(string));
	str = new char[len + 256];
	memset(str, 0, len + 256);
	sprintf(str, "%s=%s", name, string);
	len = (sint32)(strlen(str));

	p = goto_next_line(p);	//提行
	m_data = realloc_array(m_data, m_data_len, m_data_len + len);	//重新分配内存

	char *temp = new char[m_data_len - p];
	memcpy(temp, &m_data[p], m_data_len - p);
	memcpy(&m_data[p + len], temp, m_data_len - p);	//把后面的搬到末尾
	memcpy(&m_data[p], str, len);
	m_data_len += len;

	DSafeDeleteArray(temp);
	DSafeDeleteArray(str);
	return true;
}

//在当前位置修改一个数据的值
bool ini_file::modity_data(sint32 p, const char *name, const char *string)
{
	sint32 n = find_data(p, name);

	char *t = read_text(n);
	p = n + (sint32)(strlen(t));

	sint32 newlen = (sint32)(strlen(string));
	sint32 oldlen = p - n;

	m_data = realloc_array(m_data, m_data_len, m_data_len + newlen - oldlen);	//重新分配内存

	char *temp = new char[m_data_len - p];
	memcpy(temp, &m_data[p], m_data_len - p);
	memcpy(&m_data[n + newlen], temp, m_data_len - p);			    //把后面的搬到末尾
	memcpy(&m_data[n], string, newlen);
	m_data_len += newlen - oldlen;

	DSafeDeleteArray(temp);
	return true;
}

//把指针移动到本INDEX的最后一行
sint32 ini_file::goto_last_line(const char *index)
{
	sint32 n = find_index(index);
	n = goto_next_line(n);
	while (1)
	{
		if (m_data[n] == '\r' || m_data[n] == EOF || m_data[n] == -3 || m_data[n] == ' ' || m_data[n] == '/' || m_data[n] == '\t' || m_data[n] == '\n')
		{
			return n;
		}
		else
		{
			n = goto_next_line(n);
			if (n >= m_data_len) return n;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// 对外接口
/////////////////////////////////////////////////////////////////////

//以普通方式读一字符串数据
char *ini_file::read_text(const char *index, const char *name, char* str, sint32 size)
{
	char szTmp[512];
	memset(szTmp, 0, 512);
	sprintf(szTmp, "[%s][%s][%s]", m_filename, index, name);

	sint32 n = find_index(index);
	gxAssert(n != -1);

	if (n == -1)
		return NULL;

	sint32 m = find_data(n, name);
	gxAssert(m != -1);
	if (m == -1)
		return NULL;

	char* ret = read_text(m);
	strncpy(str, ret, size);
	return ret;
}

//如果存在则读取
bool ini_file::read_text_if_exist(const char *index, const char *name, char* str, sint32 size)
{
	sint32 n = find_index(index);

	if (n == -1)
	{
		return false;
	}

	sint32 m = find_data(n, name);

	if (m == -1)
	{
		return false;
	}

	char* ret = read_text(m);
	strncpy(str, ret, size);
	return true;
}

bool ini_file::read_text_if_exist(const char *section, const char* key, std::string& str)
{
	sint32 n = find_index(section);

	if (n == -1)
	{
		return false;
	}

	sint32 m = find_data(n, key);

	if (m == -1)
	{
		return false;
	}

	char* ret = read_text(m);
	str = ret;
	return true;
}

//在指定的行读一字符串
char *ini_file::read_text(const char *index, sint32 lines, char* str, sint32 size)
{
	char szTmp[512];
	memset(szTmp, 0, 512);
	sprintf(szTmp, "[%s][%s][%d]", m_filename, index, lines);


	sint32 n = find_index(index);
	gxAssert(n != -1);

	//跳到指定行数
	n = goto_next_line(n);
	for (sint32 i = 0; i<lines; i++)
	{
		if (n<m_data_len)
			n = goto_next_line(n);
	}

	//读数据
	while (n <= m_data_len)
	{
		if (m_data[n] == '=')
		{
			n++;
			char* ret = read_text(n);
			strncpy(str, ret, size);
			return ret;
		}
		if (m_data[n] == '\r')
		{
			return NULL;
		}
		n++;
	}

	return NULL;
}

//以普通方式读一整数
sint32 ini_file::read_int(const char *index, const char *name)
{
	char szTmp[512];
	memset(szTmp, 0, 512);
	sprintf(szTmp, "[%s][%s][%s]", m_filename, index, name);

	sint32 n = find_index(index);
	gxAssert(n != -1);

	sint32 m = find_data(n, name);
	gxAssert(m != -1);

	char *str = read_text(m);
	sint32 ret = atoi(str);
	return ret;
}

bool ini_file::read_int_if_exist(const char *section, const char *key, sint32& nResult)
{
	sint32 n = find_index(section);

	if (n == -1)
		return false;

	sint32 m = find_data(n, key);

	if (m == -1)
		return false;

	char *str = read_text(m);
	nResult = atoi(str);
	return true;
}

//在指定的行读一整数
sint32 ini_file::read_int(const char *index, sint32 lines)
{
	char szTmp[512];
	memset(szTmp, 0, 512);
	sprintf(szTmp, "[%s][%s][%d]", m_filename, index, lines);

	sint32 n = find_index(index);
	gxAssert(n != -1);

	//跳到指定行数
	n = goto_next_line(n);
	for (sint32 i = 0; i<lines; i++)
	{
		if (n<m_data_len)
			n = goto_next_line(n);
	}

	//读数据
	while (n<m_data_len)
	{
		if (m_data[n] == '=')
		{
			n++;
			char *str = read_text(n);
			sint32 ret = atoi(str);
			return ret;
		}
		if (m_data[n] == '\r')
		{
			return ERROR_DATA;
		}
		n++;
	}

	return ERROR_DATA;
}

//在指定的行读一数据名称
char *ini_file::read_caption(const char *index, sint32 lines, char* str, sint32 size)
{


	char szTmp[512];
	memset(szTmp, 0, 512);
	sprintf(szTmp, "[%s][%s][%d]", m_filename, index, lines);

	sint32 n = find_index(index);
	gxAssert(n != -1);

	//跳到指定行数
	n = goto_next_line(n);
	for (sint32 i = 0; i<lines; i++)
	{
		if (n<m_data_len)
			n = goto_next_line(n);
	}

	char* ret = read_data_name(n);
	strncpy(str, ret, size);
	return ret;
}

//以普通方式写一字符串数据
bool ini_file::write(const char *index, const char *name, char *string)
{
	sint32 n = find_index(index);
	if (n == -1)	//新建索引
	{
		add_index(index);
		n = find_index(index);
		n = goto_last_line(index);
		add_data(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在索引
	sint32 m = find_data(n, name);
	if (m == -1)		//新建数据
	{
		n = goto_last_line(index);
		add_data(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在数据
	modity_data(n, name, string);	//修改一个数据

	return true;
}

//以普通方式写一整数
bool ini_file::write(const char *index, const char *name, sint32 num)
{
	char string[32];
	sprintf(string, "%d", num);

	sint32 n = find_index(index);
	if (n == -1)	//新建索引
	{
		add_index(index);
		n = find_index(index);
		n = goto_last_line(index);
		add_data(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在索引
	sint32 m = find_data(n, name);
	if (m == -1)		//新建数据
	{
		n = goto_last_line(index);
		add_data(n, name, string);	//在当前位置n加一个数据
		return true;
	}

	//存在数据
	modity_data(n, name, string);	//修改一个数据

	return true;
}

//返回连续的行数
sint32 ini_file::get_continue_data_num(const char *index)
{
	sint32 num = 0;
	sint32 n = find_index(index);
	n = goto_next_line(n);
	while (1)
	{
		if (m_data[n] == '\r' || m_data[n] == EOF || m_data[n] == -3 || m_data[n] == ' ' || m_data[n] == '/' || m_data[n] == '\t' || m_data[n] == '\n')
		{
			return num;
		}
		else
		{
			num++;
			n = goto_next_line(n);
			if (n >= m_data_len) return num;
		}
	}

	return 0;
}
//在指定行读一字符串
char* ini_file::read_one_line(sint32 p)
{
	sint32 start = find_one_line(p);
	memset(m_ret, 0, sizeof(m_ret));


	for (sint32 i = start; i<m_data_len; i++)
	{
		if (m_data[i] == '\n' || m_data[i] == '\0')
		{
			memset(m_ret, 0, sizeof(m_ret));
			strncpy(m_ret, &m_data[start], i - start);
			m_ret[i - start] = '\0';
			return m_ret;
		}

	}

	return m_ret;
}

sint32 ini_file::find_one_line(sint32 p)
{
	sint32		n = 0;
	if (p == 0)	return -1;
	if (p == 1)	return 0;
	for (sint32 i = 0; i<m_data_len; i++)
	{
		if (m_data[i] == '\n')
		{
			n++;
		}
		if (n == p - 1)				//找到要了要找的的行
		{
			return i + 1;
		}
	}

	return -1; //没有找到
}

sint32 ini_file::return_line_num(const char* string)
{
	sint32 p = find_index(string);
	sint32		n = 0;
	if (p == 0)	return -1;
	if (p == 1)	return 0;
	for (sint32 i = 0; i<p; i++)
	{
		if (m_data[i] == '\n')
			n++;
	}
	return n;

	return -1; //没有找到
}