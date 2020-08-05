//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ExceptQP.hpp"
#include "NodePredicateFilterQP.hpp"
#include "EmptyQP.hpp"
#include "StructuralJoinQP.hpp"
#include "VariableQP.hpp"
#include "StepQP.hpp"
#include "IntersectQP.hpp"
#include "QueryPlanToAST.hpp"
#include "QueryExecutionContext.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/functions/FunctionEmpty.hpp>
#include <xqilla/operators/Or.hpp>

#include <xercesc/util/XMLString.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

static inline bool char_equals(const char *a, const char *b) {
	return XMLString::equals(a, b);
}

ExceptQP::ExceptQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(EXCEPT, flags, mm),
	  left_(l),
	  right_(r)
{
}

QueryPlan *ExceptQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	left_ = left_->staticTyping(context, styper);
	_src.copy(left_->getStaticAnalysis());

	right_ = right_->staticTyping(context, styper);
	_src.add(right_->getStaticAnalysis());

	unsigned int min = 0;
	if(_src.getStaticType().getMin() > right_->getStaticAnalysis().getStaticType().getMax())
		min = _src.getStaticType().getMin() - right_->getStaticAnalysis().getStaticType().getMax();

	_src.getStaticType().setCardinality(min, _src.getStaticType().getMax());

	return this;
}

void ExceptQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	left_->staticTypingLite(context);
	_src.copy(left_->getStaticAnalysis());

	right_->staticTypingLite(context);
	_src.add(right_->getStaticAnalysis());

	unsigned int min = 0;
	if(_src.getStaticType().getMin() > right_->getStaticAnalysis().getStaticType().getMax())
		min = _src.getStaticType().getMin() - right_->getStaticAnalysis().getStaticType().getMax();

	_src.getStaticType().setCardinality(min, _src.getStaticType().getMax());
}

class RemoveNodePredicateFilters : private FilterSkipper
{
public:
	virtual QueryPlan *run(QueryPlan *qp, NodePredicateFilterQP *npf)
	{
		toMatch = npf;
		return skip(qp);
	}

private:
	NodePredicateFilterQP *toMatch;

	virtual QueryPlan *skipFilter(FilterQP *item)
	{
		// TBD Not sure these subsets will work - fix this - jpcs
		if(item->getType() == QueryPlan::NODE_PREDICATE_FILTER) {
			NodePredicateFilterQP *pred = (NodePredicateFilterQP*)item;
			if(toMatch->getPred()->isSubsetOf(pred->getPred())) {
				return pred->getArg();
			}
		}
		return FilterSkipper::skipFilter(item);
	}
};

class RemoveNegativeNodePredicateFilters : private FilterSkipper
{
public:
	virtual QueryPlan *run(QueryPlan *qp, NegativeNodePredicateFilterQP *npf)
	{
		toMatch = npf;
		return skip(qp);
	}

private:
	NegativeNodePredicateFilterQP *toMatch;

	virtual QueryPlan *skipFilter(FilterQP *item)
	{
		// TBD Not sure these subsets will work - fix this - jpcs
		if(item->getType() == QueryPlan::NEGATIVE_NODE_PREDICATE_FILTER) {
			NegativeNodePredicateFilterQP *pred = (NegativeNodePredicateFilterQP*)item;
			if(toMatch->getPred()->isSubsetOf(pred->getPred())) {
				return pred->getArg();
			}
		}
		return FilterSkipper::skipFilter(item);
	}
};

class RemoveLevelFilters : private FilterSkipper
{
public:
	virtual QueryPlan *run(QueryPlan *qp)
	{
		return skip(qp);
	}

private:
	virtual QueryPlan *skipFilter(FilterQP *item)
	{
		if(item->getType() == QueryPlan::LEVEL_FILTER) {
			return item->getArg();
		}
		return FilterSkipper::skipFilter(item);
	}
};

class PullForwardDocumentJoinForExcept : public PullForwardDocumentJoin
{
public:
	virtual QueryPlan *run(QueryPlan *qp)
	{
		inExcept = false;

		return PullForwardDocumentJoin::run(qp);
	}

protected:
	virtual QueryPlan *skipExcept(ExceptQP *item)
	{
		item->setLeftArg(skip(item->getLeftArg()));

		if(documentJoin == 0) {
			inExcept = !inExcept;
			item->setRightArg(skip(item->getRightArg()));
			inExcept = !inExcept;
		}

		return item;
	}

	virtual bool canFind(QueryPlan *qp)
	{
		return qp != input && !inExcept;
	}

	bool inExcept;
};

//
// ExceptToNegativePredicate: e(*(1), FILTER(*(1))) -> NNPF(#tmp, *(1), FILTER(VAR(#tmp)))
//
class ExceptToNegativePredicate : private FilterSkipper
{
public:
	QueryPlan *run(const ExceptQP *e, OptimizationContext &opt)
	{
		ex = e;
		mm = opt.getMemoryManager();

		if((ex->getFlags() & QueryPlan::SKIP_TO_NEGATIVE_PREDICATE) != 0) return 0;

		// Do a trial run first, to see if this will succeed
		trialRun = true;
		success = false;

		skip(ex->getRightArg());
		if(!success) return 0;

		// Now do the transform for real, using a copy of the arguments
		trialRun = false;
		varName = GET_CONFIGURATION(opt.getContext())->allocateTempVarName(mm);

		QueryPlan *rightArg = skip(ex->getRightArg()->copy(mm));

		QueryPlan *result = new (mm) NegativeNodePredicateFilterQP(ex->getLeftArg()->copy(mm), rightArg, 0, varName, 0, mm);
		result->setLocationInfo(ex);
		result->staticTypingLite(opt.getContext());

#if LOG_RULES
		ex->logTransformation(opt.getLog(), "Except to negative predicate", ex, result);
#endif
		return result;
	}

private:
	virtual QueryPlan *doWork(QueryPlan *qp)
	{
		if(ex->getLeftArg()->isSubsetOf(qp)) {
			success = true;
			if(trialRun) return qp;

			QueryPlan *var = new (mm) VariableQP(0, 0, varName, StepQP::findContainer(ex->getLeftArg()), StepQP::findNodeTest(ex->getLeftArg()), 0, mm);
			const_cast<StaticAnalysis&>(var->getStaticAnalysis()).setProperties(StaticAnalysis::ONENODE);
			var->setLocationInfo(ex->getLeftArg());

			qp->release();
			return var;
		}
		if(qp->isSubsetOf(ex->getLeftArg())) {
			success = true;
			if(trialRun) return qp;

			QueryPlan *var = new (mm) VariableQP(0, 0, varName, StepQP::findContainer(ex->getLeftArg()), StepQP::findNodeTest(ex->getLeftArg()), 0, mm);
			const_cast<StaticAnalysis&>(var->getStaticAnalysis()).setProperties(StaticAnalysis::ONENODE);
			var->setLocationInfo(ex->getLeftArg());

			IntersectQP *intersect = new (mm) IntersectQP(var, qp, 0, mm);
			intersect->setLocationInfo(ex->getLeftArg());

			return intersect;
		}

		return qp;
	}

	const ExceptQP *ex;
	const XMLCh *varName;

	XPath2MemoryManager *mm;
	bool success;
	bool trialRun;
};

QueryPlan *ExceptQP::optimize(OptimizationContext &opt)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Optimize the arguments
	left_ = left_->optimize(opt);
	right_ = right_->optimize(opt);

	if(left_->isSubsetOf(right_)) {
		EmptyQP *empty = new (mm) EmptyQP(0, mm);
		empty->setLocationInfo(this);
		return empty;
	}

	// Pull forward filters
	switch(left_->getType()) {
	// We can't skip NUMERIC_PREDICATE_FILTER, because changing it's input will change
	// the cardinality of it's input, and cause it to return the wrong results.
// 	case NUMERIC_PREDICATE_FILTER:
// 	case DOC_EXISTS:
	case VALUE_FILTER:
	case PREDICATE_FILTER: {
		string before = logBefore(this);

		FilterQP *filter = (FilterQP*)left_;

		left_ = filter->getArg();
		filter->setArg(this);

		logTransformation(opt.getLog(), "Filter pulled forward", before, filter);
		return filter->optimize(opt);
	}
	case LEVEL_FILTER: {
		string before = logBefore(this);

		FilterQP *filter = (FilterQP*)left_;

		left_ = filter->getArg();
		filter->setArg(this);

		right_ = RemoveLevelFilters().run(right_);

		logTransformation(opt.getLog(), "Filter pulled forward", before, filter);
		return filter->optimize(opt);
	}
	case NODE_PREDICATE_FILTER: {
		string before = logBefore(this);

		NodePredicateFilterQP *filter = (NodePredicateFilterQP*)left_;

		left_ = filter->getArg();
		filter->setArg(this);

		right_ = RemoveNodePredicateFilters().run(right_, filter);

		logTransformation(opt.getLog(), "Filter pulled forward", before, filter);
		return filter->optimize(opt);
	}
	case NEGATIVE_NODE_PREDICATE_FILTER: {
		string before = logBefore(this);

		NegativeNodePredicateFilterQP *filter = (NegativeNodePredicateFilterQP*)left_;

		left_ = filter->getArg();
		filter->setArg(this);

		right_ = RemoveNegativeNodePredicateFilters().run(right_, filter);

		logTransformation(opt.getLog(), "Filter pulled forward", before, filter);
		return filter->optimize(opt);
	}
	default: break;
	}

	// Create a before string, in case we need it for logging
	string before = logBefore(this);
	QueryPlan *result;

	// Try to pull forward document indexes, so they will combine
	if(opt.getPhase() < OptimizationContext::ALTERNATIVES) {
		result = PullForwardDocumentJoinForExcept().run(this);
		if(result != 0) {
			logTransformation(opt.getLog(), "Pull forward document join", before, result);
			return result->optimize(opt);
		}
	}

	return this;
}

void ExceptQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Try this transformation before the arguments are transformed,
	// because it can't often be applied to transformed arguments
	AutoRelease<QueryPlan> result(ExceptToNegativePredicate().run(this, opt));
	if(result != 0) result->createCombinations(maxAlternatives, opt, combinations);

	// Generate the alternatives for the arguments
	QueryPlans leftAltArgs;
	left_->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, maxAlternatives, opt, leftAltArgs);

	QueryPlans rightAltArgs;
	right_->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, maxAlternatives, opt, rightAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = leftAltArgs.begin(); it != leftAltArgs.end(); ++it) {
		for(QueryPlans::iterator it2 = rightAltArgs.begin(); it2 != rightAltArgs.end(); ++it2) {
			ExceptQP *newEx = new (mm) ExceptQP((*it)->copy(mm), (*it2)->copy(mm), flags_, mm);
			newEx->setLocationInfo(this);
			newEx->addFlag(SKIP_TO_NEGATIVE_PREDICATE);

			combinations.push_back(newEx);
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

void ExceptQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	alternatives.push_back(this);
}

NodeIterator *ExceptQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> li(left_->createNodeIterator(context));
	AutoDelete<NodeIterator> ri(right_->createNodeIterator(context));
	return new ExceptIterator(li.adopt(), ri.adopt(), this);
}

Cost ExceptQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost result = left_->cost(context, qec);

	Cost rCost = right_->cost(context, qec);

	result.pagesOverhead += rCost.pagesOverhead;

	// TBD could be better? - jpcs
	if(rCost.keys > result.keys) {
		result.pagesOverhead += (rCost.pagesForKeys / rCost.keys) * result.keys;
	} else {
		result.pagesOverhead += rCost.pagesForKeys;
	}

	// Take a token key away from the result, because it is likely to
	// return less nodes than the argument - we just don't know how many less.
	if(result.keys > 1) result.keys -= 1;

	return result;
}

bool ExceptQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == EXCEPT) {
		ExceptQP *ex = (ExceptQP*)o;
		if(left_->isSubsetOf(ex->left_) && ex->right_->isSubsetOf(right_))
			return true;
	}

	// The result of this QP is a subset of it's right argument
	return left_->isSubsetOf(o);
}

QueryPlan *ExceptQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	ExceptQP *result = new (mm) ExceptQP(left_->copy(mm), right_->copy(mm), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void ExceptQP::release()
{
	left_->release();
	right_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

string ExceptQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<ExceptQP>" << endl;
	s << left_->printQueryPlan(context, indent + INDENT);
	s << right_->printQueryPlan(context, indent + INDENT);
	s << in <<  "</ExceptQP>" << endl;

	return s.str();
}

string ExceptQP::toString(bool brief) const
{
	ostringstream s;

	s << "e(";
	s << left_->toString(brief);
	s << ",";
	s << right_->toString(brief);
	s << ")";

	return s.str();
}

void ExceptQP::findQueryPlanRoots(QPRSet &qprset) const
{
	left_->findQueryPlanRoots(qprset);
	right_->findQueryPlanRoots(qprset);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ExceptIterator::ExceptIterator(NodeIterator *left, NodeIterator *right, const LocationInfo *o)
	: ProxyIterator(o),
	  left_(left),
	  right_(right),
	  toDo_(true)
{
	result_ = left_;
}

ExceptIterator::~ExceptIterator()
{
	delete left_;
	delete right_;
}

bool ExceptIterator::next(DynamicContext *context)
{
	if(!left_->next(context)) return false;

	if(toDo_) {
		toDo_ = false;
		if(!right_->seek(left_, context)) {
			delete right_;
			right_ = 0;
		}
	}

	return doJoin(context);
}

bool ExceptIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!left_->seek(container, did, nid, context)) return false;

	if(toDo_) {
		toDo_ = false;
		if(!right_->seek(left_, context)) {
			delete right_;
			right_ = 0;
		}
	}

	return doJoin(context);
}

bool ExceptIterator::doJoin(DynamicContext *context)
{
	while(true) {
		context->testInterrupt();

		if(right_ == 0) {
			return true;
		}

		int cmp = compare(left_, right_);
		if(cmp < 0) {
			return true;
		} else if(cmp > 0) {
			if(!right_->seek(left_, context)) {
				delete right_;
				right_ = 0;
			}
		} else {
			if(!left_->next(context)) break;
		}
	}

	return false;
}

