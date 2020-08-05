//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLINTERNAL_HPP
#define __DBXMLINTERNAL_HPP

#if defined(_MSC_VER)
// versions check:
// we don't support Visual C++ prior to version 6:
#if (_MSC_VER<1200)
#error "Compiler version not supported"
#endif // _MSC_VER<1200

#if (_MSC_VER > 1300)
#define strdup _strdup
#endif

// turn off the warnings before we #include anything
#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#pragma warning( disable : 4503 ) // warning: decorated name length exceeded
#pragma warning( disable : 4290 ) // warning: C++ exception specification ignored
// this next warning is hard to eliminate, even if _strdup() is used, so
// just shut it up
#pragma warning( disable : 4996 ) // warning: deprecation of strdup() in VS8
//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)
// VC6 warning about truncating debug symbol identifiers to 255 chars
#pragma warning (disable : 4786)

#endif

#include "dbxml_config.h"
#include "dbxml/XmlPortability.hpp"
#include <string>
#include "dbxml/XmlException.hpp"
// DB flags
#include <db.h>

//Allocation alignment on different platforms
#if (defined(__HP_aCC) && defined(__ia64)) || defined(__sparcv9)
#define ALLOC_ALIGN 16
#elif defined(__sparcv8)
#define ALLOC_ALIGN 8
#else
#define ALLOC_ALIGN 4
#endif


// define DB flags if not done, so that 4.3-4.5 are all
// supported

// new in 4.5
#ifndef DB_TXN_SNAPSHOT
#define DB_TXN_SNAPSHOT 0x10000000
#define DB_MULTIVERSION 0x008
#endif

// changed in 4.4
#ifndef DB_READ_COMMITTED
#define DB_READ_COMMITTED DB_DEGREE_2
#define DB_READ_UNCOMMITTED DB_DIRTY_READ
#endif

#define DBVER (DB_VERSION_MAJOR * 10 + DB_VERSION_MINOR)
#if DB_VERSION_MINOR > 9
#error "cannot handle db minor version numbers > 9"
#endif
#define	USECS_PER_SEC 1000000
#define	NSECS_PER_USEC 1000

//
// Use the timer abstraction from DB, since it's platform independent.
//
#if DBVER < 46
extern "C" int __os_clock(DB_ENV *, u_int32_t *, u_int32_t *);
#define DB_GETTIME(secs, usecs) __os_clock(NULL, &(secs), &(usecs))
#else
// This is not pretty, but... DB doesn't export db_timespec.
// it's declared in it's dbinc/clock.h file, internal to the source tree.
// Keep an eye on this...

typedef struct {
        time_t  tv_sec;                         /* seconds */
        long    tv_nsec;                        /* nanoseconds */
} db_timespec;

// DB keeps changing the API...
#if DBVER == 46
extern "C" int __os_gettime(DB_ENV *, db_timespec *);
#define CALL_GETTIME __os_gettime(NULL, &ts)
#else
extern "C" int __os_gettime(DB_ENV *, db_timespec *, int);
#define CALL_GETTIME __os_gettime(NULL, &ts, 1)
#endif

#define DB_GETTIME(secs, usecs)			\
{                                               \
	db_timespec ts;				\
	CALL_GETTIME;				\
	secs = ts.tv_sec;			\
	usecs = ts.tv_nsec/NSECS_PER_USEC;	\
}
#endif

namespace DbXml
{

DBXML_EXPORT void assert_fail(const char *expression, const char *file, int line);

static inline void checkNullPointer(const void *ptr, const char *message)
{
	if (ptr == 0) {
		std::string mesg = "Attempt to use uninitialized object: ";
		mesg += message;
		throw XmlException(XmlException::NULL_POINTER, mesg);
	}
}
	
}

#ifdef ENABLE_ASSERTS
#  define DBXML_ASSERT(expr)((void)((expr) ? 0 : (assert_fail(#expr, __FILE__, __LINE__), 0)))
#else
#  define DBXML_ASSERT(expr)((void)(0))
#endif

#endif
