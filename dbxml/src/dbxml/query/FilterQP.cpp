//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "FilterQP.hpp"
#include "StructuralJoinQP.hpp"
#include "ExceptQP.hpp"

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

FilterQP::FilterQP(Type t, QueryPlan *arg, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(t, flags, mm),
	  arg_(arg)
{
}

void FilterQP::findQueryPlanRoots(QPRSet &qprset) const
{
	arg_->findQueryPlanRoots(qprset);
}

Cost FilterQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	// TBD better cost here? - jpcs
	return arg_->cost(context, qec);
}

bool FilterQP::isSubsetOf(const QueryPlan *o) const
{
	return arg_->isSubsetOf(o);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

QueryPlan *FilterSkipper::skip(QueryPlan *item)
{
	switch(item->getType()) {
	// We can't skip NUMERIC_PREDICATE_FILTER, because changing it's input will change
	// the cardinality of it's input, and cause it to return the wrong results.
// 	case QueryPlan::NUMERIC_PREDICATE_FILTER:
	// DOC_EXISTS also checks the cardinality of it's argument, so can't be moved.
// 	case QueryPlan::DOC_EXISTS:

	case QueryPlan::VALUE_FILTER:
	case QueryPlan::PREDICATE_FILTER:
	case QueryPlan::NODE_PREDICATE_FILTER:
	case QueryPlan::NEGATIVE_NODE_PREDICATE_FILTER:
	case QueryPlan::LEVEL_FILTER:
	case QueryPlan::DEBUG_HOOK: {
		return skipFilter((FilterQP*)item);
	}
	case QueryPlan::DESCENDANT:
	case QueryPlan::DESCENDANT_OR_SELF:
	case QueryPlan::ANCESTOR:
	case QueryPlan::ANCESTOR_OR_SELF:
	case QueryPlan::ATTRIBUTE:
	case QueryPlan::CHILD:
	case QueryPlan::ATTRIBUTE_OR_CHILD:
	case QueryPlan::PARENT:
	case QueryPlan::PARENT_OF_ATTRIBUTE:
	case QueryPlan::PARENT_OF_CHILD: {
		return skipStructuralJoin((StructuralJoinQP*)item);
	}
	case QueryPlan::EXCEPT: {
		return skipExcept((ExceptQP*)item);
	}
	default: break;
	}
	
	return doWork(item);
}

QueryPlan *FilterSkipper::skipFilter(FilterQP *item)
{
	item->setArg(skip(item->getArg()));
	return item;
}

QueryPlan *FilterSkipper::skipStructuralJoin(StructuralJoinQP *item)
{
	item->setRightArg(skip(item->getRightArg()));
	return item;
}

QueryPlan *FilterSkipper::skipExcept(ExceptQP *item)
{
	item->setLeftArg(skip(item->getLeftArg()));
	return item;
}

QueryPlan *FilterSkipper::doWork(QueryPlan *qp)
{
	return qp;
}
