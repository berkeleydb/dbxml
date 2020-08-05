//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __VARIABLEQP_HPP
#define	__VARIABLEQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

/**
 * Returns the value of a variable
 */
class VariableQP : public QueryPlan
{
public:
	VariableQP(const XMLCh *prefix, const XMLCh *uri, const XMLCh *name, ContainerBase *container,
		DbXmlNodeTest *nodeTest, u_int32_t flags, XPath2MemoryManager *mm);

	DbXmlNodeTest *getNodeTest() const { return nodeTest_; }
	ContainerBase *getContainerBase() const { return container_; }

	const XMLCh *getURI() { return uri_; }
	const XMLCh *getName() { return name_; }

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
	const XMLCh *prefix_;
	const XMLCh *uri_;
	const XMLCh *name_;

	ContainerBase *container_;
	DbXmlNodeTest *nodeTest_;
};

}

#endif
