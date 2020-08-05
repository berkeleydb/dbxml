//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ASTTOQUERYPLAN_HPP
#define	__ASTTOQUERYPLAN_HPP

#include "QueryPlan.hpp"
#include "NodeIterator.hpp"

namespace DbXml
{

/**
 * Converts an ASTNode that returns nodes into a
 * QueryPlan object.
 */
class ASTToQueryPlan : public QueryPlan
{
public:
	ASTToQueryPlan(ASTNode *ast, u_int32_t flags, XPath2MemoryManager *mm);

	ASTNode *getASTNode() const { return ast_; }
	void setASTNode(ASTNode *ast) { ast_ = ast; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);
	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

private:
	ASTNode *ast_;
};

class ASTToQueryPlanIterator : public DbXmlNodeIterator
{
public:
	ASTToQueryPlanIterator(const Result &result, const LocationInfo *location)
		: DbXmlNodeIterator(location), result_(result) {}

	virtual bool next(DynamicContext *context);
	virtual bool seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context);

protected:
	Result result_;
};

}

#endif
