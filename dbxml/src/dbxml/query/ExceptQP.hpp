//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __EXCEPTQP_HPP
#define	__EXCEPTQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class ExceptQP : public QueryPlan
{
public:
	ExceptQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	QueryPlan *getLeftArg() const { return left_; }
	void setLeftArg(QueryPlan *a) { left_ = a; }
	QueryPlan *getRightArg() const { return right_; }
	void setRightArg(QueryPlan *a) { right_ = a; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

private:
	QueryPlan *left_;
	QueryPlan *right_;
};

class ExceptIterator : public ProxyIterator
{
public:
	ExceptIterator(NodeIterator *left, NodeIterator *right, const LocationInfo *location);
	~ExceptIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doJoin(DynamicContext *context);

	NodeIterator *left_;
	NodeIterator *right_;
	bool toDo_;
};

}

#endif
