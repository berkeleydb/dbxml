//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLDEBUGHOOKDECORATOR_HPP
#define	__DBXMLDEBUGHOOKDECORATOR_HPP

#include "../optimizer/NodeVisitingOptimizer.hpp"

class DynamicContext;
class XPath2MemoryManager;

namespace DbXml
{

class DbXmlDebugHookDecorator : public NodeVisitingOptimizer
{
public:
	DbXmlDebugHookDecorator(DynamicContext *context, Optimizer *parent = 0);

protected:
	virtual ASTNode *optimize(ASTNode *item);
	virtual TupleNode *optimizeTupleNode(TupleNode *item);
	virtual QueryPlan *optimizeQP(QueryPlan *item);

private:
	XPath2MemoryManager *mm_;
};

}

#endif
