
#ifndef _STRING_COMMON_H_
#define _STRING_COMMON_H_

#include "types_def.h"

inline std::string mw_to_string(const char* format, ...)
{
	char content[2048];
	va_list args;
	va_start(args, format);
	int _res = vsnprintf(content, 2047, format, args);
	if (_res == -1 || _res == 2047) {
		content[2047] = '\0';
	}
	va_end(args);
	return content;
}

template <class T>
struct to_base
{
	static const T& convert(const T& p) { return p; }
};

template <>
struct to_base<std::string>
{
	static const char* convert(const std::string& p) { return p.c_str(); }
};

inline std::string gx_to_string(const char* fmt) { 
	return mw_to_string(fmt);
}

template <class T1>
inline std::string gx_to_string(const char* fmt, const T1& p1) {
	return mw_to_string(fmt, to_base<T1>::convert(p1));
}

template <class T1, class T2>
inline std::string gx_to_string(const char* fmt, const T1& p1, const T2& p2) {
	return mw_to_string(fmt, to_base<T1>::convert(p1), to_base<T2>::convert(p2));
}

template <class T1, class T2, class T3>
inline std::string gx_to_string(const char* fmt, const T1& p1, const T2& p2, const T3& p3) {
	return mw_to_string(fmt, to_base<T1>::convert(p1), to_base<T2>::convert(p2), to_base<T3>::convert(p3));
}

template <class T1, class T2, class T3, class T4>
inline std::string gx_to_string(const char* fmt, const T1& p1, const T2& p2, const T3& p3, const T4& p4) {
	return mw_to_string(fmt, to_base<T1>::convert(p1), to_base<T2>::convert(p2), to_base<T3>::convert(p3),
		to_base<T4>::convert(p4));
}

template <class T1, class T2, class T3, class T4, class T5>
inline std::string gx_to_string(const char* fmt, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
	const T5& p5) {
	return mw_to_string(fmt, to_base<T1>::convert(p1), to_base<T2>::convert(p2), to_base<T3>::convert(p3),
		to_base<T4>::convert(p4), to_base<T5>::convert(p5));
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
inline std::string gx_to_string(const char* fmt, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
	const T5& p5, const T6& p6) {
	return mw_to_string(fmt, to_base<T1>::convert(p1), to_base<T2>::convert(p2), to_base<T3>::convert(p3),
		to_base<T4>::convert(p4), to_base<T5>::convert(p5), to_base<T6>::convert(p6));
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline std::string gx_to_string(const char* fmt, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
	const T5& p5, const T6& p6, const T7& p7) {
	return mw_to_string(fmt, to_base<T1>::convert(p1), to_base<T2>::convert(p2), to_base<T3>::convert(p3),
		to_base<T4>::convert(p4), to_base<T5>::convert(p5), to_base<T6>::convert(p6), 
		to_base<T7>::convert(p7));
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline std::string gx_to_string(const char* fmt, const T1& p1, const T2& p2, const T3& p3, const T4& p4,
	const T5& p5, const T6& p6, const T7& p7, const T8& p8) {
	return mw_to_string(fmt, to_base<T1>::convert(p1), to_base<T2>::convert(p2), to_base<T3>::convert(p3),
		to_base<T4>::convert(p4), to_base<T5>::convert(p5), to_base<T6>::convert(p6), 
		to_base<T7>::convert(p7), to_base<T8>::convert(p8));
}


inline std::string gx_to_string(const uint8& val) { return gx_to_string("%hu", (uint16)val); }
inline std::string gx_to_string(const sint8& val) { return gx_to_string("%hd", (sint16)val); }
inline std::string gx_to_string(const uint16& val) { return gx_to_string("%hu", val); }
inline std::string gx_to_string(const sint16& val) { return gx_to_string("%hd", val); }
inline std::string gx_to_string(const uint32& val) { return gx_to_string("%u", val); }
inline std::string gx_to_string(const sint32& val) { return gx_to_string("%d", val); }
inline std::string gx_to_string(const uint64& val) { return gx_to_string("%" I64_FMT "u", val); }
inline std::string gx_to_string(const sint64& val) { return gx_to_string("%" I64_FMT "d", val); }
inline std::string gx_to_string(const float& val) { return gx_to_string("%f", val); }
inline std::string gx_to_string(const double& val) { return gx_to_string("%lf", val); }
inline std::string gx_to_string(const bool& val) { return gx_to_string("%u", val ? 1 : 0); }
inline std::string gx_to_string(const std::string& val) { return val; }

template <class T>
struct from_string
{
	static bool convert(const std::string& str, T& val) {
		if (str.find('-') != std::string::npos) {
			val = 0;
			return false;
		}

		char* end;
		unsigned long v;
		errno = 0;
		v = strtol(str.c_str(), &end, 10);
		if (errno || v > UINT_MAX || end == str.c_str()) {
			val = 0;
			return false;
		}
		else {
			val = (T)v;
			return true;
		}
	}
};


template <>
struct from_string<uint32>
{
	static bool convert(const std::string& str, uint32& val) {
		if (str.find('-') != std::string::npos) {
			val = 0;
			return false;
		}

		char* end;
		unsigned long v;
		errno = 0;
		v = strtoul(str.c_str(), &end, 10);
		if (errno || v > UINT_MAX || end == str.c_str()) {
			val = 0;
			return false;
		}
		else {
			val = (uint32)v;
			return true;
		}
	}
};

template <>
struct from_string<uint64>
{
	static bool convert(const std::string& str, uint64& val) {
		if (sscanf(str.c_str(), "%" I64_FMT "u", &val) == 1) {
			return true;
		}
		val = 0;
		return false;
	}
};

template <>
struct from_string<sint64>
{
	static bool convert(const std::string& str, sint64& val) {
		if (sscanf(str.c_str(), "%" I64_FMT "d", &val) == 1) {
			return true;
		}
		val = 0;
		return false;
	}
};

template <>
struct from_string<float>
{
	static bool convert(const std::string& str, float& val) {
		if (sscanf(str.c_str(), "%f", &val) == 1) {
			return true;
		}
		val = 0.0f;
		return false;
	}
};

template <>
struct from_string<double>
{
	static bool convert(const std::string& str, double& val) {
		if (sscanf(str.c_str(), "%lf", &val) == 1) {
			return true;
		}
		val = 0.0;
		return false;
	}
};

template <>
struct from_string<std::string>
{
	static bool convert(const std::string& str, std::string& val) {
		val = str;
		return true;
	}
};

#endif // !_STRING_COMMON_H_

