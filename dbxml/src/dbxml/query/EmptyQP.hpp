//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __EMPTYQP_HPP
#define	__EMPTYQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class EmptyQP : public QueryPlan
{
public:
	EmptyQP(u_int32_t flags, XPath2MemoryManager *mm)
		: QueryPlan(QueryPlan::EMPTY, flags, mm) {}

	virtual void staticTypingLite(StaticContext *context) {}
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper) { return this; }
	virtual QueryPlan *optimize(OptimizationContext &opt) { return this; }

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const { return Cost(); }

	virtual void findQueryPlanRoots(QPRSet &qprset) const {}
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;
};

class EmptyIterator : public NodeIterator
{
public:
	EmptyIterator(const LocationInfo *location);

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

	virtual Type getType() const;

	virtual int getContainerID() const;
	virtual DocID getDocID() const;

	virtual const NsNid getNodeID() const;
	virtual const NsNid getLastDescendantID() const;
	virtual u_int32_t getNodeLevel() const;
	virtual u_int32_t getIndex() const;
	virtual bool isLeadingText() const;

	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context);
};

}

#endif
