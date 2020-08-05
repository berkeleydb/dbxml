//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "IntersectQP.hpp"
#include "StructuralJoinQP.hpp"
#include "FilterQP.hpp"
#include "StepQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"

#include <xqilla/context/DynamicContext.hpp>

#include <xercesc/util/XMLString.hpp>

#include <sstream>
#include <math.h>

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

static inline bool char_equals(const char *a, const char *b) {
	return XMLString::equals(a, b);
}

class UniqueArgs
{
public:
	typedef set<QueryPlan*>::iterator iterator;

	iterator begin() {
		return _s.begin();
	}
	iterator end() {
		return _s.end();
	}

	void add(QueryPlan::Type type, QueryPlan *arg) {
		if(arg) {
			if(arg->getType() == type) {
				const OperationQP::Vector &args  = ((OperationQP*)arg)->getArgs();
				OperationQP::Vector::const_iterator end = args.end();
				for(OperationQP::Vector::const_iterator i = args.begin();
				    i != end; ++i) add(type, *i);
			}
			else {
				_s.insert(arg);
			}
		}
	}

	void clear() {
		_s.clear();
	}

private:
	set<QueryPlan*> _s;
};

IntersectQP::IntersectQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: OperationQP(QueryPlan::INTERSECT, flags, mm)
{
	addArg(l);
	addArg(r);
}

QueryPlan *IntersectQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	// Remove duplicate arguments
	vector<QueryPlan*> newArgs;
	Vector::iterator it = args_.begin();
	if(it != args_.end()) {
		QueryPlan *arg = (*it)->staticTyping(context, styper);
		_src.copy(arg->getStaticAnalysis());
		newArgs.push_back(arg);

		for(++it; it != args_.end(); ++it) {
			QueryPlan *arg = (*it)->staticTyping(context, styper);
			_src.add(arg->getStaticAnalysis());
			_src.getStaticType().typeNodeIntersect(arg->getStaticAnalysis().getStaticType());
			_src.setProperties(_src.getProperties() | arg->getStaticAnalysis().getProperties());
			newArgs.push_back(arg);
		}

		args_.clear();
		std::copy(newArgs.begin(), newArgs.end(), back_inserter(args_));
	}

	_src.getStaticType().multiply(0, 1);
	return dissolve();
}

void IntersectQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	Vector::iterator it = args_.begin();
	if(it != args_.end()) {
		(*it)->staticTypingLite(context);
		_src.copy((*it)->getStaticAnalysis());

		for(++it; it != args_.end(); ++it) {
			(*it)->staticTypingLite(context);
			_src.add((*it)->getStaticAnalysis());
			_src.getStaticType().typeNodeIntersect((*it)->getStaticAnalysis().getStaticType());
			_src.setProperties(_src.getProperties() | (*it)->getStaticAnalysis().getProperties());
		}
	}
	_src.getStaticType().multiply(0, 1);
}

string IntersectQP::logIntersectBefore(const QueryPlan *left, const QueryPlan *right)
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		ostringstream s;

		s << "n(";

		s << left->toString() << ",";
		s << right->toString();

		s << ")";

		return shorten(s.str(), MAX_NAME_LENGTH);
	}
	return "";
}

static inline bool isLessThanOrGreaterThan(const QueryPlan *l)
{
	return l->getType() == QueryPlan::VALUE &&
		(((ValueQP *)l)->getOperation() == DbWrapper::LTX ||
		 ((ValueQP *)l)->getOperation() == DbWrapper::LTE ||
		 ((ValueQP *)l)->getOperation() == DbWrapper::GTX ||
		 ((ValueQP *)l)->getOperation() == DbWrapper::GTE);
}

class keys_compare_less
{
public:
	keys_compare_less(OperationContext &context, QueryExecutionContext &qec)
		: context_(context), qec_(qec) {}

	bool operator()(const QueryPlan *l, const QueryPlan *r) const
	{
		return l->cost(context_, qec_).compare(r->cost(context_, qec_)) < 0;
	}

private:
	OperationContext &context_;
	QueryExecutionContext &qec_;
};

QueryPlan *IntersectQP::optimize(OptimizationContext &opt)
{
	// Optimize the arguments
	vector<QueryPlan*> newArgs;
	Vector::iterator it;
	for(it = args_.begin(); it != args_.end(); ++it) {
		QueryPlan *arg = (*it)->optimize(opt);
		if(arg->getType() == type_) {
			// Merge IntersectQP arguments into this object
			const Vector &args  = ((OperationQP*)arg)->getArgs();
			Vector::const_iterator aend = args.end();
			for(Vector::const_iterator ai = args.begin();
			    ai != aend; ++ai) newArgs.push_back(*ai);
		} else {
			newArgs.push_back(arg);
		}
	}
	args_.clear();
	std::copy(newArgs.begin(), newArgs.end(), back_inserter(args_));

	removeSupersets(opt);

	// Return if we only have one argument
	if(args_.size() == 1) return args_[0];

	// Pull forward filters
	for(it = args_.begin(); it != args_.end(); ++it) {
		switch((*it)->getType()) {
		case VALUE_FILTER:
		case PREDICATE_FILTER:
		case NODE_PREDICATE_FILTER:
		case NEGATIVE_NODE_PREDICATE_FILTER:
		// We can't skip NUMERIC_PREDICATE_FILTER, because changing it's input will change
		// the cardinality of it's input, and cause it to return the wrong results.
// 		case NUMERIC_PREDICATE_FILTER:
// 		case DOC_EXISTS:
		case LEVEL_FILTER: {
			string before = logBefore(this);

			FilterQP *filter = (FilterQP*)*it;

			*it = filter->getArg();
			filter->setArg(this);

			logTransformation(opt.getLog(), "Filter pulled forward", before, filter);
			return filter->optimize(opt);
		}
		default: break;
		}
	}

	// Identify potential pairs of ValueQP for a RangeQP
	newArgs.clear();
	for(it = args_.begin(); it != args_.end(); ++it) {
		if(isLessThanOrGreaterThan(*it)) {
			for(Vector::iterator it2 = it + 1;
			    it2 != args_.end(); ++it2) {
				if(isLessThanOrGreaterThan(*it2)) {
					QueryPlan *range = createRange((ValueQP*)*it, (ValueQP*)*it2);
					if(range != 0) {
						logTransformation(opt.getLog(), "Merged into range",
							logIntersectBefore(*it, *it2), range);
						newArgs.push_back(range);
					}
				}
			}
		}

		newArgs.push_back((*it)->optimize(opt));
	}
	args_.clear();
	std::copy(newArgs.begin(), newArgs.end(), back_inserter(args_));

	// Return if we only have one argument
	if(args_.size() == 1) return args_[0];

	// Try to pull forward document indexes, so they will combine
	if(opt.getPhase() < OptimizationContext::ALTERNATIVES) {
		string before = logBefore(this);
		QueryPlan *result = PullForwardDocumentJoin().run(this);
		if(result != 0) {
			logTransformation(opt.getLog(), "Pull forward document join", before, result);
			return result->optimize(opt);
		}
	}

	if(opt.getPhase() < OptimizationContext::REMOVE_REDUNDENTS)
		return this;

	// TBD remove the need for QueryExecutionContext here - jpcs
	QueryExecutionContext qec(GET_CONFIGURATION(opt.getContext())->getQueryContext(),
				  /*debugging*/false);
	qec.setContainerBase(opt.getContainerBase());
	qec.setDynamicContext(opt.getContext());

	// Sort the arguments based on how many keys we think they'll return
	sort(args_.begin(), args_.end(), keys_compare_less(opt.getOperationContext(), qec));

	// Remove document index joins if they are unlikely to be useful.
	it = args_.begin();
	QueryPlan *leftMost = *it;
	Cost lCost = leftMost->cost(opt.getOperationContext(), qec);
	newArgs.clear();
	newArgs.push_back(*it);
	for(++it; it != args_.end(); ++it) {
		if(StructuralJoinQP::isDocumentIndex(*it, /*toBeRemoved*/true) &&
			StructuralJoinQP::isSuitableForDocumentIndexComparison(leftMost)) {
			Cost itCost = (*it)->cost(opt.getOperationContext(), qec);

			// TBD Calculate these constants? - jpcs
			static const double KEY_RATIO_THRESHOLD = 2.0;
			static const double PAGES_PER_KEY_FACTOR = 2.0;

			if((itCost.keys / lCost.keys) > KEY_RATIO_THRESHOLD ||
				(itCost.totalPages() / itCost.keys) > (lCost.totalPages() * PAGES_PER_KEY_FACTOR / lCost.keys)) {
				string before = logIntersectBefore(leftMost, *it);
				logTransformation(opt.getLog(), "Remove document join", before, leftMost);
				leftMost->logCost(qec, lCost, 0);
				(*it)->logCost(qec, itCost, 0);
				continue;
			}
		}

		newArgs.push_back(*it);
	}
	args_.clear();
	std::copy(newArgs.begin(), newArgs.end(), back_inserter(args_));

	// Return if we only have one argument
	if(args_.size() == 1) return args_[0];

	return this;
}

void IntersectQP::createCombinations(unsigned int maxA, OptimizationContext &opt, QueryPlans &combinations) const
{
	double maxAlternatives = maxA;
	while(::pow(maxAlternatives, (double)args_.size()) > MAX_COMBINATIONS) {
		maxAlternatives -= 1;
	}

	// Generate the alternatives for the arguments
	vector<QueryPlans> altArgs;
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		altArgs.push_back(QueryPlans());
		(*it)->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, (unsigned int)maxAlternatives, opt, altArgs.back());
	}

	// Generate the combinations of all the alternatives for the arguments
	vector<QueryPlan*> newArgs;
	combineAltArgs(altArgs.begin(), altArgs.end(), newArgs, opt, combinations);

	// Release the alternative arguments, since they've been copied
	for(vector<QueryPlans>::iterator it3 = altArgs.begin(); it3 != altArgs.end(); ++it3) {
		for(QueryPlans::iterator it2 = it3->begin(); it2 != it3->end(); ++it2) {
			(*it2)->release();
		}
	}
}

void IntersectQP::combineAltArgs(vector<QueryPlans>::iterator argIt, vector<QueryPlans>::iterator argEnd,
	vector<QueryPlan*> &newArgs, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	if(argIt != argEnd) {
		for(QueryPlans::iterator it3 = argIt->begin(); it3 != argIt->end(); ++it3) {
			newArgs.push_back(*it3);
			combineAltArgs(argIt + 1, argEnd, newArgs, opt, combinations);
			newArgs.pop_back();
		}
	} else {
		// Construct the new IntersectQP
		IntersectQP *newIntersect = new (mm) IntersectQP(flags_, mm);
		newIntersect->setLocationInfo(this);
		for(vector<QueryPlan*>::iterator it = newArgs.begin(); it != newArgs.end(); ++it) {
			newIntersect->addArg((*it)->copy(mm));
		}

		// Add this IntersectQP to the combinations vector
		combinations.push_back(newIntersect);
	}
}

void IntersectQP::removeSupersets(OptimizationContext &opt)
{
	// Remove supersets from the arguments
	vector<QueryPlan*> newArgs;
	Vector::iterator it;
	for(it = args_.begin(); it != args_.end(); ++it) {
		bool store = true;
		for(vector<QueryPlan*>::iterator it3 = newArgs.begin();
		    it3 != newArgs.end(); ++it3) {
			if((*it3)->isSubsetOf(*it)) {
				logTransformation(opt.getLog(), "Removed superset",
					logIntersectBefore(*it3, *it), *it3);
				store = false;
				break;
			}
		}

		if(store) {
			for(Vector::iterator it2 = it + 1; it2 != args_.end(); ++it2) {
				if((*it2)->isSubsetOf(*it)) {
					logTransformation(opt.getLog(), "Removed superset",
						logIntersectBefore(*it2, *it), *it2);
					store = false;
					break;
				}
			}

			if(store) newArgs.push_back(*it);
		}
	}
	args_.clear();
	std::copy(newArgs.begin(), newArgs.end(), back_inserter(args_));
}

void IntersectQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	removeSupersets(opt);

	// Return if we only have one argument
	if(args_.size() == 1) {
		alternatives.push_back(args_[0]);
		return;
	}

	// TBD remove the need for QueryExecutionContext here - jpcs
	QueryExecutionContext qec(GET_CONFIGURATION(opt.getContext())->getQueryContext(),
				  /*debugging*/false);
	qec.setContainerBase(opt.getContainerBase());
	qec.setDynamicContext(opt.getContext());

	// Sort the arguments based on how many keys we think they'll return
	sort(args_.begin(), args_.end(), keys_compare_less(opt.getOperationContext(), qec));

	alternatives.push_back(this);

	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		for(Vector::const_iterator it2 = it + 1; it2 != args_.end(); ++it2) {
			QueryPlans alts;
			applyConversionRules2Args(maxAlternatives, *it, *it2, opt, alts);

			for(QueryPlans::iterator it3 = alts.begin(); it3 != alts.end(); ++it3) {
				IntersectQP *newIntersect = new (mm) IntersectQP(flags_, mm);
				newIntersect->setLocationInfo(this);

				Vector::const_iterator it4;
				// Copy args before "it"
				for(it4 = args_.begin(); it4 != it; ++it4) {
					newIntersect->addArg((*it4)->copy(mm));
				}
				// Add new alternative
				newIntersect->addArg(*it3);
				// Copy args from after "it" to before "it2"
				for(++it4; it4 != it2; ++it4) {
					newIntersect->addArg((*it4)->copy(mm));
				}
				// Copy args from after "it2"
				for(++it4; it4 != args_.end(); ++it4) {
					newIntersect->addArg((*it4)->copy(mm));
				}

				newIntersect->applyConversionRules(maxAlternatives, opt, alternatives);
			}
		}
	}
}

void IntersectQP::applyConversionRules2Args(unsigned int maxAlternatives, QueryPlan *l, QueryPlan *r, OptimizationContext &opt, QueryPlans &alternatives) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	u_int32_t flags = flags_;

	{
		AutoRelease<QueryPlan> result(RightLookupToLeftStep().run(l, r, flags, this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	{
		AutoRelease<QueryPlan> result(RightLookupToLeftStep().run(r, l, flags, this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	flags |= SKIP_RIGHT_LOOKUP_TO_LEFT_STEP;
	{
		AutoRelease<QueryPlan> result(PushBackJoin().run(l, r, flags, this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	{
		AutoRelease<QueryPlan> result(PushBackJoin().run(r, l, flags, this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	{
		AutoRelease<QueryPlan> result(SwapStep().run(l, r, flags, this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	{
		AutoRelease<QueryPlan> result(SwapStep().run(r, l, flags, this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	flags |= SKIP_PUSH_BACK_JOIN;
	flags |= SKIP_SWAP_STEP;
	{
		AutoRelease<QueryPlan> result(LeftToPredicate().run(l, r, flags, this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	{
		AutoRelease<QueryPlan> result(LeftToPredicate().run(r, l, flags, this, opt, mm));
		if(result.get() != 0) result->createReducedAlternatives(RULE_CUTOFF_FACTOR, maxAlternatives, opt, alternatives);
	}
	flags |= SKIP_LEFT_TO_PREDICATE;
}

NodeIterator *IntersectQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> result(0);
	Vector::const_iterator it = args_.begin();
	if(it != args_.end()) {
		result.set((*it)->createNodeIterator(context));
		for(++it; it != args_.end(); ++it) {
			result.swap(new IntersectIterator(result,
					    (*it)->createNodeIterator(context), this));
		}
	}
	return result.adopt();
}

Cost IntersectQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost result;
	Vector::const_iterator it = args_.begin();
	if(it != args_.end()) {
		result = (*it)->cost(context, qec);
		for(++it; it != args_.end(); ++it) {
			result.intersectOp((*it)->cost(context, qec));
		}
	}
	return result;
}

QueryPlan *IntersectQP::createRange(const ValueQP *l, const ValueQP *r)
{
	// TBD create range if the syntax is known - jpcs
	if(l->getNodeType() == r->getNodeType() &&
	   l->isParentSet() == r->isParentSet() &&
	   char_equals(l->getChildName(), r->getChildName()) &&
	   char_equals(l->getParentName(), r->getParentName()) &&
	   l->getValue().getASTNode() == 0 &&
	   r->getValue().getASTNode() == 0) {

		switch(l->getOperation()) {
		case DbWrapper::LTX:
		case DbWrapper::LTE: {
			if(r->getOperation() == DbWrapper::GTX || r->getOperation() == DbWrapper::GTE) {
				return new (memMgr_) RangeQP(r, l, memMgr_);
			}
			break;
		}
		case DbWrapper::GTX:
		case DbWrapper::GTE: {
			if(r->getOperation() == DbWrapper::LTX || r->getOperation() == DbWrapper::LTE) {
				return new (memMgr_) RangeQP(l, r, memMgr_);
			}
			break;
		}
		default: break;
		}
	}
	return 0;
}

bool IntersectQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::INTERSECT) {
		const IntersectQP *oi = (const IntersectQP*)o;
		for(Vector::const_iterator oit = oi->args_.begin(); oit != oi->args_.end(); ++oit) {
			bool found = false;
			for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
				if((*it)->isSubsetOf(*oit)) {
					found = true;
					break;
				}
			}
			if(found == false) return false;
		}
		return true;
	} else {
		for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
			if((*it)->isSubsetOf(o)) return true;
		}
		return false;
	}
}

bool IntersectQP::isSupersetOf(const QueryPlan *o) const
{
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		if(!o->isSubsetOf(*it)) return false;
	}
	return true;
}

QueryPlan *IntersectQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	IntersectQP *result = new (mm) IntersectQP(flags_, mm);
	result->setLocationInfo(this);

	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		result->addArg((*it)->copy(mm));
	}

	return result;
}

void IntersectQP::release()
{
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		(*it)->release();
	}
#if defined(_MSC_VER) && (_MSC_VER < 1300)
        args_.~vector<QueryPlan*,XQillaAllocator<QueryPlan *> >();
#else
	args_.~Vector();
#endif
	_src.clear();
	memMgr_->deallocate(this);
}

string IntersectQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<IntersectQP>" << endl;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		s << (*it2)->printQueryPlan(context, indent + INDENT);
	}
	s << in <<  "</IntersectQP>" << endl;

	return s.str();
}

string IntersectQP::toString(bool brief) const
{
	ostringstream s;

	s << "n(";

	bool addComma = false;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		if(addComma) { s << ","; } else { addComma = true; }
		s << (*it2)->toString(brief);
	}

	s << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

IntersectIterator::IntersectIterator(NodeIterator *left, NodeIterator *right, const LocationInfo *o)
	: ProxyIterator(o),
	  left_(left),
	  right_(right)
{
}

IntersectIterator::~IntersectIterator()
{
	delete left_;
	delete right_;
}

bool IntersectIterator::next(DynamicContext *context)
{
	if(!left_->next(context)) {
		return false;
	}
	if(!right_->seek(left_, context)) {
		return false;
	}
	return doJoin(context);
}

bool IntersectIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!left_->seek(container, did, nid, context)) {
		return false;
	}
	if(!right_->seek(left_, context)) {
		return false;
	}
	return doJoin(context);
}

bool IntersectIterator::doJoin(DynamicContext *context)
{
	while(true) {
		context->testInterrupt();

		int cmp = compare(left_, right_);
		if(cmp < 0) {
			if(!left_->seek(right_, context)) break;
		} else if(cmp > 0) {
			if(!right_->seek(left_, context)) break;
		} else {
			result_ = right_;
			return true;
		}
	}

	return false;
}

