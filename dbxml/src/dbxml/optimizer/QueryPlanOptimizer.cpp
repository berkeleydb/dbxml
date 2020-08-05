//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "QueryPlanOptimizer.hpp"
#include "../query/QueryPlanToAST.hpp"

using namespace DbXml;
using namespace std;

ASTNode *QueryPlanOptimizer::optimize(ASTNode *item)
{
	return NodeVisitingOptimizer::optimize(item);
}

ASTNode *QueryPlanOptimizer::optimizeQueryPlanToAST(QueryPlanToAST *item)
{
	if(opt_.getPhase() == OptimizationContext::ALTERNATIVES) {
		item->setQueryPlan(optimizeQP(item->getQueryPlan()));
		item->setQueryPlan(item->getQueryPlan()->chooseAlternative(opt_, "QueryPlanToAST"));
	} else {
		item->setQueryPlan(item->getQueryPlan()->optimize(opt_));
	}
	return item;
}

