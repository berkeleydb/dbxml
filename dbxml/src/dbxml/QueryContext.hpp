//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYCONTEXT_HPP
#define	__QUERYCONTEXT_HPP

#include <string>
#include <map>
#include "NamespaceMap.hpp"
#include "VariableBindings.hpp"
#include "dbxml/XmlQueryContext.hpp"
#include "ReferenceCounted.hpp"
#include "dbxml/XmlManager.hpp"

class XPath2MemoryManager;
class XPathSelectionTreeParser;
class XPathNSResolver;
class StaticContext;
class DynamicContext;
class FunctionLookup;

namespace DbXml
{

class XmlValue;
class QueryInterrupt;
	
class QueryContext : public ReferenceCounted
{
public:
	/// Constructor.
	QueryContext(XmlManager &mgr, XmlQueryContext::ReturnType rt, XmlQueryContext::EvaluationType et);
	virtual ~QueryContext();

	void setNamespace( const std::string &prefix, const std::string &uri );
	std::string getNamespace( const std::string &prefix );
	void removeNamespace( const std::string &prefix );
	void clearNamespaces();

	void setVariableValue(const std::string &name, const XmlResults &value);
	bool getVariableValue(const std::string &name, XmlResults &value) const;

	void setBaseURI(const std::string &baseURI);
	std::string getBaseURI() const { return baseURI_; }

	void interruptQuery();
	void setQueryTimeoutSeconds(u_int32_t secs);
	u_int32_t getQueryTimeoutSeconds() const;
	void startQuery();

	void setReturnType(XmlQueryContext::ReturnType rt = XmlQueryContext::LiveValues)
	{
		returnType_ = rt;
	}
	XmlQueryContext::ReturnType getReturnType() const
	{
		return returnType_;
	}
	void setEvaluationType(XmlQueryContext::EvaluationType et = XmlQueryContext::Eager)
	{
		evaluationType_ = et;
	}
	XmlQueryContext::EvaluationType getEvaluationType() const
	{
		return evaluationType_;
	}
	void setDefaultCollection(const std::string &uri);
	std::string getDefaultCollection() const
	{
		return defaultCollection_;
	}
	XmlManager &getManager()
	{
		return mgr_;
	}

	QueryContext *copy() const
	{
		return new QueryContext(*this);
	}

	XmlDebugListener *getDebugListener() const { return listener_; }
	void setDebugListener(XmlDebugListener *listener) { listener_ = listener; }

	// Private stuff.
	void populateStaticContext(StaticContext *context);
	void populateDynamicContext(DynamicContext *context);
	void testInterrupt();

private:
	QueryContext(const QueryContext&);
	// no need for assignment
	QueryContext &operator=(const QueryContext &);

	NamespaceMap namespaces_;
	VariableBindings variables_;
	std::string baseURI_;
	XmlQueryContext::ReturnType returnType_;
	XmlQueryContext::EvaluationType evaluationType_;
	std::string defaultCollection_;
	XmlManager mgr_;
	QueryInterrupt *qInt_;
	XmlDebugListener *listener_;
};

}

#endif

