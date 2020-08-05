//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DbXmlDebugHookDecorator.hpp"
#include "DbXmlASTDebugHook.hpp"
#include "DbXmlTupleDebugHook.hpp"

#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

DbXmlDebugHookDecorator::DbXmlDebugHookDecorator(DynamicContext *context, Optimizer *parent)
	: NodeVisitingOptimizer(parent),
	  mm_(context->getMemoryManager())
{
}

ASTNode *DbXmlDebugHookDecorator::optimize(ASTNode *item)
{
	return new (mm_) DbXmlASTDebugHook(NodeVisitingOptimizer::optimize(item), mm_);
}

TupleNode *DbXmlDebugHookDecorator::optimizeTupleNode(TupleNode *item)
{
	return new (mm_) DbXmlTupleDebugHook(NodeVisitingOptimizer::optimizeTupleNode(item), mm_);
}

QueryPlan *DbXmlDebugHookDecorator::optimizeQP(QueryPlan *item)
{
	return new (mm_) QPDebugHook(NodeVisitingOptimizer::optimizeQP(item), 0, mm_);
}
