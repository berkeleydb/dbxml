//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

/*
 * NsTypes.hpp
 *
 * Core types for node storage
 */

#ifndef __DBXMLNSTYPES_HPP
#define __DBXMLNSTYPES_HPP

#include "../DbXmlInternal.hpp"
#include <xercesc/framework/MemoryManager.hpp>

// Put all Xerces forward decls in one place (TBD: find better place)
namespace XERCES_CPP_NAMESPACE {
	class MemoryManager;
};

/*
 * Atomic types for node storage.  Size matters.
 */
#include <db.h>
#define uint8_t u_int8_t
#define uint16_t u_int16_t
#define uint32_t u_int32_t
#define uint64_t u_int64_t
#define int64_t int64_t
typedef unsigned char xmlbyte_t; /* UTF-8 character */
/*
 * xmlch_t is typedef'd to XMLCh to handle
 * the fact that sometimes it'sunsigned short,
 * and sometimes a wchar_t (windows)
 */
typedef XMLCh xmlch_t;  /* UTF-16 character */

#endif
