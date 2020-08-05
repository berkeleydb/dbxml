//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <stdlib.h>
#include <iostream>

#include "DbXmlInternal.hpp"
#include <dbxml/XmlException.hpp>

using namespace DbXml;
using namespace std;

DBXML_EXPORT void DbXml::assert_fail(const char *expression, const char *file, int line)
{
#ifdef DEBUG
	cerr << "Assertion failed: " << expression << ", " << file << ":" << line << endl;
	abort();
#else
	throw XmlException(XmlException::INTERNAL_ERROR, string("Assertion failed: ") + expression, file, line);
#endif
}
