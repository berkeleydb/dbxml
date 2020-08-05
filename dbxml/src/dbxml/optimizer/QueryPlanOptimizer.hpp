//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYPLANOPTIMIZER_HPP
#define	__QUERYPLANOPTIMIZER_HPP

#include "NodeVisitingOptimizer.hpp"
#include "../query/QueryPlan.hpp"

class DynamicContext;

namespace DbXml
{

class QueryPlanOptimizer : public NodeVisitingOptimizer
{
public:
	QueryPlanOptimizer(OptimizationContext::Phase phase, DynamicContext *xpc, Optimizer *parent = 0)
		: NodeVisitingOptimizer(parent), opt_(phase, xpc, 0) { opt_.setQueryPlanOptimizer(this);}

	virtual ASTNode *optimize(ASTNode *item);

protected:
	virtual ASTNode *optimizeQueryPlanToAST(QueryPlanToAST *item);

private:
	OptimizationContext opt_;	
};

}

#endif
