//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __INTERSECTQP_HPP
#define	__INTERSECTQP_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

class IntersectQP : public OperationQP
{
public:
	IntersectQP(u_int32_t flags, XPath2MemoryManager *mm)
		: OperationQP(QueryPlan::INTERSECT, flags, mm) {}

	IntersectQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm);

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

	static std::string logIntersectBefore(const QueryPlan *left, const QueryPlan *right);

private:
	void removeSupersets(OptimizationContext &opt);
	QueryPlan *createRange(const ValueQP *l, const ValueQP *r);

	void combineAltArgs(std::vector<QueryPlans>::iterator argIt, std::vector<QueryPlans>::iterator argEnd,
		std::vector<QueryPlan*> &newArgs, OptimizationContext &opt, QueryPlans &combinations) const;
	void applyConversionRules2Args(unsigned int maxAlternatives, QueryPlan *l, QueryPlan *r, OptimizationContext &opt, QueryPlans &alternatives) const;
};

class IntersectIterator : public ProxyIterator
{
public:
	IntersectIterator(NodeIterator *left, NodeIterator *right, const LocationInfo *location);
	~IntersectIterator();

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	virtual bool doJoin(DynamicContext *context);

	NodeIterator *left_;
	NodeIterator *right_;
};

}

#endif
