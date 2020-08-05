//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#include "../DbXmlInternal.hpp"
#include "ASTToQueryPlan.hpp"
#include "QueryExecutionContext.hpp"
#include "QueryPlanToAST.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../optimizer/QueryPlanOptimizer.hpp"

#include <xqilla/context/DynamicContext.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

ASTToQueryPlan::ASTToQueryPlan(ASTNode *ast, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(QueryPlan::AST_TO_QP, flags, mm),
	  ast_(ast)
{
	DBXML_ASSERT((ast_->getStaticAnalysis().getStaticType().isType(StaticType::NODE_TYPE) &&
			     (ast_->getStaticAnalysis().getProperties() & StaticAnalysis::DOCORDER) != 0) ||
		ast_->getStaticAnalysis().getStaticType().isType(StaticType::EMPTY_TYPE));
}

NodeIterator *ASTToQueryPlan::createNodeIterator(DynamicContext *context) const
{
	return new ASTToQueryPlanIterator(ast_->createResult(context), this);
}

QueryPlan *ASTToQueryPlan::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	ASTToQueryPlan *result = new (mm) ASTToQueryPlan(ast_, flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void ASTToQueryPlan::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *ASTToQueryPlan::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	ast_ = ast_->staticTyping(context, styper);
	_src.copy(ast_->getStaticAnalysis());

	if(ast_->getType() == (ASTNode::whichType)DbXmlASTNode::QP_TO_AST) {
		return ((QueryPlanToAST*)ast_)->getQueryPlan();
	}
	return this;
}

void ASTToQueryPlan::staticTypingLite(StaticContext *context)
{
	_src.clear();
	_src.copy(ast_->getStaticAnalysis());
}

QueryPlan *ASTToQueryPlan::optimize(OptimizationContext &opt)
{
	if(ast_->getType() == (ASTNode::whichType)DbXmlASTNode::QP_TO_AST) {
		return ((QueryPlanToAST*)ast_)->getQueryPlan();
	}

	if(opt.getQueryPlanOptimizer())
		ast_ = opt.getQueryPlanOptimizer()->optimize(ast_);

	return this;
}

void ASTToQueryPlan::findQueryPlanRoots(QPRSet &qprset) const
{
}

Cost ASTToQueryPlan::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	// TBD we need some way to estimate this - jpcs
	return Cost(5, 1);
}

bool ASTToQueryPlan::isSubsetOf(const QueryPlan *o) const
{
	// TBD Can we do better here? - jpcs
	return false;
}

string ASTToQueryPlan::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<ASTToQueryPlan>" << endl;
	s << DbXmlPrintAST::print(ast_, context, indent + INDENT);
	s << in << "</ASTToQueryPlan>" << endl;

	return s.str();
}

string ASTToQueryPlan::toString(bool brief) const
{
	ostringstream s;

	s << "AST";

	return s.str();
}
 
////////////////////////////////////////////////////////////////////////////////////////////////////

bool ASTToQueryPlanIterator::next(DynamicContext *context)
{
	node_ = (DbXmlNodeImpl*)result_->next(context).get();
	return node_.notNull();
}

bool ASTToQueryPlanIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	// Just calling next is sufficient
	node_ = (DbXmlNodeImpl*)result_->next(context).get();
	return node_.notNull();
}

