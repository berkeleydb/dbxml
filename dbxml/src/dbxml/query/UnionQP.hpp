//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __UNIONQP_HPP
#define	__UNIONQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class UnionQP : public OperationQP
{
public:
	UnionQP(u_int32_t flags, XPath2MemoryManager *mm)
		: OperationQP(QueryPlan::UNION, flags, mm) {}

	UnionQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual bool isSupersetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

private:
	void removeSubsets(OptimizationContext &opt);
	void combineAltArgs(std::vector<QueryPlans>::iterator argIt, std::vector<QueryPlans>::iterator argEnd,
		std::vector<QueryPlan*> &newArgs, OptimizationContext &opt, QueryPlans &combinations) const;
};

class UnionIterator : public ProxyIterator
{
public:
	UnionIterator(NodeIterator *left, NodeIterator *right, const LocationInfo *location);
	~UnionIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	bool doJoin(DynamicContext *context);

	NodeIterator *left_;
	NodeIterator *right_;

	enum {
		LEFT = 0x01,
		RIGHT = 0x02,
		BOTH = (LEFT | RIGHT),
		NEITHER = 0
	} next_;
};

}

#endif
