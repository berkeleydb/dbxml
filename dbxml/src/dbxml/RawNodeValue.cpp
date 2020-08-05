//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "RawNodeValue.hpp"
#include "DatabaseNodeValue.hpp"
#include "AtomicNodeValue.hpp"
#include "RawNodeValue.hpp"
#include "Results.hpp"
#include "nodeStore/NsDom.hpp"
#include "nodeStore/NsFormat.hpp"
#include "UTF8.hpp"
#include <dbxml/XmlResults.hpp>

using namespace DbXml;

#define GET_RESULTS() ((Results &) ((XmlResults&)results_))

RawNodeValue *RawNodeValue::create(const void *data, XmlResults &results)
{
	const xmlbyte_t *ptr = (const xmlbyte_t *)data;
	if (*ptr == DatabaseNodeType)
		return new DatabaseNodeValue(data, results);
	else
		return new AtomicNodeValue(data, results);
}

RawNodeValue *RawNodeValue::create(XmlValue &value, XmlResults &results)
{
	if (!value.isNode())
		throw XmlException(XmlException::INVALID_VALUE,
				   "RawNodeValue requires a node");
	DbXmlNodeValue *val = const_cast<DbXmlNodeValue*>(((Value*)value)->
							  getDbXmlNodeValueObject());
	DBXML_ASSERT(val);
	const Document *doc = val->getDocument();
	if (doc)
		return new DatabaseNodeValue(value, results);
	else
		return new AtomicNodeValue(value, results);
}

RawNodeValue::RawNodeValue()
	: type_(0), data_(0), dataSize_(0), returnResults_(0)
{}

RawNodeValue::RawNodeValue(XmlResults &results)
	: type_(0), data_(0), dataSize_(0), results_(results), returnResults_(0)
{}

RawNodeValue::~RawNodeValue()
{
	if (data_)
		NsUtil::deallocate(data_);
}

std::string RawNodeValue::getTypeURI() const
{
	switch(type_) {
	case nsNodeText:
	case nsNodeCDATA:
	case nsNodeAttr:
	case nsNodeElement:
	{
		return "http://www.w3.org/2001/XMLSchema";
	}
	default:
	{
		return "";
	}
	}
}

std::string RawNodeValue::getTypeName() const
{
	if (type_ == nsNodeElement)
		return XMLChToUTF8(DocumentCache::g_szUntyped).str();
	else if (type_ == nsNodeAttr || type_ == nsNodeText || type_ == nsNodeCDATA) {
		return "untypedAtomic";
		//return XMLChToUTF8(ATUntypedAtomic::fgDT_UNTYPEDATOMIC).str();
	}else
		return "";
}

Transaction *RawNodeValue::getTransaction() const
{
	Results &res = GET_RESULTS();
	return res.getOperationContext().txn();
}

