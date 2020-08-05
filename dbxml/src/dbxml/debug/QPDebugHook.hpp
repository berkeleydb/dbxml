//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QPDEBUGHOOK_HPP
#define	__QPDEBUGHOOK_HPP

#include "../query/FilterQP.hpp"
#include "../query/NodeIterator.hpp"
#include <dbxml/XmlDebugListener.hpp>

#include <xqilla/context/DynamicContext.hpp>

namespace DbXml
{

#define GET_DEBUGLISTENER(x) (GET_CONFIGURATION(x)->getQueryContext().getDebugListener())

class QPDebugHook : public FilterQP
{
public:
	QPDebugHook(QueryPlan *arg, u_int32_t flags, XPath2MemoryManager *mm);

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;
};

class StackFrameImpl : public XmlStackFrame
{
public:
	virtual const char *getQueryFile() const;
	virtual int getQueryLine() const;
	virtual int getQueryColumn() const;

	virtual XmlResults query(const std::string &queryString) const;

	virtual const XmlStackFrame *getPreviousStackFrame() const
	{
		return prev_;
	}

	void setPreviousFrame(DynamicContext *context);

	DynamicContext *getContext() const
	{
		return context_;
	}

protected:
	StackFrameImpl(const LocationInfo *location, DynamicContext *context);

	const LocationInfo *location_;
	mutable std::string file_;

	DynamicContext *context_;
	Item::Ptr contextItem_;
	size_t contextPosition_;
	size_t contextSize_;
	const VariableStore *variables_;
	const XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathNSResolver *nsResolver_;
	const XMLCh *defaultElementNS_;

	const XmlStackFrame *prev_;
};

}

#endif
