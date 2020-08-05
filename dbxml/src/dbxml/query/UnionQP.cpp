//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "UnionQP.hpp"
#include "StructuralJoinQP.hpp"
#include "QueryExecutionContext.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"

#include <xqilla/context/DynamicContext.hpp>

#include <sstream>
#include <math.h>

using namespace DbXml;
using namespace std;

static const int INDENT = 1;

UnionQP::UnionQP(QueryPlan *l, QueryPlan *r, u_int32_t flags, XPath2MemoryManager *mm)
	: OperationQP(QueryPlan::UNION, flags, mm)
{
	addArg(l);
	addArg(r);
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

QueryPlan *UnionQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	vector<QueryPlan*> newArgs;
	Vector::iterator it = args_.begin();
	if(it != args_.end()) {

		QueryPlan *arg = (*it)->staticTyping(context, styper);
		_src.copy(arg->getStaticAnalysis());
		newArgs.push_back(arg);

		unsigned int min = arg->getStaticAnalysis().getStaticType().getMin();

		for(++it; it != args_.end(); ++it) {
			arg = (*it)->staticTyping(context, styper);
			_src.add(arg->getStaticAnalysis());
			_src.getStaticType().typeConcat(arg->getStaticAnalysis().getStaticType());
			newArgs.push_back(arg);

			if(min > arg->getStaticAnalysis().getStaticType().getMin())
				min = arg->getStaticAnalysis().getStaticType().getMin();
		}

		_src.getStaticType().setCardinality(min, _src.getStaticType().getMax());
	}
	args_.clear();
	std::copy(newArgs.begin(), newArgs.end(), back_inserter(args_));

	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);

	return dissolve();
}

void UnionQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	Vector::iterator it = args_.begin();
	if(it != args_.end()) {

		(*it)->staticTypingLite(context);
		_src.copy((*it)->getStaticAnalysis());

		unsigned int min = (*it)->getStaticAnalysis().getStaticType().getMin();

		for(++it; it != args_.end(); ++it) {
			(*it)->staticTypingLite(context);
			_src.add((*it)->getStaticAnalysis());
			_src.getStaticType().typeConcat((*it)->getStaticAnalysis().getStaticType());

			if(min > (*it)->getStaticAnalysis().getStaticType().getMin())
				min = (*it)->getStaticAnalysis().getStaticType().getMin();
		}

		_src.getStaticType().setCardinality(min, _src.getStaticType().getMax());
	}

	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);
}

static string logUnionBefore(const QueryPlan *left, const QueryPlan *right)
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		ostringstream s;

		s << "u(";

		s << left->toString() << ",";
		s << right->toString();

		s << ")";

		return shorten(s.str(), MAX_NAME_LENGTH);
	}
	return "";
}

class keys_compare_more
{
public:
	keys_compare_more(OperationContext &context, QueryExecutionContext &qec)
		: context_(context), qec_(qec) {}

	bool operator()(const QueryPlan *l, const QueryPlan *r) const
	{
		Cost lc = l->cost(context_, qec_);
		Cost rc = r->cost(context_, qec_);
		return lc.keys > rc.keys || (lc.keys == rc.keys && lc.totalPages() < rc.totalPages());
	}

private:
	OperationContext &context_;
	QueryExecutionContext &qec_;
};

QueryPlan *UnionQP::optimize(OptimizationContext &opt)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Optimize the arguments
	vector<QueryPlan*> newArgs;
	Vector::iterator it;
	for(it = args_.begin(); it != args_.end(); ++it) {
		QueryPlan *arg = (*it)->optimize(opt);
		if(arg->getType() == type_) {
			// Merge UnionQP arguments into this object
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

	removeSubsets(opt);

	// Return if we only have one argument
	if(args_.size() == 1) return args_[0];

	for(Vector::const_iterator itc = args_.begin(); itc != args_.end(); ++itc) {
		for(Vector::const_iterator it2 = itc + 1; it2 != args_.end(); ++it2) {
			if((*itc)->getType() == (*it2)->getType() && StructuralJoinQP::isStructuralJoin((*itc)->getType())) {
				StructuralJoinQP *lsj = (StructuralJoinQP*)*itc;
				StructuralJoinQP *rsj = (StructuralJoinQP*)*it2;

				if(lsj->getRightArg()->isSubsetOf(rsj->getRightArg()) && rsj->getRightArg()->isSubsetOf(lsj->getRightArg())) {
					UnionQP *innerUnion = new (mm) UnionQP(lsj->getLeftArg()->copy(mm), rsj->getLeftArg()->copy(mm), 0, mm);
					innerUnion->setLocationInfo(this);

					QueryPlan *join = StructuralJoinQP::createJoin(StructuralJoinQP::getJoinType(lsj->getType()),
						innerUnion, lsj->getRightArg()->copy(mm), 0, lsj, mm);

					UnionQP *newUnion = new (mm) UnionQP(flags_, mm);
					newUnion->setLocationInfo(this);

					Vector::const_iterator it4;
					// Copy args before "itc"
					for(it4 = args_.begin(); it4 != itc; ++it4) {
						newUnion->addArg((*it4)->copy(mm));
					}
					// Add new alternative
					newUnion->addArg(join);
					// Copy args from after "itc" to before "it2"
					for(++it4; it4 != it2; ++it4) {
						newUnion->addArg((*it4)->copy(mm));
					}
					// Copy args from after "it2"
					for(++it4; it4 != args_.end(); ++it4) {
						newUnion->addArg((*it4)->copy(mm));
					}

					logTransformation(opt.getLog(), "Extract common structural join",
						this, newUnion);

					this->release();
					return newUnion->optimize(opt);
				}
			}
		}
	}

	return this;
}

void UnionQP::createCombinations(unsigned int maxA, OptimizationContext &opt, QueryPlans &combinations) const
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

void UnionQP::combineAltArgs(vector<QueryPlans>::iterator argIt, vector<QueryPlans>::iterator argEnd,
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
		// Construct the new UnionQP
		UnionQP *newUnion = new (mm) UnionQP(flags_, mm);
		newUnion->setLocationInfo(this);
		for(vector<QueryPlan*>::iterator it = newArgs.begin(); it != newArgs.end(); ++it) {
			newUnion->addArg((*it)->copy(mm));
		}

		// Add this UnionQP to the combinations vector
		combinations.push_back(newUnion);
	}
}

void UnionQP::removeSubsets(OptimizationContext &opt)
{
	// Remove subsets from the arguments
	Vector::iterator it;
	vector<QueryPlan*> newArgs;
	for(it = args_.begin(); it != args_.end(); ++it) {

		bool store = true;
		for(vector<QueryPlan*>::iterator it3 = newArgs.begin();
		    it3 != newArgs.end(); ++it3) {
			if((*it)->isSubsetOf(*it3)) {
				logTransformation(opt.getLog(), "Removed subset",
					logUnionBefore(*it, *it3), *it3);
				store = false;
				break;
			}
		}

		if(store) {
			Vector::iterator it2 = it;
			for(++it2; it2 != args_.end(); ++it2) {
				if((*it)->isSubsetOf(*it2)) {
					logTransformation(opt.getLog(), "Removed subset",
						logUnionBefore(*it, *it2), *it2);
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

void UnionQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	removeSubsets(opt);

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
	sort(args_.begin(), args_.end(), keys_compare_more(opt.getOperationContext(), qec));

	alternatives.push_back(this);
}

NodeIterator *UnionQP::createNodeIterator(DynamicContext *context) const
{
	AutoDelete<NodeIterator> result(0);
	Vector::const_iterator it = args_.begin();
	if(it != args_.end()) {
		result.set((*it)->createNodeIterator(context));
		for(++it; it != args_.end(); ++it) {
			result.swap(new UnionIterator(result,
					    (*it)->createNodeIterator(context), this));
		}
	}
	return result.adopt();
}

Cost UnionQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost result;
	Vector::const_iterator it = args_.begin();
	if(it != args_.end()) {
		result = (*it)->cost(context, qec);
		for(++it; it != args_.end(); ++it) {
			Cost childcost = (*it)->cost(context, qec);
			result.unionOp(childcost);
		}
	}
	return result;
}

bool UnionQP::isSubsetOf(const QueryPlan *o) const
{
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		if(!(*it)->isSubsetOf(o)) return false;
	}
	return true;
}

bool UnionQP::isSupersetOf(const QueryPlan *o) const
{
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		if(o->isSubsetOf(*it)) return true;
	}
	return false;
}

QueryPlan *UnionQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	UnionQP *result = new (mm) UnionQP(flags_, mm);
	result->setLocationInfo(this);

	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		result->addArg((*it)->copy(mm));
	}

	return result;
}

void UnionQP::release()
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

string UnionQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<UnionQP>" << endl;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		s << (*it2)->printQueryPlan(context, indent + INDENT);
	}
	s << in <<  "</UnionQP>" << endl;

	return s.str();
}

string UnionQP::toString(bool brief) const
{
	ostringstream s;

	s << "u(";

	bool addComma = false;
	for(Vector::const_iterator it2 = args_.begin(); it2 != args_.end(); ++it2) {
		if(addComma) { s << ","; } else { addComma = true; }
		s << (*it2)->toString(brief);
	}

	s << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UnionIterator::UnionIterator(NodeIterator *left, NodeIterator *right, const LocationInfo *o)
	: ProxyIterator(o),
	  left_(left),
	  right_(right),
	  next_(BOTH)
{
}

UnionIterator::~UnionIterator()
{
	delete left_;
	delete right_;
}

bool UnionIterator::next(DynamicContext *context)
{
	if(next_ & LEFT) {
		if(!left_->next(context)) {
			delete left_;
			left_ = 0;
		}
	}
	if(next_ & RIGHT) {
		if(!right_->next(context)) {
			delete right_;
			right_ = 0;
		}
	}
	return doJoin(context);
}

bool UnionIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if((next_ & LEFT) || (left_ && isSameNID(left_, container, did, nid) < 0)) {
		if(!left_->seek(container, did, nid, context)) {
			delete left_;
			left_ = 0;
		}
	}
	if((next_ & RIGHT) || (right_ && isSameNID(right_, container, did, nid) < 0)) {
		if(!right_->seek(container, did, nid, context)) {
			delete right_;
			right_ = 0;
		}
	}
	return doJoin(context);
}

bool UnionIterator::doJoin(DynamicContext *context)
{
	if(left_ == 0) {
		if(right_ == 0) {
			next_ = NEITHER;
			return false;
		}
		result_ = right_;
		next_ = RIGHT;
	}
	else if(right_ == 0) {
		result_ = left_;
		next_ = LEFT;
	}
	else {
		int cmp = compare(left_, right_);
		if(cmp < 0) {
			result_ = left_;
			next_ = LEFT;
		} else if(cmp > 0) {
			result_ = right_;
			next_ = RIGHT;
		} else {
			result_ = left_;
			next_ = BOTH;
		}
	}

	return true;
}

