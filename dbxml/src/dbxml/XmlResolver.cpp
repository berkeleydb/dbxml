//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "dbxml/XmlResolver.hpp"

using namespace DbXml;
using namespace std;

bool XmlResolver::resolveDocument(XmlTransaction *txn, XmlManager &mgr,
	const string &uri, XmlValue &result) const
{
	return false;
}

bool XmlResolver::resolveCollection(XmlTransaction *txn, XmlManager &mgr,
	const string &uri, XmlResults &result) const
{
	return false;
}

XmlInputStream *XmlResolver::resolveSchema(XmlTransaction *txn, XmlManager &mgr,
	const std::string &schemaLocation, const std::string &nameSpace) const
{
	return 0;
}

XmlInputStream *XmlResolver::resolveEntity(XmlTransaction *txn, XmlManager &mgr,
	const std::string &systemId, const std::string &publicId) const
{
	return 0;
}

bool XmlResolver::resolveModuleLocation(XmlTransaction *txn, XmlManager &mgr,
	const std::string &nameSpace, XmlResults &result) const
{
	return false;
}

XmlInputStream *XmlResolver::resolveModule(XmlTransaction *txn, XmlManager &mgr,
	const std::string &moduleLocation, const std::string &nameSpace) const
{
	return 0;
}

XmlExternalFunction *XmlResolver::resolveExternalFunction(XmlTransaction *txn, XmlManager &mgr,
	const std::string &uri, const std::string &name, size_t numberOfArgs) const
{
	return 0;
}
