//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYPLANGENERATOR_HPP
#define	__QUERYPLANGENERATOR_HPP

#include "NodeVisitingOptimizer.hpp"
#include "../dataItem/Join.hpp"
#include "../query/QueryPlan.hpp"

class DynamicContext;

namespace DbXml
{

class QueryPlan;
class DecisionPointSource;

class QueryPlanGenerator : public NodeVisitingOptimizer
{
public:
	QueryPlanGenerator(DynamicContext *xpc, Optimizer *parent = 0);

	class QName {
	public:
		QName() : uri(0), name(0) {}
		QName(const XMLCh *u, const XMLCh *n) : uri(u), name(n) {}

		const XMLCh *uri, *name;
	};

protected:
	virtual ASTNode *optimize(ASTNode *item);
	virtual ASTNode *optimizePredicate(XQPredicate *item);
	virtual ASTNode *optimizeOperator(XQOperator *item);
	virtual ASTNode *optimizeIf(XQIf *item);
	virtual ASTNode *optimizeEffectiveBooleanValue(XQEffectiveBooleanValue *item);
	virtual ASTNode *optimizeQuantified(XQQuantified *item);

	virtual TupleNode *optimizeWhereTuple(WhereTuple *item);

	class GenerateResult {
	public:
		GenerateResult(QueryPlan *q) : qp(q), ast(0) {}
		GenerateResult(ASTNode *a) : qp(0), ast(a) {}

		QueryPlan *qp;
		ASTNode *ast;
	};

	QueryPlan *generateOrWrap(ASTNode *item, QueryPlan *context, DecisionPointSource *&dps);
	QueryPlan *generateOrWrap(const VectorOfASTNodes &args, QueryPlan *resultQP, OperationQP::Vector &resultArgs,
		QueryPlan *context, DecisionPointSource *&dps);

	GenerateResult generate(ASTNode *item, QueryPlan *context, DecisionPointSource *&dps, bool addDocOrder = true);
	GenerateResult generateNav(XQNav *item, QueryPlan *context, DecisionPointSource *&dps);
	GenerateResult generateStep(XQStep *item, QueryPlan *context, DecisionPointSource *&dps);
	GenerateResult generatePredicate(XQPredicate *item, QueryPlan *context, DecisionPointSource *&dps, bool addDocOrder);
	GenerateResult generateDocumentOrder(XQDocumentOrder *item, QueryPlan *context, DecisionPointSource *&dps);
	GenerateResult generateOperator(XQOperator *item, QueryPlan *context, DecisionPointSource *&dps);
	GenerateResult generateContextItem(XQContextItem *item, QueryPlan *context, DecisionPointSource *&dps);
	GenerateResult generateFunction(XQFunction *item, QueryPlan *context, DecisionPointSource *&dps);
	GenerateResult generateVariable(XQVariable *item, QueryPlan *context, DecisionPointSource *&dps);

	GenerateResult generateComparison(XQOperator *item, QueryPlan *context, DecisionPointSource *&dps, bool negate);
	GenerateResult generateContains(XQFunction *item, QueryPlan *context, DecisionPointSource *&dps);
	void generatePred(ASTNode *pred, QueryPlan *&expr, const QName &contextName);

	class ReverseResult {
	public:
		ReverseResult() : qp(0), joinType(Join::NONE), negate(false), ast(0), name() {}
		/// If the reverse operation is successful
		ReverseResult(QueryPlan *q, Join::Type j, const QName &n) : qp(q), joinType(j), negate(false), ast(0), name(n) {}
		/// If the reverse operation is not successful
		ReverseResult(ASTNode *a, const QName &n) : qp(0), joinType(Join::NONE), negate(false), ast(a), name(n) {}

		void inverse(XPath2MemoryManager *mm, const LocationInfo *location);
		bool uses(const QName &name, StaticContext *context) const;

		QueryPlan *qp;
		Join::Type joinType;
		bool negate;

		ASTNode *ast;
		QName name;

		std::vector<ReverseResult> subResults;
	};

	ReverseResult reverse(ASTNode *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseNav(XQNav::Steps::iterator i, XQNav::Steps::iterator end, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseStep(XQStep *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reversePredicate(XQPredicate *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseDocumentOrder(XQDocumentOrder *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseOperator(XQOperator *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseFunction(XQFunction *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseEffectiveBooleanValue(XQEffectiveBooleanValue *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseContextItem(XQContextItem *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseVariable(XQVariable *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseQuantified(XQQuantified *item, const ReverseResult &context, const QName &contextName);

	ForTuple *reverseQuantifiedForTuple(ForTuple *item, ReverseResult &context, const QName &contextName);

	ReverseResult reverseComparison(XQOperator *item, const ReverseResult &context, const QName &contextName);
	ReverseResult reverseContains(XQFunction *item, const ReverseResult &context, const QName &contextName);

	QueryPlan *createJoin(Join::Type joinType, QueryPlan *left, QueryPlan *right,
		const LocationInfo *location, XPath2MemoryManager *mm);
	QueryPlan *createNegativeJoin(Join::Type joinType, QueryPlan *left, QueryPlan *right,
		const LocationInfo *location, XPath2MemoryManager *mm);

	QueryPlan *reverseJoin(const ReverseResult &context, QueryPlan *qp, const LocationInfo *location);
	ASTNode *reverseJoin(const ReverseResult &context, ASTNode *ast, const LocationInfo *location);

	ASTNode *toASTNode(QueryPlan *qp, DecisionPointSource *&dps, XPath2MemoryManager *mm);
	QueryPlan *toQueryPlan(ASTNode *ast, DecisionPointSource *&dps, XPath2MemoryManager *mm);
	QueryPlan *getContext(QueryPlan *context, DecisionPointSource *&dps, const LocationInfo *location,
		XPath2MemoryManager *mm);
	bool insideEBV() const;

private:
	typedef std::vector<bool> AncestorStack;
	AncestorStack ancestors_;

	DynamicContext *xpc_;
};

}

#endif
