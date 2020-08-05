//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlNamespace.hpp"

namespace DbXml
{
// NOTE: if these change value, or number, or in any way, dist/swig/dbxml.i
// must be modified for non-C++ language access
const DBXML_EXPORT char *metaDataNamespace_uri = "http://www.sleepycat.com/2002/dbxml";
const DBXML_EXPORT char *metaDataNamespace_prefix = "dbxml";

const DBXML_EXPORT char *metaDataName_name = "name";
const DBXML_EXPORT char *metaDataName_root = "root";

}
