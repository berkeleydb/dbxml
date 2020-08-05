//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __FILTERQP_HPP
#define	__FILTERQP_HPP

#include "QueryPlan.hpp"

namespace DbXml
{

class StructuralJoinQP;
class ExceptQP;

/**
 * Abstract base class for QueryPlan filters
 */
class FilterQP : public QueryPlan
{
public:
	FilterQP(Type t, QueryPlan *arg, u_int32_t flags, XPath2MemoryManager *mm);

	QueryPlan *getArg() const { return arg_; }
	void setArg(QueryPlan *arg) { arg_ = arg; }

	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;

protected:
	QueryPlan *arg_;
};

class FilterSkipper
{
public:
	virtual ~FilterSkipper() {}

	virtual QueryPlan *skip(QueryPlan *item);
	virtual QueryPlan *skipFilter(FilterQP *item);
	virtual QueryPlan *skipStructuralJoin(StructuralJoinQP *item);
	virtual QueryPlan *skipExcept(ExceptQP *item);
	virtual QueryPlan *doWork(QueryPlan *qp);
};

}

#endif
