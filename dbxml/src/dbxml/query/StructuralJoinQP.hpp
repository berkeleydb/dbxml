//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __STRUCTURALJOINQP_HPP
#define	__STRUCTURALJOINQP_HPP

#include "QueryPlan.hpp"
#include "../dataItem/Join.hpp"
#include "FilterQP.hpp"

namespace DbXml
{

class NodePredicateFilterQP;
class NegativeNodePredicateFilterQP;

/**
 * Provides a base class for all structural joins. It
 * is assumed that the right argument will be returned
 * from the join in all cases.
 */
class StructuralJoinQP : public QueryPlan
{
public:
	StructuralJoinQP(QueryPlan::Type type, QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
		: QueryPlan(type, flags, mm), left_(l), right_(r) {}

	QueryPlan *getLeftArg() const { return left_; }
	void setLeftArg(QueryPlan *a) { left_ = a; }
	QueryPlan *getRightArg() const { return right_; }
	void setRightArg(QueryPlan *a) { right_ = a; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);

	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	static QueryPlan *createJoin(Join::Type joinType, QueryPlan *left, QueryPlan *right,
		u_int32_t flags, const LocationInfo *location, XPath2MemoryManager *mm);
	static bool joinSupported(Join::Type joinType);
	static bool isStructuralJoin(QueryPlan::Type type);
	static QueryPlan::Type oppositeJoin(QueryPlan::Type type);
	static Join::Type getJoinType(QueryPlan::Type type);
	static ImpliedSchemaNode::Type findType(QueryPlan *qp);
	static bool isDocumentIndex(QueryPlan *qp, bool toBeRemoved = false);
	static bool isSuitableForDocumentIndexComparison(QueryPlan *qp);
	static QueryPlan *findLookup(QueryPlan *qp);
	static bool containsAllDocumentNodes(QueryPlan *qp);

protected:
	virtual std::string getLongName() const = 0;
	virtual std::string getShortName() const = 0;

	QueryPlan *left_;
	QueryPlan *right_;
};

//
//  RightLookupToLeftStep: d(*(1), I(foo)) -> step(descendant::foo, *(1))
//
class RightLookupToLeftStep : private FilterSkipper
{
public:
	QueryPlan *run(const StructuralJoinQP *s, OptimizationContext &o,
		XPath2MemoryManager *m);
	QueryPlan *run(QueryPlan *l, QueryPlan *r, u_int32_t flags, const LocationInfo *loc,
		OptimizationContext &o, XPath2MemoryManager *m);

private:
	virtual QueryPlan *doWork(QueryPlan *qp);

	QueryPlan *leftArg;
	Join::Type joinType;

	XPath2MemoryManager *mm;
	bool success;
	bool trialRun;
};

//
// PushBackJoin: d(*(1), c(*(2), *(3))) -> c(*(2), d(*(1), *(3)))
//
// PushBackJoin: d(*(1), u(*(2), *(3))) -> BUF(0, *(1), u(d(BR(0), *(2)), d(BR(0), *(3))))
//
class PushBackJoin : private FilterSkipper
{
public:
	QueryPlan *run(const StructuralJoinQP *s, OptimizationContext &opt,
		XPath2MemoryManager *m);
	QueryPlan *run(QueryPlan *l, QueryPlan *r, u_int32_t flags,
		const LocationInfo *loc, OptimizationContext &opt,
		XPath2MemoryManager *m);

private:
	virtual QueryPlan *skipStructuralJoin(StructuralJoinQP *lsj);
	virtual QueryPlan *doWork(QueryPlan *qp);

	QueryPlan *leftArg;
	Join::Type joinType;
	const LocationInfo *location;
	u_int32_t flags;

	OptimizationContext *opt;
	XPath2MemoryManager *mm;
	bool success;
	bool trialRun;
};

//
// SwapStep: d(*(1), step(child::foo, *(2))) -> c(*(2), step(descendant::foo, *(1)))
//
class SwapStep : private FilterSkipper
{
public:
	QueryPlan *run(const StructuralJoinQP *s, OptimizationContext &opt,
		XPath2MemoryManager *m);
	QueryPlan *run(QueryPlan *l, QueryPlan *r, u_int32_t flags,
		const LocationInfo *loc, OptimizationContext &opt,
		XPath2MemoryManager *m);

private:
	virtual QueryPlan *doWork(QueryPlan *qp);

	QueryPlan *leftArg;
	Join::Type joinType;
	const LocationInfo *location;
	u_int32_t flags;

	XPath2MemoryManager *mm;
	bool success;
	bool trialRun;
};

//
// LeftToPredicate: d(*(1), *(2)) -> NPF(#tmp, *(2), d(*(1), VAR(#tmp)))
//
class LeftToPredicate
{
public:
	QueryPlan *run(const StructuralJoinQP *s, OptimizationContext &opt,
		XPath2MemoryManager *m);
	QueryPlan *run(QueryPlan *l, QueryPlan *r, u_int32_t flags, const LocationInfo *loc,
		OptimizationContext &opt, XPath2MemoryManager *m);
};

//
// ReverseJoin: NPF(#var, *(1), d(*(2), *(3))) -> NPF(#var, *(1), a(*(3), *(2)))
//
class ReverseJoin : private FilterSkipper
{
public:
	QueryPlan *run(const NodePredicateFilterQP *npf, OptimizationContext &opt,
		XPath2MemoryManager *m);
	QueryPlan *run(const NegativeNodePredicateFilterQP *npf, OptimizationContext &opt,
		XPath2MemoryManager *m);

private:
	virtual QueryPlan *skipStructuralJoin(StructuralJoinQP *sj);

	StructuralJoinQP *sj;
	bool trialRun;
};

class PullForwardDocumentJoin : protected FilterSkipper
{
public:
	QueryPlan *run(QueryPlan *qp);

protected:
	virtual QueryPlan *skipStructuralJoin(StructuralJoinQP *sj);
	virtual QueryPlan *doWork(QueryPlan *qp);

	virtual bool canFind(QueryPlan *qp);

	QueryPlan *input;
	StructuralJoinQP *documentJoin;
};

}

#endif
