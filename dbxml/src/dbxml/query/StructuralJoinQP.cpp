//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "StructuralJoinQP.hpp"
#include "SequentialScanQP.hpp"
#include "StepQP.hpp"
#include "IntersectQP.hpp"
#include "ParentJoinQP.hpp"
#include "ChildJoinQP.hpp"
#include "DescendantJoinQP.hpp"
#include "AncestorJoinQP.hpp"
#include "AttributeJoinQP.hpp"
#include "ParentOfAttributeJoinQP.hpp"
#include "ParentOfChildJoinQP.hpp"
#include "AttributeOrChildJoinQP.hpp"
#include "ContextNodeQP.hpp"
#include "VariableQP.hpp"
#include "NodePredicateFilterQP.hpp"
#include "LevelFilterQP.hpp"
#include "DecisionPointQP.hpp"
#include "UnionQP.hpp"
#include "BufferQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../dataItem/DbXmlNodeTest.hpp"
#include "../nodeStore/NsUtil.hpp"
#include "../optimizer/NodeVisitingOptimizer.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQNav.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

static const int INDENT = 1;

static unsigned int getStructuralJoinProperties(QueryPlan::Type sj)
{
	unsigned int properties = 0;
	// properties depend on the axis of the step
	switch (sj) {
	case QueryPlan::CHILD:
	case QueryPlan::ATTRIBUTE_OR_CHILD:
	case QueryPlan::ATTRIBUTE:
		properties |= StaticAnalysis::SUBTREE | StaticAnalysis::PEER;
		break;
	case QueryPlan::DESCENDANT:
	case QueryPlan::DESCENDANT_OR_SELF:
		properties |= StaticAnalysis::SUBTREE;
		break;
	case QueryPlan::PARENT:
	case QueryPlan::PARENT_OF_ATTRIBUTE:
	case QueryPlan::PARENT_OF_CHILD:
		properties |= StaticAnalysis::PEER | StaticAnalysis::ONENODE;
		break;
	case QueryPlan::ANCESTOR:
	case QueryPlan::ANCESTOR_OR_SELF:
	default: break;
	}

	properties |= StaticAnalysis::GROUPED | StaticAnalysis::SAMEDOC | StaticAnalysis::DOCORDER;
	return properties;
}

QueryPlan *StructuralJoinQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	left_ = left_->staticTyping(context, styper);
	_src.add(left_->getStaticAnalysis());

	right_ = right_->staticTyping(context, styper);
	_src.add(right_->getStaticAnalysis());
	_src.getStaticType() = right_->getStaticAnalysis().getStaticType();
	_src.getStaticType().multiply(0, 1);

	_src.setProperties(XQNav::combineProperties(left_->getStaticAnalysis().getProperties(),
				   getStructuralJoinProperties(getType())));
	_src.setProperties(_src.getProperties() | StaticAnalysis::DOCORDER);

	return this;
}

void StructuralJoinQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	left_->staticTypingLite(context);
	_src.add(left_->getStaticAnalysis());

	right_->staticTypingLite(context);
	_src.add(right_->getStaticAnalysis());
	_src.getStaticType() = right_->getStaticAnalysis().getStaticType();
	_src.getStaticType().multiply(0, 1);

	_src.setProperties(XQNav::combineProperties(left_->getStaticAnalysis().getProperties(),
				   getStructuralJoinProperties(getType())));
	_src.setProperties(_src.getProperties() | StaticAnalysis::DOCORDER);
}

bool StructuralJoinQP::containsAllDocumentNodes(QueryPlan *qp)
{
	switch(qp->getType()) {
	case QueryPlan::SEQUENTIAL_SCAN:
		return ((SequentialScanQP*)qp)->getNodeType() == ImpliedSchemaNode::METADATA;
	case QueryPlan::UNION: {
		OperationQP *u = (OperationQP*)qp;
		for(OperationQP::Vector::const_iterator i = u->getArgs().begin();
		    i != u->getArgs().end(); ++i) {
			if(containsAllDocumentNodes(*i)) return true;
		}
	}
	default: return false;
	}
}

bool StructuralJoinQP::isSuitableForDocumentIndexComparison(QueryPlan *qp)
{
	switch(qp->getType()) {
	case BUFFER_REF:
// 	case CONTEXT_NODE:
	case AST_TO_QP:
		return false;
	case VARIABLE:
		return ((qp->getStaticAnalysis().getProperties() & StaticAnalysis::ONENODE) != 0);
	default: break;
	}

	return true;
}

bool StructuralJoinQP::isDocumentIndex(QueryPlan *qp, bool toBeRemoved)
{
	switch(qp->getType()) {
	case QueryPlan::PRESENCE:
	case QueryPlan::VALUE:
	case QueryPlan::RANGE: {
		PresenceQP *pqp = (PresenceQP*)qp;

		if(pqp->isDocumentIndex()) return true;
		if(toBeRemoved) return false;

		return pqp->getReturnType() == ImpliedSchemaNode::METADATA;
	}
	case QueryPlan::UNION:
	case QueryPlan::INTERSECT: {
		OperationQP::Vector &args = const_cast<OperationQP::Vector&>(((OperationQP*)qp)->getArgs());
		for(OperationQP::Vector::iterator i = args.begin(); i != args.end(); ++i) {
			if(!isDocumentIndex(*i, toBeRemoved)) return false;
		}
		return true;
	}
	default: break;
	}

	return false;
}

ImpliedSchemaNode::Type StructuralJoinQP::findType(QueryPlan *qp)
{
	// See if we can find the type of nodes that right_ returns
	while(qp != 0) {
		switch(qp->getType()) {
		case PRESENCE:
		case VALUE:
		case RANGE: {
			return ((PresenceQP*)qp)->getReturnType();
		}
		case STEP: {
			StepQP *step = (StepQP*)qp;

			if(step->getNodeTest() != 0 && step->getNodeTest()->getItemType() == 0 &&
				!step->getNodeTest()->getTypeWildcard()) {
				if(step->getNodeTest()->getNodeType() == Node::element_string) {
					return ImpliedSchemaNode::CHILD;
				} else if(step->getNodeTest()->getNodeType() == Node::attribute_string) {
					return ImpliedSchemaNode::ATTRIBUTE;
				} else if(step->getNodeTest()->getNodeType() == Node::document_string) {
					return ImpliedSchemaNode::METADATA;
				}
			}

			return (ImpliedSchemaNode::Type)-1;
		}
		case DESCENDANT:
		case DESCENDANT_OR_SELF:
		case ANCESTOR:
		case ANCESTOR_OR_SELF:
		case ATTRIBUTE:
		case CHILD:
		case ATTRIBUTE_OR_CHILD:
		case PARENT:
		case PARENT_OF_ATTRIBUTE:
		case PARENT_OF_CHILD:
			qp = ((StructuralJoinQP*)qp)->getRightArg();
			break;
		case SEQUENTIAL_SCAN:
			return ((SequentialScanQP*)qp)->getNodeType();
		case UNION:
		case INTERSECT: {
			OperationQP::Vector &args = const_cast<OperationQP::Vector&>(((OperationQP*)qp)->getArgs());
			OperationQP::Vector::iterator i = args.begin();
			if(i != args.end()) {
				ImpliedSchemaNode::Type type = findType(*i);
				for(++i; i != args.end(); ++i) {
					if(findType(*i) != type) return (ImpliedSchemaNode::Type)-1;
				}
				return type;
			}
			break;
		}
		default:
			return (ImpliedSchemaNode::Type)-1;
		}
	}
	return (ImpliedSchemaNode::Type)-1;
}

QueryPlan *StructuralJoinQP::findLookup(QueryPlan *qp)
{
	while(qp != 0) {
		switch(qp->getType()) {
		case PRESENCE:
		case VALUE:
		case RANGE:
			return qp;
		case DESCENDANT:
		case DESCENDANT_OR_SELF:
		case ANCESTOR:
		case ANCESTOR_OR_SELF:
		case ATTRIBUTE:
		case CHILD:
		case ATTRIBUTE_OR_CHILD:
		case PARENT:
		case PARENT_OF_ATTRIBUTE:
		case PARENT_OF_CHILD:
			qp = ((StructuralJoinQP*)qp)->getRightArg();
			break;
		case SEQUENTIAL_SCAN:
		default: return 0;
		}
	}
	return 0;
}

void StructuralJoinQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans leftAltArgs;
	left_->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, maxAlternatives, opt, leftAltArgs);
	DBXML_ASSERT(leftAltArgs.size() > 0);

	QueryPlans rightAltArgs;
	right_->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, maxAlternatives, opt, rightAltArgs);
	DBXML_ASSERT(rightAltArgs.size() > 0);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = leftAltArgs.begin(); it != leftAltArgs.end(); ++it) {
		for(QueryPlans::iterator it2 = rightAltArgs.begin(); it2 != rightAltArgs.end(); ++it2) {
			combinations.push_back(createJoin(getJoinType(getType()),
						       (*it)->copy(mm), (*it2)->copy(mm), flags_, this, mm));
		}
	}

	// Release the alternative arguments, since they've been copied
	for(it = leftAltArgs.begin(); it != leftAltArgs.end(); ++it) {
		(*it)->release();
	}
	for(it = rightAltArgs.begin(); it != rightAltArgs.end(); ++it) {
		(*it)->release();
	}
}

void StructuralJoinQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Find subsets in right structural joins of the same type
	QueryPlan **right = &right_;
	while(true) {
		if((*right)->getType() == getType()) {
			StructuralJoinQP *lsj = (StructuralJoinQP*)*right;

			if(left_->isSubsetOf(lsj->left_)) {
				string before = logBefore(this);

				*right = lsj->right_;

				logTransformation(opt.getLog(), "Right join subset", before, this);

				applyConversionRules(maxAlternatives, opt, alternatives);
				return;
			}
			if(lsj->left_->isSubsetOf(left_)) {
				string before = logBefore(this);

				*right = lsj->right_;
				left_ = lsj->left_;

				logTransformation(opt.getLog(), "Right join subset", before, this);

				applyConversionRules(maxAlternatives, opt, alternatives);
				return;
			}
		}
		if(!isStructuralJoin((*right)->getType())) break;

		right = &((StructuralJoinQP*)*right)->right_;
	}

	// Find subsets in left structural joins of the opposite type
	QueryPlan **left = &left_;
	while(true) {
		if((*left)->getType() == oppositeJoin(getType())) {
			StructuralJoinQP *lsj = (StructuralJoinQP*)*left;

			if(right_->isSubsetOf(lsj->left_)) {
				string before = logBefore(this);

				*left = lsj->right_;

				logTransformation(opt.getLog(), "Left join subset", before, this);

				applyConversionRules(maxAlternatives, opt, alternatives);
				return;
			}
		}
		if(!isStructuralJoin((*left)->getType())) break;

		left = &((StructuralJoinQP*)*left)->right_;
	}

	alternatives.push_back(this);

	{
		AutoRelease<QueryPlan> result(RightLookupToLeftStep().run(this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	addFlag(SKIP_RIGHT_LOOKUP_TO_LEFT_STEP);

	{
		AutoRelease<QueryPlan> result(PushBackJoin().run(this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	{
		AutoRelease<QueryPlan> result(SwapStep().run(this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	addFlag(SKIP_PUSH_BACK_JOIN);
	addFlag(SKIP_SWAP_STEP);

	{
		AutoRelease<QueryPlan> result(LeftToPredicate().run(this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	addFlag(SKIP_LEFT_TO_PREDICATE);
}

QueryPlan *StructuralJoinQP::optimize(OptimizationContext &opt)
{
	// Optimize the arguments
	left_ = left_->optimize(opt);
	right_ = right_->optimize(opt);

	// Pull forward filters
	switch(right_->getType()) {
	case VALUE_FILTER:
	case PREDICATE_FILTER:
	case NODE_PREDICATE_FILTER:
	case NEGATIVE_NODE_PREDICATE_FILTER:
	// We can't skip NUMERIC_PREDICATE_FILTER, because changing it's input will change
	// the cardinality of it's input, and cause it to return the wrong results.
// 	case NUMERIC_PREDICATE_FILTER:
// 	case DOC_EXISTS:
	case LEVEL_FILTER: {
		string before = logBefore(this);

		FilterQP *filter = (FilterQP*)right_;

		right_ = filter->getArg();
		filter->setArg(this);

		logTransformation(opt.getLog(), "Filter pulled forward", before, filter);
		return filter->optimize(opt);
	}
	default: break;
	}

	// Create a before string, in case we need it for logging
	QueryPlan *result;

	// Try to pull forward document indexes, so they will combine
	if(opt.getPhase() < OptimizationContext::ALTERNATIVES) {
		string before = logBefore(this);
		result = PullForwardDocumentJoin().run(this);
		if(result != 0) {
			logTransformation(opt.getLog(), "Pull forward document join", before, result);
			return result->optimize(opt);
		}
	}

	return this;
}

bool StructuralJoinQP::joinSupported(Join::Type joinType)
{
	switch(joinType) {
	case Join::NONE:
	case Join::PARENT:
	case Join::DESCENDANT_OR_SELF:
	case Join::SELF:
	case Join::ANCESTOR:
	case Join::ANCESTOR_OR_SELF:
	case Join::ATTRIBUTE:
	case Join::CHILD:
	case Join::DESCENDANT:
	case Join::PARENT_A:
	case Join::PARENT_C:
	case Join::ATTRIBUTE_OR_CHILD:
		return true;
	case Join::FOLLOWING:
	case Join::PRECEDING:
	case Join::PRECEDING_SIBLING:
	case Join::FOLLOWING_SIBLING: {
		// TBD implement this somehow - jpcs
		break;
	}
	case Join::NAMESPACE:
	default:
		// Should never occur
		DBXML_ASSERT(false);
		break;
	}

	return false;
}

QueryPlan *StructuralJoinQP::createJoin(Join::Type joinType, QueryPlan *left, QueryPlan *right, u_int32_t flags,
	const LocationInfo *location, XPath2MemoryManager *mm)
{
	QueryPlan *join = 0;

	switch(joinType) {
	case Join::NONE: {
		join = right;
		break;
	}
	case Join::PARENT: {
		join = new (mm) ParentJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::DESCENDANT_OR_SELF: {
		join = new (mm) DescendantOrSelfJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::SELF: {
		join = new (mm) IntersectQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::ANCESTOR: {
		join = new (mm) AncestorJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::ANCESTOR_OR_SELF: {
		join = new (mm) AncestorOrSelfJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::ATTRIBUTE: {
		join = new (mm) AttributeJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::CHILD: {
		join = new (mm) ChildJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::DESCENDANT: {
		join = new (mm) DescendantJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::PARENT_A: {
		join = new (mm) ParentOfAttributeJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::PARENT_C: {
		join = new (mm) ParentOfChildJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::ATTRIBUTE_OR_CHILD: {
		join = new (mm) AttributeOrChildJoinQP(left, right, flags, mm);
		join->setLocationInfo(right);
		break;
	}
	case Join::FOLLOWING:
	case Join::PRECEDING:
	case Join::PRECEDING_SIBLING:
	case Join::FOLLOWING_SIBLING: {
		// TBD implement this somehow - jpcs
		break;
	}
	case Join::NAMESPACE:
	default:
		// Should never occur
		DBXML_ASSERT(false);
		break;
	}

	return join;
}

Join::Type StructuralJoinQP::getJoinType(QueryPlan::Type type)
{
	switch(type) {
	case DESCENDANT: return Join::DESCENDANT;
	case DESCENDANT_OR_SELF: return Join::DESCENDANT_OR_SELF;
	case ANCESTOR: return Join::ANCESTOR;
	case ANCESTOR_OR_SELF: return Join::ANCESTOR_OR_SELF;
	case ATTRIBUTE: return Join::ATTRIBUTE;
	case CHILD: return Join::CHILD;
	case ATTRIBUTE_OR_CHILD: return Join::ATTRIBUTE_OR_CHILD;
	case PARENT: return Join::PARENT;
	case PARENT_OF_ATTRIBUTE: return Join::PARENT_A;
	case PARENT_OF_CHILD: return Join::PARENT_C;
	default: break;
	}
	return Join::NONE;
}

bool StructuralJoinQP::isStructuralJoin(QueryPlan::Type type)
{
	switch(type) {
	case DESCENDANT:
	case DESCENDANT_OR_SELF:
	case ANCESTOR:
	case ANCESTOR_OR_SELF:
	case ATTRIBUTE:
	case CHILD:
	case ATTRIBUTE_OR_CHILD:
	case PARENT:
	case PARENT_OF_ATTRIBUTE:
	case PARENT_OF_CHILD:
		return true;
	default: break;
	}
	return false;
}

QueryPlan::Type StructuralJoinQP::oppositeJoin(QueryPlan::Type type)
{
	switch(type) {
	case DESCENDANT: return ANCESTOR;
	case ANCESTOR: return DESCENDANT;

	case DESCENDANT_OR_SELF: return ANCESTOR_OR_SELF;
	case ANCESTOR_OR_SELF: return DESCENDANT_OR_SELF;

	case ATTRIBUTE: return PARENT_OF_ATTRIBUTE;
	case PARENT_OF_ATTRIBUTE: return ATTRIBUTE;

	case CHILD: return PARENT_OF_CHILD;
	case PARENT_OF_CHILD: return CHILD;

	case ATTRIBUTE_OR_CHILD: return PARENT;
	case PARENT: return ATTRIBUTE_OR_CHILD;

	default: break;
	}
	return (QueryPlan::Type)-1;
}

void StructuralJoinQP::findQueryPlanRoots(QPRSet &qprset) const
{
	left_->findQueryPlanRoots(qprset);
	right_->findQueryPlanRoots(qprset);
}

Cost StructuralJoinQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost rCost = right_->cost(context, qec);
	Cost lCost = left_->cost(context, qec);

	ContainerBase *container = StepQP::findContainer(left_);
	if(container == 0) container = StepQP::findContainer(right_);

	Join::Type joinType = getJoinType(getType());
	Cost stepCost = StepQP::cost(container, left_, lCost, joinType, StepQP::findNodeTest(right_), context,
		GET_CONFIGURATION(qec.getDynamicContext())->getStatsCache());

	Cost result;
	if(stepCost.keys < rCost.keys) {
		result.keys = stepCost.keys;
	} else {
		result.keys = rCost.keys;
	}

	// TBD Can we model the interactions better here? - jpcs
	result.pagesOverhead = lCost.pagesOverhead + rCost.pagesOverhead;
	result.pagesForKeys = lCost.pagesForKeys;

	switch(getType()) {
	case ATTRIBUTE:
	case DESCENDANT:
	case DESCENDANT_OR_SELF:
		result.pagesForKeys += (rCost.pagesForKeys / rCost.keys) * result.keys;
		break;
	case CHILD:
	case ATTRIBUTE_OR_CHILD: {
		Cost descStepCost = StepQP::cost(container, left_, lCost, Join::DESCENDANT, StepQP::findNodeTest(right_), context,
			GET_CONFIGURATION(qec.getDynamicContext())->getStatsCache());
		if(descStepCost.keys < rCost.keys) {
			result.pagesForKeys += (rCost.pagesForKeys / rCost.keys) * descStepCost.keys;
		} else {
			result.pagesForKeys += rCost.pagesForKeys;
		}
		break;
	}
	case ANCESTOR:
	case ANCESTOR_OR_SELF:
	case PARENT:
	case PARENT_OF_ATTRIBUTE:
	case PARENT_OF_CHILD:
		result.pagesForKeys += rCost.pagesForKeys;
		break;
	default: break;
	}

	return result;
}

bool StructuralJoinQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == getType()) {
		StructuralJoinQP *sj = (StructuralJoinQP*)o;
		if(left_->isSubsetOf(sj->left_) && right_->isSubsetOf(sj->right_))
			return true;
	}

	// The result of this QP is a subset of it's right argument
	return right_->isSubsetOf(o);
}

string StructuralJoinQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<" << getLongName() << ">" << endl;
	s << left_->printQueryPlan(context, indent + INDENT);
	s << right_->printQueryPlan(context, indent + INDENT);
	s << in << "</" << getLongName() << ">" << endl;

	return s.str();
}

string StructuralJoinQP::toString(bool brief) const
{
	ostringstream s;

	s << getShortName() << "(";
	s << left_->toString(brief);
	s << ",";
	s << right_->toString(brief);
	s << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//
// I - Index lookup (P, V, R, or SS)
//
// RightLookupToLeftStep: d(*(1), I(foo)) -> step(descendant::foo, *(1))
//

QueryPlan *RightLookupToLeftStep::run(const StructuralJoinQP *sj, OptimizationContext &opt,
	XPath2MemoryManager *m)
{
	leftArg = sj->getLeftArg();
	joinType = StructuralJoinQP::getJoinType(sj->getType());
	mm = m;

	if(StructuralJoinQP::isDocumentIndex(leftArg, /*toBeRemoved*/true)) return 0;
	if((sj->getFlags() & QueryPlan::SKIP_RIGHT_LOOKUP_TO_LEFT_STEP) != 0) return 0;

	// Do a trial run first, to see if this will succeed
	trialRun = true;
	success = false;

	skip(sj->getRightArg());
	if(!success) return 0;

	// Now do the transform for real, using a copy of the arguments
	trialRun = false;

	leftArg = sj->getLeftArg()->copy(mm);
	QueryPlan *result = skip(sj->getRightArg()->copy(mm));
	result->staticTypingLite(opt.getContext());

#if LOG_RULES
	sj->logTransformation(opt.getLog(), "Right lookup to left step", sj, result);
#endif

	return result;
}

QueryPlan *RightLookupToLeftStep::run(QueryPlan *l, QueryPlan *r, u_int32_t flags, const LocationInfo *loc,
	OptimizationContext &opt, XPath2MemoryManager *m)
{
	leftArg = l;
	joinType = Join::SELF;
	mm = m;

	if(StructuralJoinQP::isDocumentIndex(leftArg, /*toBeRemoved*/true)) return 0;
	if((flags & QueryPlan::SKIP_RIGHT_LOOKUP_TO_LEFT_STEP) != 0) return 0;

	// Do a trial run first, to see if this will succeed
	trialRun = true;
	success = false;

	skip(r);
	if(!success) return 0;

	// Now do the transform for real, using a copy of the arguments
	trialRun = false;

	leftArg = l->copy(mm);
	QueryPlan *result = skip(r->copy(mm));
	result->staticTypingLite(opt.getContext());

#if LOG_RULES
	l->logTransformation(opt.getLog(), "Right lookup to left step", IntersectQP::logIntersectBefore(l, r), result);
#endif

	return result;
}

QueryPlan *RightLookupToLeftStep::doWork(QueryPlan *qp)
{
	// See if we want to convert right_ to using conventional navigation
	switch(qp->getType()) {
	case QueryPlan::SEQUENTIAL_SCAN:
	case QueryPlan::PRESENCE:
	case QueryPlan::VALUE:
	case QueryPlan::RANGE: {
		ImpliedSchemaNode::Type type = StructuralJoinQP::findType(qp);

		if(type == ImpliedSchemaNode::METADATA) return qp;
		if(type == ImpliedSchemaNode::ATTRIBUTE && joinType != Join::ATTRIBUTE &&
			joinType != Join::ATTRIBUTE_OR_CHILD)
			return qp;
		break;
	}
	default: return qp;
	}

	success = true;
	if(trialRun) return qp;

	return StepQP::createStep(leftArg, joinType, qp, 0, qp, mm);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//
// PushBackJoin: d(*(1), c(*(2), *(3))) -> c(*(2), d(*(1), *(3)))
//
// PushBackJoin: d(*(1), u(*(2), *(3))) -> BUF(0, *(1), u(d(BR(0), *(2)), d(BR(0), *(3))))
//

QueryPlan *PushBackJoin::run(const StructuralJoinQP *sj, OptimizationContext &o,
	XPath2MemoryManager *m)
{
	leftArg = sj->getLeftArg();
	joinType = StructuralJoinQP::getJoinType(sj->getType());
	location = sj;
	flags = sj->getFlags();
	opt = &o;
	mm = m;

	if(joinType == Join::DESCENDANT_OR_SELF && StructuralJoinQP::findType(leftArg) == ImpliedSchemaNode::METADATA)
		return 0;
	if((flags & QueryPlan::SKIP_PUSH_BACK_JOIN) != 0) return 0;

	// Do a trial run first, to see if this will succeed
	trialRun = true;
	success = false;

	skip(sj->getRightArg());
	if(!success) return 0;

	// Now do the transform for real, using a copy of the arguments
	trialRun = false;

	leftArg = sj->getLeftArg()->copy(mm);
	QueryPlan *result = skip(sj->getRightArg()->copy(mm));
	result->staticTypingLite(opt->getContext());

#if LOG_RULES
	sj->logTransformation(opt->getLog(), "Push back join", sj, result);
#endif
	return result;
}

QueryPlan *PushBackJoin::run(QueryPlan *l, QueryPlan *r, u_int32_t f, const LocationInfo *loc,
	OptimizationContext &o, XPath2MemoryManager *m)
{
	leftArg = l;
	joinType = Join::SELF;
	location = loc;
	flags = f;
	opt = &o;
	mm = m;

	if(StructuralJoinQP::findType(leftArg) == ImpliedSchemaNode::METADATA)
		return 0;
	if((flags & QueryPlan::SKIP_PUSH_BACK_JOIN) != 0) return 0;

	// Do a trial run first, to see if this will succeed
	trialRun = true;
	success = false;

	skip(r);
	if(!success) return 0;

	// Now do the transform for real, using a copy of the arguments
	trialRun = false;

	leftArg = l->copy(mm);
	QueryPlan *result = skip(r->copy(mm));
	result->staticTypingLite(opt->getContext());

#if LOG_RULES
	l->logTransformation(opt->getLog(), "Push back join", IntersectQP::logIntersectBefore(l, r), result);
#endif
	return result;
}

QueryPlan *PushBackJoin::skipStructuralJoin(StructuralJoinQP *rsj)
{
	if(!StructuralJoinQP::isDocumentIndex(rsj->getLeftArg())) {
		success = true;
		if(trialRun) return rsj;

		QueryPlan *join = StructuralJoinQP::createJoin(joinType, leftArg, rsj->getRightArg(), 0, location, mm);
		rsj->setRightArg(join);
		rsj->setFlags(QueryPlan::SKIP_PUSH_BACK_JOIN);

		return rsj;
	}

	return FilterSkipper::skipStructuralJoin(rsj);
}

// PushBackJoin: d(*(1), u(*(2), *(3))) -> BUF(0, *(1), u(d(BR(0), *(2)), d(BR(0), *(3))))
QueryPlan *PushBackJoin::doWork(QueryPlan *qp)
{
	switch(qp->getType()) {
	case QueryPlan::UNION: {
		UnionQP *op = (UnionQP*)qp;

		if(trialRun) {
			success = true;
			OperationQP::Vector &args = const_cast<OperationQP::Vector &>(op->getArgs());
			for(OperationQP::Vector::iterator i = args.begin(); i != args.end(); ++i) {
				if(StructuralJoinQP::isDocumentIndex(*i)) {
					success = false;
					break;
				}
			}
			return qp;
		}
		else {
			unsigned int bufferId = GET_CONFIGURATION(opt->getContext())->allocateBufferID();
			BufferQP *buffer = new (mm) BufferQP(leftArg, 0, bufferId, 0, mm);
			buffer->setLocationInfo(leftArg);

			OperationQP::Vector &args = const_cast<OperationQP::Vector &>(op->getArgs());
			for(OperationQP::Vector::iterator i = args.begin(); i != args.end(); ++i) {
				BufferReferenceQP *ref = new (mm) BufferReferenceQP(buffer, 0, mm);
				ref->setBuffer(buffer);
				ref->setLocationInfo(buffer);

				*i = StructuralJoinQP::createJoin(joinType, ref, *i, 0, location, mm);
			}

			op->setFlags(QueryPlan::SKIP_PUSH_BACK_JOIN);

			buffer->setArg(op);
			return buffer;
		}

		break;
	}
	default: break;
	}

	return qp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//
// SwapStep: d(*(1), step(child::foo, *(2))) -> c(*(2), step(descendant::foo, *(1)))
//

QueryPlan *SwapStep::run(const StructuralJoinQP *sj, OptimizationContext &opt,
	XPath2MemoryManager *m)
{
	leftArg = sj->getLeftArg();
	joinType = StructuralJoinQP::getJoinType(sj->getType());
	location = sj;
	flags = sj->getFlags();
	mm = m;

	if(joinType == Join::DESCENDANT_OR_SELF && StructuralJoinQP::findType(leftArg) == ImpliedSchemaNode::METADATA)
		return 0;
	if((flags & QueryPlan::SKIP_SWAP_STEP) != 0) return 0;
	if((flags & QueryPlan::SKIP_PUSH_BACK_JOIN) != 0) return 0;

	// Do a trial run first, to see if this will succeed
	trialRun = true;
	success = false;

	skip(sj->getRightArg());
	if(!success) return 0;

	// Now do the transform for real, using a copy of the arguments
	trialRun = false;

	leftArg = sj->getLeftArg()->copy(mm);
	QueryPlan *result = skip(sj->getRightArg()->copy(mm));
	result->staticTypingLite(opt.getContext());

#if LOG_RULES
	sj->logTransformation(opt.getLog(), "Swap step", sj, result);
#endif
	return result;
}

QueryPlan *SwapStep::run(QueryPlan *l, QueryPlan *r, u_int32_t f, const LocationInfo *loc,
	OptimizationContext &opt, XPath2MemoryManager *m)
{
	leftArg = l;
	joinType = Join::SELF;
	location = loc;
	flags = f;
	mm = m;

	if(StructuralJoinQP::findType(leftArg) == ImpliedSchemaNode::METADATA)
		return 0;
	if((flags & QueryPlan::SKIP_SWAP_STEP) != 0) return 0;
	if((flags & QueryPlan::SKIP_PUSH_BACK_JOIN) != 0) return 0;

	// Do a trial run first, to see if this will succeed
	trialRun = true;
	success = false;

	skip(r);
	if(!success) return 0;

	// Now do the transform for real, using a copy of the arguments
	trialRun = false;

	leftArg = l->copy(mm);
	QueryPlan *result = skip(r->copy(mm));
	result->staticTypingLite(opt.getContext());

#if LOG_RULES
	l->logTransformation(opt.getLog(), "Swap step", IntersectQP::logIntersectBefore(l, r), result);
#endif
	return result;
}

QueryPlan *SwapStep::doWork(QueryPlan *qp)
{
	if(qp->getType() == QueryPlan::STEP) {
		StepQP *step = (StepQP*)qp;

		if(!StructuralJoinQP::joinSupported(step->getJoinType())) return qp;

		success = true;
		if(trialRun) return qp;

		QueryPlan *join = StructuralJoinQP::createJoin(step->getJoinType(), step->getArg(), step,
			QueryPlan::SKIP_SWAP_STEP, location, mm);
		step->setArg(leftArg);
		step->setJoinType(joinType);
		step->setFlags(0);

		return join;
	}

	return qp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//
// LeftToPredicate: d(*(1), *(2)) -> NPF(#tmp, *(2), d(*(1), VAR(#tmp)))
//

class SuitableForPredicate : private NodeVisitingOptimizer
{
public:
	bool check(const QueryPlan *qp)
	{
		suitable_ = true;
		optimizeQP(const_cast<QueryPlan*>(qp));
		return suitable_;
	}

private:
	virtual void resetInternal() {}

	virtual QueryPlan *optimizeDecisionPoint(DecisionPointQP *item)
	{
		// Don't look inside nested DecisionPointQP's arguments
		if(item->getSource())
			item->setSource(optimizeDPS(item->getSource()));
		return item;
	}

	virtual QueryPlan *optimizeDecisionPointEnd(DecisionPointEndQP *item)
	{
		// DecisionPointEndQP objects for the current DecisionPointQP
		// can't be evaluated more than once
		suitable_ = false;
		return item;
	}

	bool suitable_;
};

QueryPlan *LeftToPredicate::run(const StructuralJoinQP *sj, OptimizationContext &opt, XPath2MemoryManager *mm)
{
	// Don't apply this query rewrite to document index joins
	if(sj->getType() == QueryPlan::DESCENDANT_OR_SELF &&
		StructuralJoinQP::findType(sj->getLeftArg()) == ImpliedSchemaNode::METADATA)
		return 0;

	switch(sj->getLeftArg()->getType()) {
	case QueryPlan::VARIABLE: return 0;
	default: break;
	}

	if(!SuitableForPredicate().check(sj->getLeftArg()))
		return 0;

	if((sj->getFlags() & QueryPlan::SKIP_LEFT_TO_PREDICATE) != 0) return 0;

	const XMLCh *varName = GET_CONFIGURATION(opt.getContext())->allocateTempVarName(mm);

	QueryPlan *var = new (mm) VariableQP(0, 0, varName, StepQP::findContainer(sj->getRightArg()), StepQP::findNodeTest(sj->getRightArg()), 0, mm);
	const_cast<StaticAnalysis&>(var->getStaticAnalysis())
		.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
			StaticAnalysis::ONENODE | StaticAnalysis::SELF);
	var->setLocationInfo(sj->getRightArg());

	QueryPlan *join = StructuralJoinQP::createJoin(StructuralJoinQP::getJoinType(sj->getType()), sj->getLeftArg()->copy(mm), var,
		QueryPlan::SKIP_LEFT_TO_PREDICATE, sj, mm);

	QueryPlan *result = new (mm) NodePredicateFilterQP(sj->getRightArg()->copy(mm), join, 0, varName, 0, mm);
	result->setLocationInfo(sj);
	result->addFlag(QueryPlan::SKIP_NESTED_PREDICATES);
	result->staticTypingLite(opt.getContext());

#if LOG_RULES
	sj->logTransformation(opt.getLog(), "Left to predicate", sj, result);
#endif
	return result;
}

QueryPlan *LeftToPredicate::run(QueryPlan *l, QueryPlan *r, u_int32_t flags, const LocationInfo *loc,
	OptimizationContext &opt, XPath2MemoryManager *mm)
{
	// Don't apply this query rewrite to document index joins
	if(StructuralJoinQP::findType(l) == ImpliedSchemaNode::METADATA)
		return 0;

	switch(l->getType()) {
	case QueryPlan::VARIABLE: return 0;
	default: break;
	}

	if(!SuitableForPredicate().check(l))
		return 0;

	if((flags & QueryPlan::SKIP_LEFT_TO_PREDICATE) != 0) return 0;

	const XMLCh *varName = GET_CONFIGURATION(opt.getContext())->allocateTempVarName(mm);

	QueryPlan *var = new (mm) VariableQP(0, 0, varName, StepQP::findContainer(r), StepQP::findNodeTest(r), 0, mm);
	const_cast<StaticAnalysis&>(var->getStaticAnalysis())
		.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
			StaticAnalysis::ONENODE | StaticAnalysis::SELF);
	var->setLocationInfo(r);

	QueryPlan *join = StructuralJoinQP::createJoin(Join::SELF, l->copy(mm), var,
		QueryPlan::SKIP_LEFT_TO_PREDICATE, loc, mm);

	QueryPlan *result = new (mm) NodePredicateFilterQP(r->copy(mm), join, 0, varName, 0, mm);
	result->setLocationInfo(loc);
	result->addFlag(QueryPlan::SKIP_NESTED_PREDICATES);
	result->staticTypingLite(opt.getContext());

#if LOG_RULES
	l->logTransformation(opt.getLog(), "Left to predicate", IntersectQP::logIntersectBefore(l, r), result);
#endif
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//
// ReverseJoin: NPF(#var, *(1), d(*(2), *(3))) -> NPF(#var, *(1), a(*(3), *(2)))
//

QueryPlan *ReverseJoin::run(const NodePredicateFilterQP *npf, OptimizationContext &opt,
	XPath2MemoryManager *mm)
{
	if((npf->getFlags() & QueryPlan::SKIP_REVERSE_JOIN) != 0) return 0;

	// Do a trial run first, to see if this will succeed
	sj = 0;
	trialRun = true;

	skip(npf->getPred());
	if(sj == 0) return 0;

	// Now do the transform for real, using a copy of the arguments
	trialRun = false;

	QueryPlan *newRightArg = skip(npf->getPred()->copy(mm));

	QueryPlan *join = StructuralJoinQP::createJoin(Join::inverse(StructuralJoinQP::getJoinType(sj->getType())),
		newRightArg, sj->getLeftArg(), QueryPlan::SKIP_REVERSE_JOIN, sj, mm);

	QueryPlan *result = new (mm) NodePredicateFilterQP(npf->getArg()->copy(mm), join,
		npf->getURI(), npf->getName(), QueryPlan::SKIP_REVERSE_JOIN, mm);
	result->setLocationInfo(npf);
	result->staticTypingLite(opt.getContext());

#if LOG_RULES
	npf->logTransformation(opt.getLog(), "Reverse join", npf, result);
#endif
	return result;
}

QueryPlan *ReverseJoin::run(const NegativeNodePredicateFilterQP *npf, OptimizationContext &opt,
	XPath2MemoryManager *mm)
{
	if((npf->getFlags() & QueryPlan::SKIP_REVERSE_JOIN) != 0) return 0;

	// Do a trial run first, to see if this will succeed
	sj = 0;
	trialRun = true;

	skip(npf->getPred());
	if(sj == 0) return 0;

	// Now do the transform for real, using a copy of the arguments
	trialRun = false;

	QueryPlan *newRightArg = skip(npf->getPred()->copy(mm));

	QueryPlan *join = StructuralJoinQP::createJoin(Join::inverse(StructuralJoinQP::getJoinType(sj->getType())),
		newRightArg, sj->getLeftArg(), QueryPlan::SKIP_REVERSE_JOIN, sj, mm);

	QueryPlan *result = new (mm) NegativeNodePredicateFilterQP(npf->getArg()->copy(mm), join,
		npf->getURI(), npf->getName(), QueryPlan::SKIP_REVERSE_JOIN, mm);
	result->setLocationInfo(npf);
	result->staticTypingLite(opt.getContext());

#if LOG_RULES
	npf->logTransformation(opt.getLog(), "Reverse join", npf, result);
#endif
	return result;
}

QueryPlan *ReverseJoin::skipStructuralJoin(StructuralJoinQP *psj)
{
	if((psj->getFlags() & QueryPlan::SKIP_REVERSE_JOIN) == 0 &&
		!StructuralJoinQP::isDocumentIndex(psj->getLeftArg(), /*toBeRemoved*/true)) {
		sj = psj;
		if(trialRun) return psj;

		return psj->getRightArg();
	}

	return FilterSkipper::skipStructuralJoin(psj);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

QueryPlan *PullForwardDocumentJoin::run(QueryPlan *qp)
{
	input = qp;
	documentJoin = 0;

	QueryPlan *result = skip(qp);
	if(documentJoin == 0) return 0;

	documentJoin->setRightArg(result);
	return documentJoin;
}

bool PullForwardDocumentJoin::canFind(QueryPlan *qp)
{
	return true;
}

QueryPlan *PullForwardDocumentJoin::skipStructuralJoin(StructuralJoinQP *sj)
{
	if(canFind(sj) && sj->getType() == QueryPlan::DESCENDANT_OR_SELF &&
		StructuralJoinQP::findType(sj->getLeftArg()) == ImpliedSchemaNode::METADATA) {
		if(((QueryPlan*)sj) == input) return sj;

		documentJoin = sj;
		return sj->getRightArg();
	}

	sj->setLeftArg(skip(sj->getLeftArg()));
	if(documentJoin == 0)
		sj->setRightArg(skip(sj->getRightArg()));
	return sj;
}

QueryPlan *PullForwardDocumentJoin::doWork(QueryPlan *qp)
{
	switch(qp->getType()) {
	case QueryPlan::STEP: {
		StepQP *step = (StepQP*)qp;
		step->setArg(skip(step->getArg()));
		break;
	}
	case QueryPlan::INTERSECT: {
		OperationQP::Vector &args = const_cast<OperationQP::Vector&>(((OperationQP*)qp)->getArgs());
		for(OperationQP::Vector::iterator i = args.begin(); i != args.end(); ++i) {
			*i = skip(*i);
			if(documentJoin != 0) break;
		}
		break;
	}
	default: break;
	}

	return qp;
}

