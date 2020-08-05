//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "QueryPlanToAST.hpp"
#include "ASTToQueryPlan.hpp"
#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

#include <xqilla/ast/XQNav.hpp>
#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

QueryPlanToAST::QueryPlanToAST(QueryPlan *qp, StaticContext *context, XPath2MemoryManager *mm)
	: DbXmlASTNode(QP_TO_AST, mm),
	  qp_(qp)
{
	qp_->staticTypingLite(context);
	_src.copy(qp_->getStaticAnalysis());
	_src.availableCollectionsUsed(true);
}

ASTNode *QueryPlanToAST::staticTypingImpl(StaticContext *context)
{
	_src.clear();

	_src.availableCollectionsUsed(true);

	_src.copy(qp_->getStaticAnalysis());

	if(qp_->getType() == QueryPlan::AST_TO_QP) {
		return ((ASTToQueryPlan*)qp_)->getASTNode();
	}

	return this;
}

Result QueryPlanToAST::createResult(DynamicContext* context, int flags) const
{
	return new QueryPlanToASTResult(qp_->createNodeIterator(context), this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

QueryPlanToASTResult::~QueryPlanToASTResult()
{
	delete it_;
}

Item::Ptr QueryPlanToASTResult::next(DynamicContext *context)
{
	if(it_ == 0 || !it_->next(context)) {
		delete it_;
		it_ = 0;
		return 0;
	}

	return it_->asDbXmlNode(context);
}

