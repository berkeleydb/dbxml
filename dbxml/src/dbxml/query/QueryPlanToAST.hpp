//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYPLANTOAST_HPP
#define	__QUERYPLANTOAST_HPP

#include "../dataItem/DbXmlASTNode.hpp"

namespace DbXml
{

class QueryPlan;
class NodeIterator;

/**
 * Converts an ASTNode that returns nodes into a
 * QueryPlan object.
 */
class QueryPlanToAST : public DbXmlASTNode
{
public:
	QueryPlanToAST(QueryPlan *qp, StaticContext *context, XPath2MemoryManager *mm);

	QueryPlan *getQueryPlan() const { return qp_; }
	void setQueryPlan(QueryPlan *qp) { qp_ = qp; }

	virtual ASTNode *staticTypingImpl(StaticContext *context);
	virtual Result createResult(DynamicContext* context, int flags=0) const;

private:
	QueryPlan *qp_;
};

class QueryPlanToASTResult : public ResultImpl
{
public:
	QueryPlanToASTResult(NodeIterator *it, const LocationInfo *location)
		: ResultImpl(location), it_(it) {}
	~QueryPlanToASTResult();

	Item::Ptr next(DynamicContext *context);

protected:
	NodeIterator *it_;
};

}

#endif
