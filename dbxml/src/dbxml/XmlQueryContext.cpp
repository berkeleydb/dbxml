//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlQueryContext.hpp"
#include "dbxml/XmlManager.hpp"
#include "QueryContext.hpp"
#include "Results.hpp"

using namespace DbXml;
using namespace std;

static const char *className = "XmlQueryContext";
#define CHECK_POINTER checkNullPointer(queryContext_,className)

XmlQueryContext::XmlQueryContext()
	: queryContext_(0)
{
	
}

XmlQueryContext::XmlQueryContext(QueryContext *context)
  : queryContext_(context)
{
	if (queryContext_)
		queryContext_->acquire();
}

XmlQueryContext::~XmlQueryContext()
{
	if (queryContext_)
		queryContext_->release();
}

XmlQueryContext::XmlQueryContext(const XmlQueryContext &o)
	: queryContext_(o.queryContext_)
{
	if (queryContext_ != 0)
		queryContext_->acquire();
}

XmlQueryContext &XmlQueryContext::operator=(const XmlQueryContext &o)
{
	if (this != &o && queryContext_ != o.queryContext_) {
		if (queryContext_ != 0)
			queryContext_->release();
		queryContext_ = o.queryContext_;
		if (queryContext_ != 0)
			queryContext_->acquire();
	}
	return *this;
}

void XmlQueryContext::setVariableValue(const string &name,
				       const XmlValue &value)
{
	CHECK_POINTER;
	if (value.getType() == XmlValue::BINARY)
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlQueryContext::setVariableValue value cannot be binary");
	queryContext_->setVariableValue(name, new ValueResults(value,
							       queryContext_->getManager()));
}

bool XmlQueryContext::getVariableValue(const string &name,
				       XmlValue &value) const
{
	CHECK_POINTER;

	XmlResults results;
	bool success = queryContext_->getVariableValue(name, results);
	if(!success) return false;

	if(results.size() > 1) {
		throw XmlException(XmlException::INVALID_VALUE,
			"Variable has more than one value assigned to it");
	}

	results.reset();
	results.next(value);

	return true;
}

void XmlQueryContext::setVariableValue(const string &name,
				       XmlResults &value)
{
	CHECK_POINTER;
	queryContext_->setVariableValue(name, new ValueResults(
						value,
						queryContext_->getManager()));
}

bool XmlQueryContext::getVariableValue(const string &name,
				       XmlResults &value) const
{
	CHECK_POINTER;
	return queryContext_->getVariableValue(name, value);
}

void XmlQueryContext::setNamespace(const string &prefix, const string &uri )
{
	CHECK_POINTER;
	queryContext_->setNamespace(prefix, uri);
}

string XmlQueryContext::getNamespace(const string &prefix )
{
	CHECK_POINTER;
	return queryContext_->getNamespace(prefix);
}

void XmlQueryContext::removeNamespace(const string &prefix )
{
	CHECK_POINTER;
	queryContext_->removeNamespace(prefix);
}

void XmlQueryContext::clearNamespaces()
{
	CHECK_POINTER;
	queryContext_->clearNamespaces();
}

void XmlQueryContext::setBaseURI(const string &baseURI)
{
	CHECK_POINTER;
	queryContext_->setBaseURI(baseURI);
}

string XmlQueryContext::getBaseURI() const
{
	CHECK_POINTER;
	return queryContext_->getBaseURI();
}

void XmlQueryContext::setReturnType(ReturnType rt)
{
	CHECK_POINTER;
	queryContext_->setReturnType(rt);
}

XmlQueryContext::ReturnType XmlQueryContext::getReturnType() const
{
	CHECK_POINTER;
	return queryContext_->getReturnType();
}

void XmlQueryContext::setEvaluationType(EvaluationType et)
{
	CHECK_POINTER;
	queryContext_->setEvaluationType(et);
}

XmlQueryContext::EvaluationType XmlQueryContext::getEvaluationType() const
{
	CHECK_POINTER;
	return queryContext_->getEvaluationType();
}

void XmlQueryContext::setDefaultCollection(const std::string &uri)
{
	CHECK_POINTER;
	queryContext_->setDefaultCollection(uri);
}

string XmlQueryContext::getDefaultCollection() const
{
	CHECK_POINTER;
	return queryContext_->getDefaultCollection();
}


void XmlQueryContext::interruptQuery()
{
	queryContext_->interruptQuery();
}

void XmlQueryContext::setQueryTimeoutSeconds(u_int32_t secs)
{
	queryContext_->setQueryTimeoutSeconds(secs);
}

u_int32_t XmlQueryContext::getQueryTimeoutSeconds() const
{
	return queryContext_->getQueryTimeoutSeconds();
}

XmlDebugListener *XmlQueryContext::getDebugListener() const
{
	return queryContext_->getDebugListener();
}

void XmlQueryContext::setDebugListener(XmlDebugListener *listener)
{
	queryContext_->setDebugListener(listener);
}
