//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLPRINTAST_HPP
#define	__DBXMLPRINTAST_HPP

#include <iostream>

#include <xqilla/utils/PrintAST.hpp>
#include <xqilla/optimizer/Optimizer.hpp>

#include "dataItem/Join.hpp"

namespace DbXml
{

class OutputTree;
class DbXmlDocAvailable;
class QueryPlan;
class QueryPlanToAST;
class DbXmlNodeCheck;
class DbXmlLastStepCheck;
class DbXmlPredicate;

class DbXmlPrintAST : public PrintAST
{
public:
	static std::string print(const XQillaExpression *expr, const DynamicContext *context, int indent = 0);
	static std::string print(const XQQuery *query, const DynamicContext *context, int indent = 0);
	static std::string print(const ASTNode *item, const DynamicContext *context, int indent = 0);
	static std::string print(const TupleNode *item, const DynamicContext *context, int indent = 0);

	virtual std::string printUnknown(const ASTNode *item, const DynamicContext *context, int indent);
	virtual std::string printFunction(const XQFunction *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlDocAvailable(const DbXmlDocAvailable *item, const DynamicContext *context,
		int indent);
	virtual std::string printQueryPlanToAST(const QueryPlanToAST *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlNodeCheck(const DbXmlNodeCheck *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlLastStepCheck(const DbXmlLastStepCheck *item, const DynamicContext *context, int indent);
	virtual std::string printDbXmlPredicate(const DbXmlPredicate *item, const DynamicContext *context, int indent);

	static std::string getJoinTypeName(Join::Type join);
	static std::string printNodeTestAttrs(const NodeTest *step);
};

class OutputTree : public Optimizer
{
public:
	OutputTree(std::string label, const DynamicContext *context, Optimizer *parent = 0)
		: Optimizer(parent), label_(label), context_(context) {}
	virtual ~OutputTree() {}
protected:
	virtual void resetInternal() {}
	virtual void optimize(XQQuery *query)
	{
		std::cerr << label_ << ":" << std::endl;
		std::cerr << DbXmlPrintAST::print(query, context_) << std::endl;
	}
	virtual ASTNode *optimize(ASTNode *item)
	{
		std::cerr << label_ << ":" << std::endl;
		std::cerr << DbXmlPrintAST::print(item, context_) << std::endl;
		return item;
	}
private:
	std::string label_;
	const DynamicContext *context_;
};

}

#endif
