//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __CONTEXTNODEQP_HPP
#define	__CONTEXTNODEQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

/**
 * Returns the context node
 */
class ContextNodeQP : public QueryPlan
{
public:
	ContextNodeQP(ContainerBase *container, DbXmlNodeTest *nodeTest, u_int32_t flags, XPath2MemoryManager *mm)
		: QueryPlan(CONTEXT_NODE, flags, mm), container_(container), nodeTest_(nodeTest) {}

	DbXmlNodeTest *getNodeTest() const { return nodeTest_; }
	ContainerBase *getContainerBase() const { return container_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

private:
	ContainerBase *container_;
	DbXmlNodeTest *nodeTest_;
};

class ContextNodeIterator : public DbXmlNodeIterator
{
public:
	ContextNodeIterator(const LocationInfo *location)
		: DbXmlNodeIterator(location), toDo_(true) {}

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	bool toDo_;
};

}

#endif
