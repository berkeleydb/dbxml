//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLFWD_HPP
#define	__DBXMLFWD_HPP

//BEGIN-DBXML-VERSION
/* DO NOT EDIT: automatically built by dist/s_include. */
#define DBXML_VERSION_MAJOR 2
#define DBXML_VERSION_MINOR 5
#define DBXML_VERSION_PATCH 16
#define DBXML_VERSION_STRING "Oracle: Berkeley DB XML 2.5.16: (December 22, 2009)"
//END-DBXML-VERSION

#include "XmlPortability.hpp"

namespace DbXml
{

class XmlContainer;
class XmlManager;
class XmlDocument;
class XmlException;
class XmlQueryContext;
class XmlUpdateContext;
class XmlResults;
class XmlValue;
class XmlQueryExpression;
class XmlIndexSpecification;
class XmlResolver;
class XmlData;
class XmlTransaction;
class XmlInputStream;
class XmlMetaDataIterator;
class XmlStatistics;
class XmlEventReader;
class XmlEventWriter;	
class XmlCompression;
class XmlContainerConfig;
class XmlArguments;
class XmlExternalFunction;
class XmlStackFrame;
class XmlDebugListener;

//
// Flags:
//
// DEVELOPER'S NOTES:
// 1.  These flags have specific values to (1) avoid
//    conflicts with Berekeley DB flags and (2) be used
//    along side them by users. When DB flags change,
//    these flags may need to, as well.
// 2.  When values are changed and/or new constants are
//     added, it is necessary to reflect these changes in:
//     o dist/swig/dbxml.i (values are reproduced, and apply
//       to Python, Tcl, and Java interfaces	
//     o src/php/php_dbxml.cpp (constants are declared)
//     o src/Perl/DbXml/Makefile.PL (constants are declared).
//       After modifying Makefile.PL, it's necessary to run
//       "perl Makefile.PL" to regenerate const-c.inc as well.	
//
enum Flags {
	// XmlManager construction flags
	DBXML_ADOPT_DBENV           = 0x00000001, ///< take ownership of DbEnv
	DBXML_ALLOW_EXTERNAL_ACCESS = 0x00000002, ///< allow FS and net access
	DBXML_ALLOW_AUTO_OPEN       = 0x00000004, ///< auto-open in queries
	
	// these next three are only used by XmlContainer::lookupIndex, and can
	// safely re-use the preceding enumeration values.
	DBXML_REVERSE_ORDER         = 0x00100000, ///< return in reverse sort
	DBXML_INDEX_VALUES          = 0x00200000, ///< return values also
	DBXML_CACHE_DOCUMENTS       = 0x00400000, ///< ensure that two index entries that refer to the same document return the exact same XmlDocument object

	DBXML_LAZY_DOCS             = 0x00800000, ///< lazily materialize docs
	DBXML_DOCUMENT_PROJECTION   = 0x80000000, ///< Use the document projection optimisation
	DBXML_NO_AUTO_COMMIT        = 0x00020000, ///< Do not auto transact the operation
	// below used for putDocument, and query operations (reuses enum)
	DBXML_WELL_FORMED_ONLY      = 0x01000000, ///< well-formed parser only
	// only used in putDocument, safe to reuse enum above
	DBXML_GEN_NAME              = 0x02000000  ///< generate name in putDoc
};

enum LogLevel
{
	LEVEL_NONE =    0x00000000,  ///< No debug levels are enabled.
	LEVEL_DEBUG =   0x00000001,  ///< Program execution tracing messages.
	LEVEL_INFO =    0x00000002,  ///< Informational, just for interest.
	LEVEL_WARNING = 0x00000004,  ///< For warnings, bad things but recoverable.
	LEVEL_ERROR =   0x00000008,  ///< For errors that can't be recovered from.
	LEVEL_ALL =     0xFFFFFFFF   ///< All debug levels are enabled.
};

enum LogCategory
{
	CATEGORY_NONE =       0x00000000,
	CATEGORY_INDEXER =    0x00000001,
	CATEGORY_QUERY =      0x00000002,
	CATEGORY_OPTIMIZER =  0x00000004,
	CATEGORY_DICTIONARY = 0x00000008,
	CATEGORY_CONTAINER =  0x00000010,
	CATEGORY_NODESTORE =  0x00000020,
	CATEGORY_MANAGER =    0x00000040,
	CATEGORY_ALL =        0xFFFFFFFF
};

DBXML_EXPORT const char *dbxml_version(int *majorp, int *minorp, int *patchp);

DBXML_EXPORT const char *dbxml_db_version(int *majorp, int *minorp, int *patchp);

DBXML_EXPORT void setLogLevel(LogLevel level, bool enabled);

DBXML_EXPORT void setLogCategory(LogCategory category, bool enabled);
}

#endif
