//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __NODEVISITINGOPTIMIZER_HPP
#define	__NODEVISITINGOPTIMIZER_HPP

#include <xqilla/optimizer/ASTVisitor.hpp>

#include "../dataItem/DbXmlPredicate.hpp"

#include "../query/ASTToQueryPlan.hpp"
#include "../query/QueryPlanToAST.hpp"
#include "../query/ParentJoinQP.hpp"
#include "../query/ChildJoinQP.hpp"
#include "../query/DescendantJoinQP.hpp"
#include "../query/AncestorJoinQP.hpp"
#include "../query/AttributeJoinQP.hpp"
#include "../query/IntersectQP.hpp"
#include "../query/PredicateFilterQP.hpp"
#include "../query/NodePredicateFilterQP.hpp"
#include "../query/UnionQP.hpp"
#include "../query/EmptyQP.hpp"
#include "../query/SequentialScanQP.hpp"
#include "../query/ValueFilterQP.hpp"
#include "../query/AttributeOrChildJoinQP.hpp"
#include "../query/ParentOfAttributeJoinQP.hpp"
#include "../query/ParentOfChildJoinQP.hpp"
#include "../query/DecisionPointQP.hpp"
#include "../query/ExceptQP.hpp"
#include "../query/ContextNodeQP.hpp"
#include "../query/CollectionQP.hpp"
#include "../query/DocQP.hpp"
#include "../query/BufferQP.hpp"
#include "../query/LevelFilterQP.hpp"
#include "../query/StepQP.hpp"
#include "../query/VariableQP.hpp"
#include "../debug/QPDebugHook.hpp"

namespace DbXml
{

#define ASTVISITOREXTENDER_AST_METHODS() \
	virtual ASTNode *optimizeQueryPlanToAST(QueryPlanToAST *item); \
	virtual ASTNode *optimizeDbXmlNodeCheck(DbXmlNodeCheck *item); \
	virtual ASTNode *optimizeDbXmlLastStepCheck(DbXmlLastStepCheck *item); \
	virtual ASTNode *optimizeDbXmlPredicate(DbXmlPredicate *item);

#define ASTVISITOREXTENDER_QP_METHODS() \
	virtual QueryPlan *optimizeUnion(UnionQP *item); \
	virtual QueryPlan *optimizeIntersect(IntersectQP *item); \
	virtual QueryPlan *optimizePaths(PathsQP *item); \
	virtual QueryPlan *optimizePresence(PresenceQP *item); \
	virtual QueryPlan *optimizeValue(ValueQP *item); \
	virtual QueryPlan *optimizeRange(RangeQP *item); \
	virtual QueryPlan *optimizeEmpty(EmptyQP *item); \
	virtual QueryPlan *optimizeSequentialScan(SequentialScanQP *item); \
	virtual QueryPlan *optimizeValueFilter(ValueFilterQP *item); \
	virtual QueryPlan *optimizePredicateFilter(PredicateFilterQP *item); \
	virtual QueryPlan *optimizeNumericPredicateFilter(NumericPredicateFilterQP *item); \
	virtual QueryPlan *optimizeNodePredicateFilter(NodePredicateFilterQP *item); \
	virtual QueryPlan *optimizeNegativeNodePredicateFilter(NegativeNodePredicateFilterQP *item); \
	virtual QueryPlan *optimizeASTToQueryPlan(ASTToQueryPlan *item); \
	virtual QueryPlan *optimizeDescendantJoin(DescendantJoinQP *item); \
	virtual QueryPlan *optimizeDescendantOrSelfJoin(DescendantOrSelfJoinQP *item); \
	virtual QueryPlan *optimizeAncestorJoin(AncestorJoinQP *item); \
	virtual QueryPlan *optimizeAncestorOrSelfJoin(AncestorOrSelfJoinQP *item); \
	virtual QueryPlan *optimizeAttributeJoin(AttributeJoinQP *item); \
	virtual QueryPlan *optimizeChildJoin(ChildJoinQP *item); \
	virtual QueryPlan *optimizeAttributeOrChildJoin(AttributeOrChildJoinQP *item); \
	virtual QueryPlan *optimizeParentJoin(ParentJoinQP *item); \
	virtual QueryPlan *optimizeParentOfAttributeJoin(ParentOfAttributeJoinQP *item); \
	virtual QueryPlan *optimizeParentOfChildJoin(ParentOfChildJoinQP *item); \
	virtual QueryPlan *optimizeDecisionPoint(DecisionPointQP *item); \
	virtual QueryPlan *optimizeDecisionPointEnd(DecisionPointEndQP *item); \
	virtual QueryPlan *optimizeExcept(ExceptQP *item); \
	virtual QueryPlan *optimizeContextNode(ContextNodeQP *item); \
	virtual QueryPlan *optimizeCollection(CollectionQP *item); \
	virtual QueryPlan *optimizeDoc(DocQP *item); \
	virtual QueryPlan *optimizeBuffer(BufferQP *item); \
	virtual QueryPlan *optimizeBufferReference(BufferReferenceQP *item); \
	virtual QueryPlan *optimizeLevelFilter(LevelFilterQP *item); \
	virtual QueryPlan *optimizeDocExistsCheck(DocExistsCheckQP *item); \
	virtual QueryPlan *optimizeStepQP(StepQP *item); \
	virtual QueryPlan *optimizeVariableQP(VariableQP *item); \
	virtual QueryPlan *optimizeQPDebugHook(QPDebugHook *item); \
\
	virtual DecisionPointSource *optimizeQueryPlanDPSource(QueryPlanDPSource *item);

#define ASTVISITOREXTENDER_METHODS() \
	ASTVISITOREXTENDER_QP_METHODS() \
	ASTVISITOREXTENDER_AST_METHODS()

#define ALL_EXTENDEDASTVISITOR_METHODS() \
	ALL_ASTVISITOR_METHODS() \
	ASTVISITOREXTENDER_METHODS()

template<class T>
class ASTVisitorExtender : public T
{
public:
	ASTVisitorExtender() {}
	ASTVisitorExtender(Optimizer *parent)
		: T(parent) {}
	template <typename P> ASTVisitorExtender(P arg1, Optimizer *parent = 0)
		: T(arg1, parent) {}

protected:
	virtual void optimize(XQQuery *query);
	virtual ASTNode *optimize(ASTNode *item);
	virtual ASTNode *optimizeUnknown(ASTNode *item);
	virtual QueryPlan *optimizeQP(QueryPlan *item);
	virtual DecisionPointSource *optimizeDPS(DecisionPointSource *item);

	ASTVISITOREXTENDER_METHODS();
};

class NodeVisitingOptimizer : public ASTVisitorExtender<ASTVisitor>
{
public:
	NodeVisitingOptimizer(Optimizer *parent = 0)
		: ASTVisitorExtender<ASTVisitor>(parent) {}

protected:
	virtual void resetInternal() {}
};

template<class T> inline void ASTVisitorExtender<T>::optimize(XQQuery *query)
{
	T::optimize(query);
}

template<class T> inline ASTNode *ASTVisitorExtender<T>::optimize(ASTNode *item)
{
	return T::optimize(item);
}

template<class T> inline ASTNode *ASTVisitorExtender<T>::optimizeUnknown(ASTNode *item)
{
	switch((DbXmlASTNode::whichType)item->getType()) {
	case (DbXmlASTNode::QP_TO_AST):
		return optimizeQueryPlanToAST((QueryPlanToAST*)item);
	case (DbXmlASTNode::NODE_CHECK):
		return optimizeDbXmlNodeCheck((DbXmlNodeCheck*)item);
	case (DbXmlASTNode::LAST_STEP_CHECK):
		return optimizeDbXmlLastStepCheck((DbXmlLastStepCheck*)item);
	case (DbXmlASTNode::DBXML_PREDICATE):
		return optimizeDbXmlPredicate((DbXmlPredicate *)item);
	}
	return T::optimizeUnknown(item);
}

template<class T> inline ASTNode *ASTVisitorExtender<T>::optimizeQueryPlanToAST(QueryPlanToAST *item)
{
	item->setQueryPlan(optimizeQP(item->getQueryPlan()));
	return item;
}

template<class T> inline ASTNode *ASTVisitorExtender<T>::optimizeDbXmlNodeCheck(DbXmlNodeCheck *item)
{
	item->setArg(optimize(item->getArg()));
	return item;
}

template<class T> inline ASTNode *ASTVisitorExtender<T>::optimizeDbXmlLastStepCheck(DbXmlLastStepCheck *item)
{
	item->setArg(optimize(item->getArg()));
	return item;
}

template<class T> inline ASTNode *ASTVisitorExtender<T>::optimizeDbXmlPredicate(DbXmlPredicate *item)
{
	item->setExpression(optimize(const_cast<ASTNode *>(item->getExpression())));
	item->setPredicate(optimize(const_cast<ASTNode *>(item->getPredicate())));
	return item;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeQP(QueryPlan *item)
{
	QueryPlan *result = item;
	switch(item->getType()) {
	case QueryPlan::UNION: {
		result = optimizeUnion((UnionQP*)item);
		break;
	}
	case QueryPlan::INTERSECT: {
		result = optimizeIntersect((IntersectQP*)item);
		break;
	}
	case QueryPlan::PATHS: {
		result = optimizePaths((PathsQP*)item);
		break;
	}
	case QueryPlan::PRESENCE: {
		result = optimizePresence((PresenceQP*)item);
		break;
	}
	case QueryPlan::VALUE: {
		result = optimizeValue((ValueQP*)item);
		break;
	}
	case QueryPlan::RANGE: {
		result = optimizeRange((RangeQP*)item);
		break;
	}
	case QueryPlan::EMPTY: {
		result = optimizeEmpty((EmptyQP*)item);
		break;
	}
	case QueryPlan::SEQUENTIAL_SCAN: {
		result = optimizeSequentialScan((SequentialScanQP*)item);
		break;
	}
	case QueryPlan::VALUE_FILTER: {
		result = optimizeValueFilter((ValueFilterQP*)item);
		break;
	}
	case QueryPlan::PREDICATE_FILTER: {
		result = optimizePredicateFilter((PredicateFilterQP*)item);
		break;
	}
	case QueryPlan::NUMERIC_PREDICATE_FILTER: {
		result = optimizeNumericPredicateFilter((NumericPredicateFilterQP*)item);
		break;
	}
	case QueryPlan::NODE_PREDICATE_FILTER: {
		result = optimizeNodePredicateFilter((NodePredicateFilterQP*)item);
		break;
	}
	case QueryPlan::NEGATIVE_NODE_PREDICATE_FILTER: {
		result = optimizeNegativeNodePredicateFilter((NegativeNodePredicateFilterQP*)item);
		break;
	}
	case QueryPlan::AST_TO_QP: {
		result = optimizeASTToQueryPlan((ASTToQueryPlan*)item);
		break;
	}
	case QueryPlan::DESCENDANT: {
		result = optimizeDescendantJoin((DescendantJoinQP*)item);
		break;
	}
	case QueryPlan::DESCENDANT_OR_SELF: {
		result = optimizeDescendantOrSelfJoin((DescendantOrSelfJoinQP*)item);
		break;
	}
	case QueryPlan::ANCESTOR: {
		result = optimizeAncestorJoin((AncestorJoinQP*)item);
		break;
	}
	case QueryPlan::ANCESTOR_OR_SELF: {
		result = optimizeAncestorOrSelfJoin((AncestorOrSelfJoinQP*)item);
		break;
	}
	case QueryPlan::ATTRIBUTE: {
		result = optimizeAttributeJoin((AttributeJoinQP*)item);
		break;
	}
	case QueryPlan::CHILD: {
		result = optimizeChildJoin((ChildJoinQP*)item);
		break;
	}
	case QueryPlan::ATTRIBUTE_OR_CHILD: {
		result = optimizeAttributeOrChildJoin((AttributeOrChildJoinQP*)item);
		break;
	}
	case QueryPlan::PARENT: {
		result = optimizeParentJoin((ParentJoinQP*)item);
		break;
	}
	case QueryPlan::PARENT_OF_ATTRIBUTE: {
		result = optimizeParentOfAttributeJoin((ParentOfAttributeJoinQP*)item);
		break;
	}
	case QueryPlan::PARENT_OF_CHILD: {
		result = optimizeParentOfChildJoin((ParentOfChildJoinQP*)item);
		break;
	}
	case QueryPlan::DECISION_POINT: {
		result = optimizeDecisionPoint((DecisionPointQP*)item);
		break;
	}
	case QueryPlan::DECISION_POINT_END: {
		result = optimizeDecisionPointEnd((DecisionPointEndQP*)item);
		break;
	}
	case QueryPlan::EXCEPT: {
		result = optimizeExcept((ExceptQP*)item);
		break;
	}
	case QueryPlan::CONTEXT_NODE: {
		result = optimizeContextNode((ContextNodeQP*)item);
		break;
	}
	case QueryPlan::COLLECTION: {
		result = optimizeCollection((CollectionQP*)item);
		break;
	}
	case QueryPlan::DOC: {
		result = optimizeDoc((DocQP*)item);
		break;
	}
	case QueryPlan::BUFFER: {
		result = optimizeBuffer((BufferQP*)item);
		break;
	}
	case QueryPlan::BUFFER_REF: {
		result = optimizeBufferReference((BufferReferenceQP*)item);
		break;
	}
	case QueryPlan::LEVEL_FILTER: {
		result = optimizeLevelFilter((LevelFilterQP*)item);
		break;
	}
	case QueryPlan::DOC_EXISTS: {
		result = optimizeDocExistsCheck((DocExistsCheckQP*)item);
		break;
	}
	case QueryPlan::STEP: {
		result = optimizeStepQP((StepQP*)item);
		break;
	}
	case QueryPlan::VARIABLE: {
		result = optimizeVariableQP((VariableQP*)item);
		break;
	}
	case QueryPlan::DEBUG_HOOK: {
		result = optimizeQPDebugHook((QPDebugHook*)item);
		break;
	}
	default: break;
	}
	return result;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeUnion(UnionQP *item)
{
	OperationQP::Vector &args = const_cast<OperationQP::Vector&>(item->getArgs());
	for(OperationQP::Vector::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimizeQP(*i);
	}
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeIntersect(IntersectQP *item)
{
	OperationQP::Vector &args = const_cast<OperationQP::Vector&>(item->getArgs());
	for(OperationQP::Vector::iterator i = args.begin(); i != args.end(); ++i) {
		*i = optimizeQP(*i);
	}
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizePaths(PathsQP *item)
{
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizePresence(PresenceQP *item)
{
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeValue(ValueQP *item)
{
	QPValue &value = const_cast<QPValue&>(item->getValue());
	if(value.getASTNode() != 0) {
		value.setASTNode(optimize(const_cast<ASTNode*>(value.getASTNode())));
	}
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeRange(RangeQP *item)
{
	QPValue &value = const_cast<QPValue&>(item->getValue());
	if(value.getASTNode() != 0) {
		value.setASTNode(optimize(const_cast<ASTNode*>(value.getASTNode())));
	}
	QPValue &value2 = const_cast<QPValue&>(item->getValue2());
	if(value2.getASTNode() != 0) {
		value2.setASTNode(optimize(const_cast<ASTNode*>(value2.getASTNode())));
	}
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeEmpty(EmptyQP *item)
{
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeSequentialScan(SequentialScanQP *item)
{
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeValueFilter(ValueFilterQP *item)
{
	item->setArg(optimizeQP(item->getArg()));

	ImpliedSchemaNode *isn = item->getImpliedSchemaNode();
	isn->setASTNode(optimize(const_cast<ASTNode*>(isn->getASTNode())));

	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizePredicateFilter(PredicateFilterQP *item)
{
	item->setArg(optimizeQP(item->getArg()));
	item->setPred(optimize(item->getPred()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeNumericPredicateFilter(NumericPredicateFilterQP *item)
{
	item->setArg(optimizeQP(item->getArg()));
	item->setPred(optimize(item->getPred()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeNodePredicateFilter(NodePredicateFilterQP *item)
{
	item->setArg(optimizeQP(item->getArg()));
	item->setPred(optimizeQP(item->getPred()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeNegativeNodePredicateFilter(NegativeNodePredicateFilterQP *item)
{
	item->setArg(optimizeQP(item->getArg()));
	item->setPred(optimizeQP(item->getPred()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeASTToQueryPlan(ASTToQueryPlan *item)
{
	item->setASTNode(optimize(item->getASTNode()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeDescendantJoin(DescendantJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeDescendantOrSelfJoin(DescendantOrSelfJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeAncestorJoin(AncestorJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeAncestorOrSelfJoin(AncestorOrSelfJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeAttributeJoin(AttributeJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeChildJoin(ChildJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeAttributeOrChildJoin(AttributeOrChildJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeParentJoin(ParentJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeParentOfAttributeJoin(ParentOfAttributeJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeParentOfChildJoin(ParentOfChildJoinQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline DecisionPointSource *ASTVisitorExtender<T>::optimizeDPS(DecisionPointSource *item)
{
	switch(item->getType()) {
	case DecisionPointSource::QUERY_PLAN:
		return optimizeQueryPlanDPSource((QueryPlanDPSource*)item);
	default: break;
	}
	return item;
}

template<class T> inline DecisionPointSource *ASTVisitorExtender<T>::optimizeQueryPlanDPSource(QueryPlanDPSource *item)
{
	item->setParent(optimizeQP(item->getParent()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeDecisionPoint(DecisionPointQP *item)
{
	if(item->getSource())
		item->setSource(optimizeDPS(item->getSource()));

	if(item->getArg() != 0)
		item->setArg(optimizeQP(item->getArg()));

	for(DecisionPointQP::ListItem *li = item->getList(); li != 0; li = li->next)
		li->qp = optimizeQP(li->qp);

	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeDecisionPointEnd(DecisionPointEndQP *item)
{
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeExcept(ExceptQP *item)
{
	item->setLeftArg(optimizeQP(item->getLeftArg()));
	item->setRightArg(optimizeQP(item->getRightArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeContextNode(ContextNodeQP *item)
{
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeCollection(CollectionQP *item)
{
	if(item->getArg() != 0)
		item->setArg(optimize(item->getArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeDoc(DocQP *item)
{
	item->setArg(optimize(item->getArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeBuffer(BufferQP *item)
{
	item->setParent(optimizeQP(item->getParent()));
	item->setArg(optimizeQP(item->getArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeBufferReference(BufferReferenceQP *item)
{
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeLevelFilter(LevelFilterQP *item)
{
	item->setArg(optimizeQP(item->getArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeDocExistsCheck(DocExistsCheckQP *item)
{
	item->setArg(optimizeQP(item->getArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeStepQP(StepQP *item)
{
	item->setArg(optimizeQP(item->getArg()));
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeVariableQP(VariableQP *item)
{
	return item;
}

template<class T> inline QueryPlan *ASTVisitorExtender<T>::optimizeQPDebugHook(QPDebugHook *item)
{
	item->setArg(optimizeQP(item->getArg()));
	return item;
}

}

#endif
