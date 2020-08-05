//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __TYPECONVERSIONS_HPP
#define	__TYPECONVERSIONS_HPP

#include "DbXmlInternal.hpp"
#include <string>
#include <db.h>

#if defined(_MSC_VER)
// 1310 is Visual Studio .NET 2003
#if (_MSC_VER > 1310)
#define snprintf(b,s,f,arg) _snprintf_s((b), (s), (s), (f), (arg))
#else
#define snprintf _snprintf
#endif
#endif

namespace DbXml
{

inline std::string toString(int i)
{
	char buf[64];
	::snprintf(buf, 64, "%d", i);
	return std::string(buf);
}

inline std::string toString(u_int32_t i)
{
	char buf[64];
	::snprintf(buf, 64, "%u", i);
	return std::string(buf);
}

inline std::string toString(double d)
{
	char buf[128];
	::snprintf(buf, 128, "%0.17g", d);
	return std::string(buf);
}

inline std::string toString(uint64_t i)
{
	char buf[128];
	::snprintf(buf, 128, "%llu", (uint64_t)i);
	return std::string(buf);
}

// assumes buf is large enough	
inline void toChars(uint64_t i, char *buf, size_t buflen)
{
	::snprintf(buf, buflen, "%llu", (uint64_t)i);
}

// assumes buf is large enough	
inline void toChars(uint32_t i, char *buf, size_t buflen)
{
	::snprintf(buf, buflen, "%u", i);
}
	
inline int stringToInt(const std::string &s)
{
	return ::atoi(s.c_str());
}

}

#endif

