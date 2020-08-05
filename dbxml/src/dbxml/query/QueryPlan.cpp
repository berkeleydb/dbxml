//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include <assert.h>
#include <string.h>
#include <sstream>
#include <set>
#include <algorithm>
#include "QueryPlan.hpp"
#include "QueryExecutionContext.hpp"
#include "UnionQP.hpp"
#include "IntersectQP.hpp"
#include "EmptyQP.hpp"
#include "DescendantJoinQP.hpp"
#include "SequentialScanQP.hpp"
#include "ValueFilterQP.hpp"
#include "../UTF8.hpp"
#include "../Name.hpp"
#include "../Container.hpp"
#include "../OperationContext.hpp"
#include "../QueryContext.hpp"
#include "../SyntaxManager.hpp"
#include "../Value.hpp"
#include "../Manager.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../dataItem/Join.hpp"
#include "../dataItem/DbXmlNodeImpl.hpp"
#include "../dataItem/DbXmlFactoryImpl.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../optimizer/QueryPlanOptimizer.hpp"
#include <dbxml/XmlNamespace.hpp>

#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

using namespace DbXml;
using namespace std;

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

static const int INDENT = 1;

static inline char *char_clone(const char *s, MemoryManager *mm) {
	return XMLString::replicate(s, mm);
}

static inline char *char_clone(const char *s, size_t len, MemoryManager *mm) {
	if(s == 0) return 0;
	char *result = (char*)mm->allocate(len * sizeof(char));
	::strncpy(result, s, len);
	return result;
}

static inline bool char_equals(const char *a, const char *b) {
	return XMLString::equals(a, b);
}

static inline bool char_equals(const char *a, size_t lena, const char *b, size_t lenb) {
	if(lena != lenb) return false;
	return (::strncmp(a, b, lena) == 0) ? true : false;
}

/////////////////////////////////
// OptimizationContext

XPath2MemoryManager *OptimizationContext::getMemoryManager() const
{
	return context_->getMemoryManager();
}

OperationContext &OptimizationContext::getOperationContext() const
{
	return GET_CONFIGURATION(context_)->getOperationContext();
}

Transaction *OptimizationContext::getTransaction() const
{
	return GET_CONFIGURATION(context_)->getTransaction();
}

const IndexSpecification &OptimizationContext::getIndexSpecification() const
{
	if(!isFetched_ && container_) {
		container_->getIndexSpecification(getTransaction(), is_);
		isFetched_ = true;
	}
	return is_;
}

const Log &OptimizationContext::getLog() const
{
	if(container_) return *container_;
	return (Manager&)GET_CONFIGURATION(context_)->getManager();
}

/////////////////////////////////
// QueryPlan

string DbXml::shorten(const string &name, unsigned int length) {
	string shortName = name;
	if(shortName.length() > length) {
		shortName = name.substr(0, length - 3) + "...";
	}

	return shortName;
}

void QueryPlan::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	combinations.push_back(copy(opt.getMemoryManager()));
}

void QueryPlan::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	alternatives.push_back(this);
}

void QueryPlan::createAlternatives(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives) const
{
	QueryPlans combinations;
	createCombinations(maxAlternatives, opt, combinations);

	// Generate the alternatives by applying conversion rules to the combinations
	for(QueryPlans::iterator it = combinations.begin(); it != combinations.end(); ++it) {
		(*it)->applyConversionRules(maxAlternatives, opt, alternatives);
	}
}

class ContainsSequentialScan : public NodeVisitingOptimizer
{
public:
	bool run(QueryPlan *qp)
	{
		found = false;
		optimizeQP(qp);
		return found;
	}

private:
	virtual void resetInternal() {}

	virtual ASTNode *optimize(ASTNode *item)
	{
		// Don't look inside ASTNode objects
		return item;
	}
	virtual QueryPlan *optimizeSequentialScan(SequentialScanQP *item)
	{
		found = true;
		return item;
	}

	bool found;
};

CostSortItem::CostSortItem(QueryPlan *qp, OperationContext &oc, QueryExecutionContext &qec, bool checkForSS)
	: qp_(qp), cost_(qp->cost(oc, qec)),
	  hasSS_(false)
{
	if(checkForSS) hasSS_ = ContainsSequentialScan().run(qp);
}

bool CostSortItem::operator<(const CostSortItem &o) const
{
        if(hasSS_ != o.hasSS_) return !hasSS_;
	
	if(cost_.totalPages() < o.cost_.totalPages()) return true;
	if(cost_.totalPages() > o.cost_.totalPages()) return false;

	if(cost_.pagesOverhead < o.cost_.pagesOverhead) return true;
	if(cost_.pagesOverhead > o.cost_.pagesOverhead) return false;

	return qp_ < o.qp_;
}

static const unsigned int ALTERNATIVES_THRESHOLD = 1;

void QueryPlan::createReducedAlternatives(double cutOffFactor, unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives) const
{
	unsigned int maxChildAlternatives = maxAlternatives;
	if(maxChildAlternatives > 2) maxChildAlternatives -= 2;
	else if(maxChildAlternatives > 1) maxChildAlternatives -= 1;

	QueryPlans combinations;
	createCombinations(maxChildAlternatives, opt, combinations);

	// TBD remove the need for QueryExecutionContext here - jpcs
	QueryExecutionContext qec(GET_CONFIGURATION(opt.getContext())->getQueryContext(),
		/*debugging*/false);
	qec.setContainerBase(opt.getContainerBase());
	qec.setDynamicContext(opt.getContext());

	OperationContext &oc = opt.getOperationContext();

	// Sort the alternatives by their cost
	set<CostSortItem> costSortSet;

	// Generate the alternatives by applying conversion rules to the combinations
	int alternativesCount = 0;
	for(QueryPlans::iterator it2 = combinations.begin(); it2 != combinations.end(); ++it2) {
		QueryPlans myAlts;
		(*it2)->applyConversionRules(maxChildAlternatives, opt, myAlts);

		for(QueryPlans::iterator it = myAlts.begin(); it != myAlts.end(); ++it) {

			// Check that the query plan isn't equal to one we already have (mutual subsets of each other)
			bool found = false;
			set<CostSortItem>::iterator i;
			for(i = costSortSet.begin(); i != costSortSet.end(); ++i) {
				if(i->qp_->isSubsetOf(*it) && (*it)->isSubsetOf(i->qp_)) {
					found = true;
					break;
				}
			}
			if(i != costSortSet.end()) {
				(*it)->release();
				continue;
			}

			++alternativesCount;
			costSortSet.insert(CostSortItem(*it, oc, qec, opt.checkForSS()));

			if(costSortSet.size() > ALTERNATIVES_THRESHOLD) {
				// Trim all QueryPlans outside of a factor of the cost of the lowest cost QueryPlan
				// TBD Make the specific factor configurable - jpcs
				set<CostSortItem>::iterator cutPoint = costSortSet.lower_bound(CostSortItem(costSortSet.begin()->cost_.totalPages() * cutOffFactor, false));
				if(cutPoint != costSortSet.begin() && cutPoint != costSortSet.end()) {
					for(i = cutPoint; i != costSortSet.end(); ++i) {
						if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
							ostringstream oss;
							oss << "Rejected Alternative (outside cut off factor: ";
							oss << (costSortSet.begin()->cost_.totalPages() * cutOffFactor);
							oss << ")";
							log(qec, oss.str());
							i->qp_->logCost(qec, const_cast<Cost&>(i->cost_), 0);
						}
						i->qp_->release();
					}
					costSortSet.erase(cutPoint, costSortSet.end());
				}
				DBXML_ASSERT(!costSortSet.empty());

				// Limit the number of alternatives returned to maxAlternatives
				while(costSortSet.size() > maxAlternatives) {
					i = costSortSet.end();
					--i;
					log(qec, "Rejected Alternative (exceeded maximum alternatives)");
					i->qp_->logCost(qec, const_cast<Cost&>(i->cost_), 0);
					i->qp_->release();
					costSortSet.erase(i);
				}
				DBXML_ASSERT(!costSortSet.empty());
			}
		}
	}

	DBXML_ASSERT(!costSortSet.empty());

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG) && alternativesCount != 1) {
		ostringstream oss2;
		oss2 << "Original query plan: " << logBefore(this);
		log(qec, oss2.str());

		ostringstream oss;
		oss << "Chosen " << (unsigned int) costSortSet.size() << " from " << alternativesCount << " alternatives";
		log(qec, oss.str());

		for(set<CostSortItem>::iterator i = costSortSet.begin(); i != costSortSet.end(); ++i) {
			i->qp_->logCost(qec, const_cast<Cost&>(i->cost_), 0);
			alternatives.push_back(i->qp_);
		}
	} else {
		for(set<CostSortItem>::iterator i = costSortSet.begin(); i != costSortSet.end(); ++i) {
			alternatives.push_back(i->qp_);
		}
	}
}

static bool betterAlternativeCost(const Cost &costA, bool ssA, const Cost &costB, bool ssB, bool checkForSS)
{
	if(ssA != ssB && checkForSS) return ssB;

	if(costA.totalPages() < costB.totalPages()) return true;
	if(costA.totalPages() > costB.totalPages()) return false;

	return costA.pagesOverhead < costB.pagesOverhead;
}

QueryPlan *QueryPlan::chooseAlternative(OptimizationContext &opt, const char *name) const
{
	QueryPlans combinations;
	createCombinations(MAX_ALTERNATIVES, opt, combinations);

	// TBD remove the need for QueryExecutionContext here - jpcs
	QueryExecutionContext qec(GET_CONFIGURATION(opt.getContext())->getQueryContext(),
		/*debugging*/false);
	qec.setContainerBase(opt.getContainerBase());
	qec.setDynamicContext(opt.getContext());

	QueryPlan *bestQP = 0;
	Cost bestCost;
	bool bestSS = false; // initial value doesn't matter; this
	                     // keeps compilers quiet

	// Generate the alternatives by applying conversion rules to the combinations
	int alternativesCount = 0;
	for(QueryPlans::iterator it2 = combinations.begin(); it2 != combinations.end(); ++it2) {
		QueryPlans myAlts;
		(*it2)->applyConversionRules(MAX_ALTERNATIVES, opt, myAlts);

		for(QueryPlans::iterator it = myAlts.begin(); it != myAlts.end(); ++it) {
			++alternativesCount;

			QueryPlan *qp = (*it);
			Cost itCost = qp->cost(opt.getOperationContext(), qec);
			bool itSS = ContainsSequentialScan().run(qp);

			if(bestQP == 0 || betterAlternativeCost(itCost, itSS, bestCost, bestSS, opt.checkForSS())) {
				if(bestQP != 0) {
					log(qec, "Rejected Alternative (not best)");
					bestQP->logCost(qec, bestCost, 0);
					bestQP->release();
				}
				bestQP = qp;
				bestCost = itCost;
				bestSS = itSS;
			} else {
				log(qec, "Rejected Alternative (not best)");
				qp->logCost(qec, itCost, 0);
				qp->release();
			}
		}
	}

	DBXML_ASSERT(bestQP != 0);

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		ostringstream oss2;
		oss2 << "Original query plan: " << logBefore(this);
		log(qec, oss2.str());

		ostringstream oss;
		oss << name << " alternative chosen (from " << alternativesCount << ")";
		log(qec, oss.str());

		bestQP->logCost(qec, bestCost, 0);
	}
	return bestQP;
}

void QueryPlan::logLegend(const Log &log) const
{
	static bool outputLegend = true;

	if(outputLegend && Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_INFO)) {
		outputLegend = false;
		ostringstream oss;
		oss << "\n"
		    << "\t Legend for the Query Plan log output\n"
		    << "\n"
		    << "\t RQP  - Raw Query Plan before any optimizations\n"
		    << "\t POQP - Partially optimized Query Plan\n"
		    << "\t OQP  - Optimized Query Plan after optimizations\n"
		    << "\n"
		    << "\t path - Paths\n"
		    << "\n"
		    << "\t P    - Presence index look up\n"
		    << "\t V    - Value index look up\n"
		    << "\t R    - Range index look up\n"
		    << "\t Pd   - Presence document index look up\n"
		    << "\t Vd   - Value document index look up\n"
		    << "\t Rd   - Range document index look up\n"
		    << "\t SS   - Sequential scan\n"
		    << "\t U    - Universal set\n"
		    << "\t E    - Empty set\n"
		    << "\n"
		    << "\t COL  - Collection function\n"
		    << "\t DOC  - Document function\n"
		    << "\t CN   - Context node\n"
		    << "\t VAR  - Variable\n"
		    << "\t AST  - Non query plan operation\n"
		    << "\n"
		    << "\t VF   - Value filter\n"
		    << "\t PF   - Predicate filter\n"
		    << "\t NPF  - Node predicate filter\n"
		    << "\t NNPF - Negative node predicate filter\n"
		    << "\t NuPF - Numeric predicate filter\n"
		    << "\t RNPF - Reverse numeric predicate filter\n"
		    << "\t LF   - Level filter\n"
		    << "\n"
		    << "\t DP   - Optimization decision point\n"
		    << "\t DPE  - Decision point end\n"
		    << "\t BUF  - Buffer\n"
		    << "\t BR   - Buffer reference\n"
		    << "\n"
		    << "\t n    - Intersection\n"
		    << "\t u    - Union\n"
		    << "\t e    - Except\n"
		    << "\n"
		    << "\t step - Conventional navigation step\n"
		    << "\n"
		    << "\t d    - Descendant join\n"
		    << "\t ds   - Descendant or self join\n"
		    << "\t c    - Child join\n"
		    << "\t ca   - Attribute or child join\n"
		    << "\t a    - Attribute join\n"
		    << "\t p    - Parent join\n"
		    << "\t pa   - Parent of attribute join\n"
		    << "\t pc   - Parent of child join\n"
		    << "\t an   - Ancestor join\n"
		    << "\t ans  - Ancestor or self join\n"
		    << "\n";
		log.log(Log::C_OPTIMIZER, Log::L_INFO, oss);
	}
}

void QueryPlan::logQP(const Log &log, const string &title, const QueryPlan *qp, OptimizationContext::Phase phase) const
{
	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_INFO)) {
		ostringstream oss;

		oss << title;

		if((int)phase != -1) {
			oss << "(" << (int)phase << ")";
		}

		oss << ": ";
		if(qp) oss << qp->toString(false);
		else oss << "none";

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_INFO, oss);
	}
}

void QueryPlan::log(QueryExecutionContext &qec, const string &message)
{
	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		const Log *log = qec.getContainerBase() ? (Log*)qec.getContainerBase() : (Log*)(Manager*)qec.getContext().getManager();
		log->log(Log::C_OPTIMIZER, Log::L_DEBUG, message);
	}
}

void QueryPlan::logCost(QueryExecutionContext &qec, Cost &cost, const KeyStatistics *stats) const
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	string name = toString();
	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string shortName = shorten(name, MAX_NAME_LENGTH);

		ostringstream oss;
		oss << shortName;
		oss << " : keys=" << cost.keys;
		oss << ", overhead=" << (cost.pagesOverhead / 1024) << "k";
		oss << ", forKeys=" << (cost.pagesForKeys / 1024) << "k";
		if(stats != 0) oss << " : stats(" << stats->asString() << ")";

		const Log *log = qec.getContainerBase() ? (Log*)qec.getContainerBase() : (Log*)(Manager*)qec.getContext().getManager();

		logLegend(*log);
		log->log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

void QueryPlan::logCost(QueryExecutionContext &qec, const string &name, Cost &cost) const
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string shortName = shorten(name, MAX_NAME_LENGTH);

		ostringstream oss;
		oss << shortName;
		oss << " : keys=" << cost.keys;
		oss << ", overhead=" << (cost.pagesOverhead / 1024) << "k";
		oss << ", forKeys=" << (cost.pagesForKeys / 1024) << "k";

		const Log *log = qec.getContainer() ? (Log*)qec.getContainer() : (Log*)(Manager*)qec.getContext().getManager();

		logLegend(*log);
		log->log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

void QueryPlan::logCost(QueryExecutionContext &qec, const string &name, double pages) const
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string shortName = shorten(name, MAX_NAME_LENGTH);

		ostringstream oss;
		oss << shortName;
		oss << " : pages=" << pages;

		const Log *log = qec.getContainerBase() ? (Log*)qec.getContainerBase() : (Log*)(Manager*)qec.getContext().getManager();

		logLegend(*log);
		log->log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

void QueryPlan::logTransformation(const Log &log, const QueryPlan *transformed) const
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string start_name = shorten(toString(), MAX_NAME_LENGTH);
		string end_name = shorten(transformed->toString(), MAX_NAME_LENGTH);

		ostringstream oss;
		oss << start_name << " -> " << end_name;

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

string QueryPlan::logBefore(const QueryPlan *before) const
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		return shorten(before->toString(), MAX_NAME_LENGTH);
	}
	return "";
}

void QueryPlan::logTransformation(const Log &log, const string &transform, const string &before,
	const QueryPlan *result) const
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string result_name = shorten(result->toString(), MAX_NAME_LENGTH);

		ostringstream oss;

		oss << transform << ": ";
		oss << before << " -> " << result_name;

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

void QueryPlan::logTransformation(const Log &log, const string &transform, const QueryPlan *before,
	const string &result) const
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string before_name = shorten(before->toString(), MAX_NAME_LENGTH);

		ostringstream oss;

		oss << transform << ": ";
		oss << before_name << " -> " << result;

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

void QueryPlan::logTransformation(const Log &log, const string &transform, const QueryPlan *before,
	const QueryPlan *result) const
{
	static const unsigned int MAX_NAME_LENGTH = 500;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string before_name = shorten(before->toString(), MAX_NAME_LENGTH);
		string result_name = shorten(result->toString(), MAX_NAME_LENGTH);

		ostringstream oss;

		oss << transform << ": ";
		oss << before_name << " -> " << result_name;

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

/////////////////////////////////
// OperationQP

QueryPlan *OperationQP::addArg(QueryPlan *o)
{
	if(o) {
		if(type_ == o->getType()) {
			OperationQP *op = (OperationQP*)o;
			args_.insert(args_.end(), op->args_.begin(), op->args_.end());
			flags_ |= op->flags_;
		}
		else {
			args_.push_back(o);
			return o;
		}
	}

	return this;
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

QueryPlan *OperationQP::compress()
{
	// Remove duplicate arguments
	UniqueArgs uniqueArgs;
	for(Vector::iterator it = args_.begin(); it != args_.end(); ++it) {
		uniqueArgs.add(type_, *it);
	}
	args_.clear();
	std::copy(uniqueArgs.begin(), uniqueArgs.end(), back_inserter(args_));

	return dissolve();
}

QueryPlan *OperationQP::dissolve()
{
	// Dissolve if we have none of only one argument
	if(args_.empty()) return 0;
	if(args_.size() == 1) return args_[0];
	return this;
}

void OperationQP::findQueryPlanRoots(QPRSet &qprset) const
{
	for(Vector::const_iterator it = args_.begin(); it != args_.end(); ++it) {
		(*it)->findQueryPlanRoots(qprset);
	}
}

/////////////////////////////////
// PathsQP

PathsQP::PathsQP(const Paths &paths, XPath2MemoryManager *mm)
	: QueryPlan(QueryPlan::PATHS, 0, mm),
	  paths_(XQillaAllocator<ImpliedSchemaNode*>(mm))
{
	paths_.insert(paths_.end(), paths.begin(), paths.end());
}

PathsQP::PathsQP(const ImpliedSchemaNode::Vector &paths, XPath2MemoryManager *mm)
	: QueryPlan(QueryPlan::PATHS, 0, mm),
	  paths_(XQillaAllocator<ImpliedSchemaNode*>(mm))
{
	paths_.insert(paths_.end(), paths.begin(), paths.end());
}

void PathsQP::addPaths(const Paths &o)
{
	Paths::const_iterator it = o.begin();
	Paths::const_iterator end = o.end();
	for(; it != end; ++it) {
		paths_.push_back(*it);
	}
}

QueryPlan *PathsQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	PathsQP *result = new (mm) PathsQP(paths_, mm);
	result->setLocationInfo(this);

	return result;
}

void PathsQP::release()
{
#if defined(_MSC_VER) && (_MSC_VER < 1300)
        paths_.~vector<ImpliedSchemaNode*,XQillaAllocator<ImpliedSchemaNode *> >();
#else
	paths_.~Paths();
#endif
	_src.clear();
	memMgr_->deallocate(this);
}

string PathsQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<PathsQP>";
	bool addComma = false;
	for(Paths::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		if(addComma) { s << ","; } else { addComma = true; }
		s << (*it)->getStepName();
	}
	s << "</PathsQP>" << endl;

	return s.str();
}

string PathsQP::toString(bool brief) const
{
	ostringstream s;

	s << "path(";

	bool addComma = false;
	for(Paths::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		if(addComma) { s << ","; } else { addComma = true; }
		s << "\"" << (*it)->getStepName() << "\"";
	}

	s << ")";
	return s.str();
}

QueryPlan *PathsQP::dissolve()
{
	if(paths_.empty()) return 0;
	return this;
}

void PathsQP::findQueryPlanRoots(QPRSet &qprset) const
{
	for(Paths::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		qprset.insert(((ImpliedSchemaNode*)(*it)->getRoot())->getQueryPlanRoot());
	}
}

static QueryPlan *createSS(ImpliedSchemaNode *isn, const LocationInfo *location, XPath2MemoryManager *mm)
{
	QueryPlan *result = 0;

	switch((ImpliedSchemaNode::Type)isn->getType()) {
	case ImpliedSchemaNode::ROOT:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::DESCENDANT_ATTR:
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::METADATA:
		result = new (mm) SequentialScanQP(isn, 0, mm);
		break;
	case ImpliedSchemaNode::LTX:
	case ImpliedSchemaNode::LTE:
	case ImpliedSchemaNode::GTX:
	case ImpliedSchemaNode::GTE:
	case ImpliedSchemaNode::EQUALS:
	case ImpliedSchemaNode::NOT_EQUALS:
	case ImpliedSchemaNode::PREFIX:
		result = new (mm) ValueFilterQP(createSS((ImpliedSchemaNode*)isn->getParent(), location, mm), isn, 0, mm);
		break;
	case ImpliedSchemaNode::CAST:
		// Skip a cast
		result = createSS((ImpliedSchemaNode*)isn->getParent(), location, mm);
		break;
	case ImpliedSchemaNode::SUFFIX:
	case ImpliedSchemaNode::SUBSTRING:
	case ImpliedSchemaNode::SUBSTRING_CD:
		// A substring value filter is always added, so we don't
		// need another one.
		result = createSS((ImpliedSchemaNode*)isn->getParent(), location, mm);
		break;
	}

	result->setLocationInfo(location);
	return result;
}

QueryPlan *PathsQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	for(Paths::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		if((*it)->getASTNode()) {
			(*it)->setASTNode(const_cast<ASTNode*>((*it)->getASTNode())->staticTyping(context, styper));
			_src.add((*it)->getASTNode()->getStaticAnalysis());
		}
	}

	_src.getStaticType() = StaticType(StaticType::NODE_TYPE, 0, StaticType::UNLIMITED);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);

	return this;
}

void PathsQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	for(Paths::const_iterator it = paths_.begin(); it != paths_.end(); ++it) {
		if((*it)->getASTNode())
			_src.add((*it)->getASTNode()->getStaticAnalysis());
	}

	_src.getStaticType() = StaticType(StaticType::NODE_TYPE, 0, StaticType::UNLIMITED);
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE);
}

QueryPlan *PathsQP::optimize(OptimizationContext &opt)
{
	UnionQP *unionOp = new (memMgr_) UnionQP(0, memMgr_);
	unionOp->setLocationInfo(this);
	for(Paths::iterator it = paths_.begin(); it != paths_.end(); ++it) {
		QueryPlan *arg = createStep(*it, this, memMgr_);

		if(arg == 0) {
			arg = createSS(*it, this, memMgr_);
		}
		if((*it)->getType() == (QueryPathNode::Type)ImpliedSchemaNode::SUBSTRING ||
			(*it)->getType() == (QueryPathNode::Type)ImpliedSchemaNode::SUBSTRING_CD ||
			(*it)->getType() == (QueryPathNode::Type)ImpliedSchemaNode::SUFFIX) {
			// Add in the value filter that we're going to need
			arg = new (memMgr_) ValueFilterQP(arg, *it, 0, memMgr_);
			arg->setLocationInfo(this);
		}

		unionOp->addArg(arg);
	}

	logTransformation(opt.getLog(), unionOp);
	return unionOp->optimize(opt);
}

QueryPlan *PathsQP::createStep(ImpliedSchemaNode *child, const LocationInfo *location, XPath2MemoryManager *mm)
{
	switch((ImpliedSchemaNode::Type)child->getType()) {
	case ImpliedSchemaNode::ROOT:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::DESCENDANT_ATTR:
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::METADATA: {
		const char *parentName, *childName;
		if(!getStepNames(child, parentName, childName, mm)) {
			return 0;
		}

		ImpliedSchemaNode::Type type = ImpliedSchemaNode::CHILD;
		switch(child->getType()) {
		case ImpliedSchemaNode::DESCENDANT_ATTR:
		case ImpliedSchemaNode::ATTRIBUTE: type = ImpliedSchemaNode::ATTRIBUTE; break;
		case ImpliedSchemaNode::METADATA: type = ImpliedSchemaNode::METADATA; break;
		default: break;
		}

		QueryPlan *result = new (mm) PresenceQP(type, parentName, childName, /*documentIndex*/false, child, 0, mm);
		result->setLocationInfo(location);
		return result;
	}
	case ImpliedSchemaNode::CAST:
		DBXML_ASSERT(false);
		break;
	case ImpliedSchemaNode::LTX:
	case ImpliedSchemaNode::LTE:
	case ImpliedSchemaNode::GTX:
	case ImpliedSchemaNode::GTE:
	case ImpliedSchemaNode::EQUALS:
	case ImpliedSchemaNode::NOT_EQUALS:
	case ImpliedSchemaNode::PREFIX:
	case ImpliedSchemaNode::SUFFIX:
	case ImpliedSchemaNode::SUBSTRING_CD:
	case ImpliedSchemaNode::SUBSTRING: {
		DbWrapper::Operation op = DbWrapper::NONE;
		switch((ImpliedSchemaNode::Type)child->getType()) {
		case ImpliedSchemaNode::LTX: op = DbWrapper::LTX; break;
		case ImpliedSchemaNode::LTE: op = DbWrapper::LTE; break;
		case ImpliedSchemaNode::GTX: op = DbWrapper::GTX; break;
		case ImpliedSchemaNode::GTE: op = DbWrapper::GTE; break;
		case ImpliedSchemaNode::EQUALS: op = DbWrapper::EQUALITY; break;
		case ImpliedSchemaNode::NOT_EQUALS: op = DbWrapper::NEG_NOT_EQUALITY; break;
		case ImpliedSchemaNode::PREFIX: op = DbWrapper::PREFIX; break;
		case ImpliedSchemaNode::SUFFIX:
		case ImpliedSchemaNode::SUBSTRING_CD:
		case ImpliedSchemaNode::SUBSTRING: op = DbWrapper::SUBSTRING; break;
		default: break;
		}

		const ImpliedSchemaNode *parent = child->getBaseNode();

		const char *parentName, *childName;
		if(!getStepNames(parent, parentName, childName, mm)) {
			return 0;
		}

		ImpliedSchemaNode::Type type = ImpliedSchemaNode::CHILD;
		switch(parent->getType()) {
		case ImpliedSchemaNode::ATTRIBUTE: type = ImpliedSchemaNode::ATTRIBUTE; break;
		case ImpliedSchemaNode::METADATA: type = ImpliedSchemaNode::METADATA; break;
		default: break;
		}

		QueryPlan *result = new (mm) ValueQP(type, parentName, childName, /*documentIndex*/false, op, child->getGeneralComp(),
			child->getSyntaxType(), child->getASTNode(), child, 0, mm);
		result->setLocationInfo(location);
		return result;
	}
	}

	return 0;
}

/*
 * parent        child         result
 * -------------------------------------------------------
 *
 * CHILD(x)      CHILD(y)       =  P(x.y)
 * DESCENDANT(x) CHILD(y)       =  P(x.y)
 * ROOT          CHILD(x)       =  P(#.x)
 * ...           CHILD(x)       =  P(x)
 * ...           CHILD(*)       =  U
 *
 * CHILD(x)      ATTRIBUTE(y)   =  P(x.y)
 * DESCENDANT(x) ATTRIBUTE(y)   =  P(x.y)
 * ROOT          ATTRIBUTE(x)   =  P(#.x)
 * ...           ATTRIBUTE(x)   =  P(x)
 * ...           ATTRIBUTE(*)   =  U
 *
 * ...           DESCENDANT(x)  =  P(x)
 * ...           DESCENDANT(*)  =  U
 *
 *               ROOT           =  U
 *
 * Returns true if the step should be a U
 */
bool PathsQP::getStepNames(const ImpliedSchemaNode *child, const char *&parentName, const char *&childName, XPath2MemoryManager *mm)
{
	const ImpliedSchemaNode *parent = (ImpliedSchemaNode*)child->getParent();

	parentName = 0;
	childName = 0;

	if(child->isWildcard()) {
		return false;
	}

	switch(child->getType()) {
	case ImpliedSchemaNode::ROOT: {
		return false;
	}
	case ImpliedSchemaNode::METADATA:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::DESCENDANT_ATTR: {
		childName = child->getUriName();
		break;
	}
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD: {
		childName = child->getUriName();

		if(parent->getType() == QueryPathNode::ROOT) {
			parentName = char_clone(Name::dbxml_colon_root.getURIName().c_str(), mm);
		}
		else if(!parent->isWildcard()) {
			parentName = parent->getUriName();
		}

		break;
	}
	default: break;
	}

	return true;
}

/////////////////////////////////
// IndexLookups

namespace DbXml {
class IndexLookups
{
public:
	IndexLookups(bool intersect = false)
		: intersect_(intersect), op_(DbWrapper::NONE), key_(0) {}

	void add(DbWrapper::Operation op, const Key &key)
	{
		values_.push_back(IndexLookups(op, key));
	}

	void add(const IndexLookups &okv)
	{
		values_.push_back(okv);
	}

	NodeIterator *createNodeIterator(const PresenceQP *pqp, DynamicContext *context)
	{
		AutoDelete<NodeIterator> result(0);
		if(op_ != DbWrapper::NONE) {
			result.set(pqp->lookupNodeIterator(op_, key_, context));
		}
		else {
			std::vector<IndexLookups>::iterator i = values_.begin();
			if(i != values_.end()) {
				result.set(i->createNodeIterator(pqp, context));
				for(++i; i != values_.end(); ++i) {
					if(intersect_) {
						result.swap(new IntersectIterator(result,
								    i->createNodeIterator(pqp, context), pqp));
					} else {
						result.swap(new UnionIterator(result,
								    i->createNodeIterator(pqp, context), pqp));
					}
				}
			}
		}

		return result.adopt();
	}

	Cost cost(const PresenceQP *pqp, OperationContext &context, QueryExecutionContext &qec)
	{
		Cost result;
		if(op_ != DbWrapper::NONE) {
			result = pqp->lookupCost(op_, key_, context, qec);
		}
		else {
			std::vector<IndexLookups>::iterator i = values_.begin();
			if(i != values_.end()) {
				result = i->cost(pqp, context, qec);
				for(++i; i != values_.end(); ++i) {
					if(intersect_) {
						result.intersectOp(i->cost(pqp, context, qec));
					}
					else {
						result.unionOp(i->cost(pqp, context, qec));
					}
				}
			}
		}

		return result;
	}

private:
	IndexLookups(DbWrapper::Operation op, const Key &key)
		: intersect_(false), op_(op), key_(key) {}

	bool intersect_;
	std::vector<IndexLookups> values_;
	DbWrapper::Operation op_;
	Key key_;
};
}

/////////////////////////////////
// PresenceQP

PresenceQP::PresenceQP(ImpliedSchemaNode::Type type, const char *parent, const char *child, bool documentIndex,
	ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(QueryPlan::PRESENCE, flags, mm),
	  isn_(isn),
	  nodeType_(type),
	  parentUriName_(parent),
	  childUriName_(child),
	  documentIndex_(documentIndex),
	  container_(0),
	  key_(),
	  operation_(DbWrapper::NONE),
	  cost_(),
	  costSet_(false)
{
	DBXML_ASSERT(isn);
}

PresenceQP::PresenceQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
	const char *parent, const char *child, bool documentIndex,
	const QPKey &key, DbWrapper::Operation operation,
	ImpliedSchemaNode *isn, ContainerBase *cont,
	u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(qpType, flags, mm),
	  isn_(isn),
	  nodeType_(type),
	  parentUriName_(parent),
	  childUriName_(child),
	  documentIndex_(documentIndex),
	  container_(cont),
	  key_(key),
	  operation_(operation),
	  cost_(),
	  costSet_(false)
{
	DBXML_ASSERT(isn);
}

void PresenceQP::logIndexUse(const Log &log, const Index &index, const DbWrapper::Operation op) const
{
	static const unsigned int MAX_NAME_LENGTH = 80;

	if(Log::isLogEnabled(Log::C_OPTIMIZER, Log::L_DEBUG)) {
		string qp_name = shorten(toString(), MAX_NAME_LENGTH);

		ostringstream oss;

		oss << qp_name << " using: ";
		oss << index.asString() << ",";
		oss << DbWrapper::operationToString(op);

		logLegend(log);
		log.log(Log::C_OPTIMIZER, Log::L_DEBUG, oss);
	}
}

NodeIterator *PresenceQP::lookupNodeIterator(DbWrapper::Operation operation, Key &key, DynamicContext *context) const
{
	OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();

	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(oc, *container_, parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}

	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		return new EmptyIterator(this);
	} else {
		return container_->createIndexIterator(key.getSyntaxType(),
			context, documentIndex_, this, operation, key);
	}
}

Cost PresenceQP::lookupCost(DbWrapper::Operation operation, Key &key,
	OperationContext &context, QueryExecutionContext &qec) const
{
	if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
		key.setIDsFromNames(context, *container_, parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key.getID1());
		const_cast<QPKey&>(key_).setID2(key.getID2());
	}
	return container_->getIndexCost(context, operation, key);
}

void PresenceQP::getKeys(IndexLookups &keys, DynamicContext *context) const
{
	int timezone = ((Manager&)GET_CONFIGURATION(context)->getManager()).getImplicitTimezone();
	keys.add(operation_, key_.createKey(timezone));
}

void PresenceQP::getKeysForCost(IndexLookups &keys, DynamicContext *context) const
{
	int timezone = ((Manager&)GET_CONFIGURATION(context)->getManager()).getImplicitTimezone();
	keys.add(operation_, key_.createKey(timezone));
}

QueryPlan *PresenceQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	staticTypingLite(context);
	return this;
}

void PresenceQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	if(documentIndex_ || nodeType_ == ImpliedSchemaNode::METADATA) {
		_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE);
	} else if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		_src.getStaticType() = StaticType(StaticType::ATTRIBUTE_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::SUBTREE);
	} else {
		_src.getStaticType() = StaticType(StaticType::ELEMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::SUBTREE);
	}

	if(isParentSet() && char_equals(parentUriName_, metaDataName_uri_root)) {
		_src.setProperties(_src.getProperties() | StaticAnalysis::PEER);
	}
}

QueryPlan *PresenceQP::optimize(OptimizationContext &opt)
{
	if(container_ == 0) container_ = opt.getContainerBase();

	if(opt.getContainerBase() && !indexesResolved()) {
		if(!resolveIndexes(*opt.getContainerBase(), opt.getIndexSpecification(), /*useSubstring*/false)) {
			if(isParentSet()) {
				// else convert to node-*-presence-* and try again
				PresenceQP *pres = new (memMgr_) PresenceQP(nodeType_, 0, childUriName_, documentIndex_, isn_, flags_, memMgr_);
				pres->setLocationInfo(this);
				logTransformation(opt.getLog(), pres);
				return pres->optimize(opt);
			}
			else if(documentIndex_) {
				QueryPlan *result = createSS((ImpliedSchemaNode*)isn_->getRoot(), this, memMgr_);
				logTransformation(opt.getLog(), result);
				return result->optimize(opt);
			}
			else {
				QueryPlan *result = createSS(isn_, this, memMgr_);
				logTransformation(opt.getLog(), result);
				return result->optimize(opt);
			}
		}
		// Convert document indexes to node indexes -
		// only done just after we successfully resolve the index to use
		if(!container_->nodesIndexed() && !documentIndex_ && nodeType_ != ImpliedSchemaNode::METADATA) {
			documentIndex_ = true;

			QueryPlan *result = createSS(isn_, this, memMgr_);
			result = new (memMgr_) DescendantOrSelfJoinQP(this, result, 0, memMgr_);
			result->setLocationInfo(this);
			logTransformation(opt.getLog(), result);
			return result->optimize(opt);
		}
		// Add extra document indexes for every node index if
		// this is a DLS+ container
		if(container_->nodesIndexed() && !documentIndex_ && nodeType_ != ImpliedSchemaNode::METADATA &&
			container_->getContainer()->isWholedocContainer() ) {

			QueryPlan *result = copy();
			documentIndex_ = true;
			result = new (memMgr_) DescendantOrSelfJoinQP(this, result, 0, memMgr_);
			result->setLocationInfo(this);
			logTransformation(opt.getLog(), result);
			return result->optimize(opt);
		}
	}
	return this;
}

QueryPlan *PresenceQP::simpleLookupOptimize(OptimizationContext &opt)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	if(container_ == 0) container_ = opt.getContainerBase();

	if(opt.getContainerBase() && !indexesResolved()) {
		if(!resolveIndexes(*opt.getContainerBase(), opt.getIndexSpecification(), /*useSubstring*/true)) {
			return new (mm) EmptyQP(0, mm);
		}
	}
	return this;
}

NodeIterator *PresenceQP::createNodeIterator(DynamicContext *context) const
{
	DBXML_ASSERT(indexesResolved());

	IndexLookups keys;
	getKeys(keys, context);

	NodeIterator *result = keys.createNodeIterator(this, context);
	if(result != 0) return result;

	return new EmptyIterator(this);
}

Cost PresenceQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	if(!costSet_ && indexesResolved()) {
		IndexLookups keys;
		getKeysForCost(keys, qec.getDynamicContext());

		qec.setContainerBase(container_);

		cost_ = keys.cost(this, context, qec);
		costSet_ = true;
	}
	return cost_;
}

StructuralStats PresenceQP::getStructuralStats(OperationContext &oc, StructuralStatsCache &cache, const NameID &nameID, bool ancestor) const
{
	if(container_ == 0) return StructuralStats(nodeType_ == ImpliedSchemaNode::CHILD, nameID != 0);

	NameID id;
	if(documentIndex_ || nodeType_ == ImpliedSchemaNode::METADATA) {
		id = container_->getNIDForRoot();
	} else if(nodeType_ == ImpliedSchemaNode::CHILD) {
		if(key_.getID1() == 0) {
			container_->lookupID(oc, childUriName_, ::strlen(childUriName_),
				const_cast<NameID&>(key_.getID1()));
		}
		if(isParentSet() && key_.getID2() == 0) {
			container_->lookupID(oc, parentUriName_, ::strlen(parentUriName_),
				const_cast<NameID&>(key_.getID2()));
		}
		id = key_.getID1();
	}

	if(ancestor)
		return cache.get(container_, oc, nameID, id);
	return cache.get(container_, oc, id, nameID);
}

inline Index::Type nodeTypeToIndexType(ImpliedSchemaNode::Type t) {
	switch(t) {
	case ImpliedSchemaNode::ATTRIBUTE: return Index::NODE_ATTRIBUTE;
	case ImpliedSchemaNode::METADATA: return Index::NODE_METADATA;
	case ImpliedSchemaNode::CHILD: return Index::NODE_ELEMENT;
	default: break;
	}
	return (Index::Type)-1;
}

bool PresenceQP::indexesResolved() const
{
	return key_.getIndex().getNode() != Index::NODE_NONE;
}

// Order of preference for indexes
//
// edge-*-presence-none (equality)
// edge-*-equality-* (prefix)
// else convert to n(node-*-presence-*, node-*-presence) and try again
// ---- or ----
// node-*-presence-none (equality)
// edge-*-presence-none (prefix)
// node-*-equality-* (prefix)
// edge-*-equality-* (prefix)
//
bool PresenceQP::resolveIndexes(const ContainerBase &container, const IndexSpecification &is, bool useSubstring)
{
	Index &index = key_.getIndex();

	if(index.getNode() != Index::NODE_NONE) return true;

	container_ = (ContainerBase*)&container;

	const IndexVector *iv = is.getIndexOrDefault(childUriName_);

	if(iv) {
		index.set(nodeTypeToIndexType(nodeType_), Index::NODE_MASK);

		if(isParentSet()) {
			// edge-*-presence-none (equality)
			index.set(Index::PATH_EDGE | Index::KEY_PRESENCE | Index::SYNTAX_NONE, Index::PKS_MASK);
			operation_ = DbWrapper::EQUALITY;
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				logIndexUse(container, key_.getIndex(), operation_);
				return true;
			}

			// edge-*-equality-* (prefix)
			index.set(Index::KEY_EQUALITY, Index::KEY_MASK);
			operation_ = DbWrapper::PREFIX;
			Index prefix_index = iv->getIndex(index, Index::PNK_MASK);
			if(prefix_index != 0) {
				index = prefix_index;
				logIndexUse(container, key_.getIndex(), operation_);
				return true;
			}

			// Don't use substring indexes for presence lookups,
			// as they are too slow.
			if(useSubstring) {
				// edge-*-substring-*
				index.set(Index::KEY_SUBSTRING, Index::KEY_MASK);
				operation_ = DbWrapper::PREFIX;
				prefix_index = iv->getIndex(index, Index::PNK_MASK);
				if(prefix_index != 0) {
					index = prefix_index;
					logIndexUse(container, key_.getIndex(), operation_);
					return true;
				}
			}
		}
		else { // !isParentSet()

			// node-*-presence-none (equality)
			index.set(Index::PATH_NODE | Index::KEY_PRESENCE | Index::SYNTAX_NONE, Index::PKS_MASK);
			operation_ = DbWrapper::EQUALITY;
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				logIndexUse(container, key_.getIndex(), operation_);
				return true;
			}

			// edge-*-presence-none (prefix)
			index.set(Index::PATH_EDGE | Index::KEY_PRESENCE | Index::SYNTAX_NONE, Index::PKS_MASK);
			operation_ = DbWrapper::PREFIX;
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				key_.setNodeLookup(true);
				logIndexUse(container, key_.getIndex(), operation_);
				return true;
			}

			// node-*-equality-* (prefix)
			index.set(Index::PATH_NODE | Index::KEY_EQUALITY, Index::PK_MASK);
			Index prefix_index = iv->getIndex(index, Index::PNK_MASK);
			if(prefix_index != 0) {
				index = prefix_index;
				logIndexUse(container, key_.getIndex(), operation_);
				return true;
			}

			// edge-*-equality-* (prefix)
			index.set(Index::PATH_EDGE | Index::KEY_EQUALITY, Index::PK_MASK);
			prefix_index = iv->getIndex(index, Index::PNK_MASK);
			if(prefix_index != 0) {
				index = prefix_index;
				key_.setNodeLookup(true);
				logIndexUse(container, key_.getIndex(), operation_);
				return true;
			}

			// Don't use substring indexes for presence lookups,
			// as they are too slow.
			if(useSubstring) {
				// node-*-substring-*
				index.set(Index::PATH_NODE | Index::KEY_SUBSTRING, Index::PK_MASK);
				prefix_index = iv->getIndex(index, Index::PNK_MASK);
				if(prefix_index != 0) {
					index = prefix_index;
					logIndexUse(container, key_.getIndex(), operation_);
					return true;
				}

				// edge-*-substring-*
				index.set(Index::PATH_EDGE | Index::KEY_SUBSTRING, Index::PK_MASK);
				prefix_index = iv->getIndex(index, Index::PNK_MASK);
				if(prefix_index != 0) {
					index = prefix_index;
					key_.setNodeLookup(true);
					logIndexUse(container, key_.getIndex(), operation_);
					return true;
				}
			}
		}
	}

	index.set(Index::NODE_NONE, Index::NODE_MASK);
	return false;
}

void PresenceQP::findQueryPlanRoots(QPRSet &qprset) const
{
	qprset.insert(((ImpliedSchemaNode*)isn_->getRoot())->getQueryPlanRoot());
}

ImpliedSchemaNode::Type PresenceQP::getReturnType() const
{
	if(documentIndex_ || (container_ && !container_->nodesIndexed()))
		return ImpliedSchemaNode::METADATA;

	return nodeType_;
}

bool PresenceQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::PRESENCE) {
		const PresenceQP *step = (const PresenceQP*)o;

		if(container_ != step->getContainerBase()) return false;

		bool thisReturnsDocs = getReturnType() == ImpliedSchemaNode::METADATA;
		bool stepReturnsDocs = step->getReturnType() == ImpliedSchemaNode::METADATA;
		if(thisReturnsDocs != stepReturnsDocs) return false;

		if(char_equals(step->childUriName_, childUriName_) &&
		   step->nodeType_ == nodeType_) {

			// x-y-presence <isSubsetOf> x-y-presence
			if(isParentSet() && step->isParentSet() &&
			   char_equals(parentUriName_, step->parentUriName_)) {
				return true;
			}

			// *-x-presence <isSubsetOf> node-x-presence
			if(!step->isParentSet()) {
				return true;
			}
		}
	}
	else if(o->getType() == QueryPlan::UNION ||
		o->getType() == QueryPlan::INTERSECT) {
		return ((const OperationQP*)o)->isSupersetOf(this);
	}
	else if(o->getType() == QueryPlan::SEQUENTIAL_SCAN) {
		SequentialScanQP *ss = (SequentialScanQP*)o;
		if(documentIndex_) {
			return ss->getNodeType() == ImpliedSchemaNode::METADATA &&
				container_ == ss->getContainerBase();
		} else {
			return ss->getNodeType() == nodeType_ &&
				(ss->getChildName() == 0 || char_equals(ss->getChildName(), childUriName_)) &&
				container_ == ss->getContainerBase();
		}
	}

	return false;
}

QueryPlan *PresenceQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	PresenceQP *result = new (mm) PresenceQP(QueryPlan::PRESENCE, nodeType_,
		parentUriName_, childUriName_, documentIndex_, key_, operation_, isn_, container_, flags_, mm);
	result->cost_ = cost_;
	result->costSet_ = costSet_;
	result->setLocationInfo(this);
	return result;
}

void PresenceQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

string PresenceQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	if(documentIndex_) s << in << "<DocumentPresenceQP";
	else s << in << "<PresenceQP";

	if(container_ != 0) {
		s << " container=\"" << container_->getName() << "\"";
	}
	if(key_.getIndex() != 0) {
		s << " index=\"" << key_.getIndex().asString() << "\"";
	}
	if(operation_ != DbWrapper::NONE) {
		s << " operation=\"" << DbWrapper::operationToWord(operation_) << "\"";
	}
	if(parentUriName_ != 0) {
		s << " parent=\"" << parentUriName_ << "\"";
	}
	if(childUriName_ != 0) {
		s << " child=\"" << childUriName_ << "\"";
	}
	s << "/>" << endl;

	return s.str();
}

string PresenceQP::toString(bool brief) const
{
	ostringstream s;

	if(documentIndex_) s << "Pd(";
	else s << "P(";

	if(!brief && operation_ != DbWrapper::NONE) {
		s << key_.getIndex().asString() << ",";
		s << DbWrapper::operationToString(operation_) << ",";
	}

	if(parentUriName_ != 0) {
		s << parentUriName_ << ".";
	}

	if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		s << "@";
	}
	else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << "metadata::";
	}
	else if(nodeType_ == ImpliedSchemaNode::DESCENDANT) {
		s << "descendant::";
	}

	if(childUriName_ != 0) {
		s << childUriName_ << ")";
	}
	else {
		s << "null)";
	}

	return s.str();
}

/////////////////////////////////
// ValueQP

ValueQP::ValueQP(ImpliedSchemaNode::Type type,
	const char *parent, const char *child, bool documentIndex,
	DbWrapper::Operation operation, bool generalComp,
	Syntax::Type syntax, const ASTNode *value,
	ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm)
	: PresenceQP(QueryPlan::VALUE, type, parent, child, documentIndex, QPKey(syntax), operation, isn, 0, flags, mm),
	  value_(syntax, value, generalComp, mm)
{
}

ValueQP::ValueQP(ImpliedSchemaNode::Type type,
	const char *parent, const char *child, bool documentIndex,
	Syntax::Type syntax, DbWrapper::Operation operation,
	bool generalComp, const char *value, size_t vlen,
	ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm)
	: PresenceQP(QueryPlan::VALUE, type, parent, child, documentIndex, QPKey(syntax), operation, isn, 0, flags, mm),
	  value_(syntax, value, vlen, generalComp, mm)
{
}

ValueQP::ValueQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
	const char *parent, const char *child, bool documentIndex,
	const QPKey &key, DbWrapper::Operation operation,
	const QPValue &value, ImpliedSchemaNode *isn,
	ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm)
	: PresenceQP(qpType, type, parent, child, documentIndex, key, operation, isn, cont, flags, mm),
	  value_(value, mm)
{
}

QueryPlan *ValueQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	if(value_.getASTNode() != 0) {
		value_.setASTNode(const_cast<ASTNode*>(value_.getASTNode())->staticTyping(context, styper));
	}

	staticTypingLite(context);
	return this;
}

void ValueQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	if(value_.getASTNode() != 0) {
		_src.add(value_.getASTNode()->getStaticAnalysis());
	}

	if(documentIndex_) {
		_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE);
	} else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE);
	} else if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		_src.getStaticType() = StaticType(StaticType::ATTRIBUTE_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::SUBTREE);
	} else {
		_src.getStaticType() = StaticType(StaticType::ELEMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::SUBTREE);
	}

	if(key_.getIndex().getUnique() == Index::UNIQUE_ON && operation_ == DbWrapper::EQUALITY) {
		_src.setProperties(_src.getProperties() | StaticAnalysis::ONENODE | StaticAnalysis::PEER);
	}

	if(isParentSet() && char_equals(parentUriName_, metaDataName_uri_root)) {
		_src.setProperties(_src.getProperties() | StaticAnalysis::PEER);
	}
}

static ImpliedSchemaNode *getPresenceParent(ImpliedSchemaNode *isn)
{
	DBXML_ASSERT(isn != 0);

	switch((ImpliedSchemaNode::Type)isn->getType()) {
	case ImpliedSchemaNode::ATTRIBUTE:
	case ImpliedSchemaNode::CHILD:
	case ImpliedSchemaNode::DESCENDANT:
	case ImpliedSchemaNode::DESCENDANT_ATTR:
	case ImpliedSchemaNode::ROOT:
	case ImpliedSchemaNode::METADATA:
		return isn;
	case ImpliedSchemaNode::CAST:
	case ImpliedSchemaNode::EQUALS:
	case ImpliedSchemaNode::NOT_EQUALS:
	case ImpliedSchemaNode::LTX:
	case ImpliedSchemaNode::LTE:
	case ImpliedSchemaNode::GTX:
	case ImpliedSchemaNode::GTE:
	case ImpliedSchemaNode::PREFIX:
	case ImpliedSchemaNode::SUBSTRING:
	case ImpliedSchemaNode::SUBSTRING_CD:
	case ImpliedSchemaNode::SUFFIX:
		return getPresenceParent((ImpliedSchemaNode*)isn->getParent());
	}

	DBXML_ASSERT(false);
	return 0;
}

QueryPlan *ValueQP::optimize(OptimizationContext &opt)
{
	if(opt.getQueryPlanOptimizer() && value_.getASTNode() != 0) {
		value_.setASTNode(opt.getQueryPlanOptimizer()->optimize(const_cast<ASTNode*>(value_.getASTNode())));
	}

	if(container_ == 0) container_ = opt.getContainerBase();

	if(opt.getContainerBase() && !indexesResolved()) {
		if(!resolveIndexes(*opt.getContainerBase(), opt.getIndexSpecification()) || value_.getSyntax() == Syntax::NONE) {
			// TBD Could we do better in the case of unknown syntax? - jpcs

			if(isParentSet()) {
				// else convert to node-*-equality-* and try again
				ValueQP *val = new (memMgr_) ValueQP(QueryPlan::VALUE, nodeType_, 0, childUriName_, documentIndex_,
					key_, operation_, value_, isn_, container_, flags_, memMgr_);
				val->setLocationInfo(this);
				logTransformation(opt.getLog(), val);
				return val->optimize(opt);
			}
			else {
				// else convert to node-*-presence with a value filter and try again
				QueryPlan *result = new (memMgr_) PresenceQP(nodeType_, 0, childUriName_, documentIndex_, getPresenceParent(isn_), flags_, memMgr_);
				result->setLocationInfo(this);

				if(operation_ != DbWrapper::SUBSTRING) {
					result = new (memMgr_) ValueFilterQP(result, isn_, 0, memMgr_);
					result->setLocationInfo(this);
				}

				logTransformation(opt.getLog(), result);
				return result->optimize(opt);
			}
		}

		// Convert document indexes to node indexes -
		// only done just after we successfully resolve the index to use
		if(!container_->nodesIndexed() && !documentIndex_) {
			documentIndex_ = true;

			QueryPlan *result = createSS(isn_, this, memMgr_);

			if(operation_ != DbWrapper::NEG_NOT_EQUALITY) {
				result = new (memMgr_) DescendantOrSelfJoinQP(this, result, 0, memMgr_);
				result->setLocationInfo(this);
			}

			logTransformation(opt.getLog(), result);
			return result->optimize(opt);
		}

		QueryPlan *result = this;

		// Add extra document indexes for every node index if
		// this is a DLS+ container
		if(container_->nodesIndexed() && !documentIndex_ &&
			container_->getContainer()->isWholedocContainer() &&
			operation_ != DbWrapper::NEG_NOT_EQUALITY) {

			QueryPlan *nodeIndex = copy();
			documentIndex_ = true;
			result = new (memMgr_) DescendantOrSelfJoinQP(result, nodeIndex, 0, memMgr_);
			result->setLocationInfo(this);
			logTransformation(opt.getLog(), result);
		}

		if(!documentIndex_ && operation_ != DbWrapper::SUBSTRING &&
			key_.getIndex().getKey() == Index::KEY_SUBSTRING) {
			// We're using a substring index to perform equals or starts-with() -
			// we'll have to add a ValueFilterQP, since a substring index isn't exact
			result = new (memMgr_) ValueFilterQP(result, isn_, 0, memMgr_);
			result->setLocationInfo(this);
			logTransformation(opt.getLog(), result);
		}

		if(result != this) return result->optimize(opt);
	}

	QueryPlan *result = resolveValues(opt.getLog(), opt.getContext());
	if(result != this) {
		return result->optimize(opt);
	}

	return this;
}

void ValueQP::getKeysForCost(IndexLookups &keys, DynamicContext *context) const
{
	int timezone = ((Manager&)GET_CONFIGURATION(context)->getManager()).getImplicitTimezone();
	if(value_.getASTNode() != 0) {
		const Index &index = key_.getIndex();
		if(index.getKey() == Index::KEY_SUBSTRING) {
			// TBD How can we do this better, given that we don't want to lookup
			// the value? - jpcs

			// Guess that the substring lookup will result in 5 index retrievals
			IndexLookups intersect(/*intersect*/true);
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));
			intersect.add(DbWrapper::EQUALITY, key_.createKey(timezone));

			keys.add(intersect);
		}
		else if(operation_ == DbWrapper::NEG_NOT_EQUALITY) {
			keys.add(DbWrapper::EQUALITY, key_.createKey(timezone));
		}
		else {
			keys.add(operation_, key_.createKey(timezone));
		}
	}
	else {
		getKeysImpl(keys, value_.getValue(), value_.getLength(), timezone);
	}
}

void ValueQP::getKeys(IndexLookups &keys, DynamicContext *context) const
{
	int timezone = ((Manager&)GET_CONFIGURATION(context)->getManager()).getImplicitTimezone();
	if(value_.getASTNode() != 0) {

		Result result = value_.getASTNode()->createResult(context);

		Item::Ptr item = result->next(context);

		// If there are no values and this is a NEG_NOT_EQUALITY then this is equivalent
		// to a presence lookup
		if(operation_ == DbWrapper::NEG_NOT_EQUALITY && item.isNull()) {
			keys.add(DbWrapper::PREFIX, key_.createKey(timezone));
		}

		// Index keys for NEG_NOT_EQUALITY value lookups are intersected, not unioned
		IndexLookups newKeys(/*intersect*/operation_ == DbWrapper::NEG_NOT_EQUALITY);

		while(!item.isNull()) {
			if(item->isNode()) {
				XMLChToUTF8 transcoded(((const Node*)item.get())->dmStringValue(context));
				getKeysImpl(newKeys, transcoded.str(), transcoded.len(), timezone);
			}
			else {
				XMLChToUTF8 transcoded(item->asString(context));
				getKeysImpl(newKeys, transcoded.str(), transcoded.len(), timezone);
			}

			item = result->next(context);
			if(!value_.isGeneralComp() && !item.isNull()) {
				XQThrow3(XPath2TypeMatchException,X("ValueQP::getKeys"),
					X("A parameter of the operator is not a single atomic value [err:XPTY0004]"),
					value_.getASTNode());
			}
		}

		keys.add(newKeys);
	}
	else {
		getKeysImpl(keys, value_.getValue(), value_.getLength(), timezone);
	}
}

void ValueQP::getKeysImpl(IndexLookups &keys, const char *value, size_t length, int timezone) const
{
	const Index &index = key_.getIndex();
	if(index.getKey() == Index::KEY_SUBSTRING) {
		// Split the value using the KeyGenerator from the syntax
		const Syntax *syntax = SyntaxManager::getInstance()->getSyntax((Syntax::Type)index.getSyntax());
		KeyGenerator::Ptr kg = syntax->getKeyGenerator(index, value, length, /*forIndex*/false);

		const char *keyBuffer = 0;
		size_t keyLength = 0;
		if(kg->noOfKeys() == 0) {
			if(container_->getContainer()->getIndexVersion() >= SUBSTRING_3CHAR_VERSION) {
				// The value is shorted than 3 significant chars - do a prefix
				// lookup on the value we do have
				kg->getWholeValue(keyBuffer, keyLength);
				keys.add(DbWrapper::PREFIX, key_.createKey(keyBuffer, keyLength, timezone));
			} else {
				// Do a presence lookup when no substring keys are generated
				keys.add(DbWrapper::PREFIX, key_.createKey(timezone));
			}
		}
		else {
			IndexLookups intersect(/*intersect*/true);
			while(kg->next(keyBuffer, keyLength)) {
				intersect.add(DbWrapper::EQUALITY, key_.createKey(keyBuffer, keyLength, timezone));
			}
			keys.add(intersect);
		}
	}
	else if(operation_ == DbWrapper::NEG_NOT_EQUALITY) {
		keys.add(DbWrapper::EQUALITY, key_.createKey(value, length, timezone));
	}
	else {
		keys.add(operation_, key_.createKey(value, length, timezone));
	}
}

// Order of preference for indexes
//
// edge-*-equality-* (if not substring)
// edge-*-substring-* (if equality, prefix or substring)
// else convert to n(node-*-equality-*, edge-*-presence) and try again
// ---- or ----
// node-*-equality-* (if not substring)
// node-*-substring-* (if equality, prefix or substring)
// else convert to node-*-presence and try again
bool ValueQP::resolveIndexes(const ContainerBase &container, const IndexSpecification &is)
{
	Index &index = key_.getIndex();

	if(index.getNode() != Index::NODE_NONE) return true;

	container_ = &(const_cast<ContainerBase&>(container));

	const IndexVector *iv = is.getIndexOrDefault(childUriName_);

	if(iv) {
		Index::Type searchMask = Index::PNKS_MASK;
		if(value_.getSyntax() == Syntax::NONE) {
			// We don't know the syntax yet - so we won't include it
			// in our search for an index. If we find indexes that
			// potentially match, we won't convert ourselves.
			searchMask = Index::PNK_MASK;
		}

		index.set(nodeTypeToIndexType(nodeType_), Index::NODE_MASK);

		if(isParentSet()) {
			if(operation_ != DbWrapper::SUBSTRING) {
				// edge-*-equality-* (if not substring)
				index.set(Index::PATH_EDGE | Index::KEY_EQUALITY, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK) {
						index = iv->getIndex(index, searchMask);
						logIndexUse(container, index, operation_);
					}
					else index.set(Index::NODE_NONE, Index::NODE_MASK);
					return true;
				}
			}

			if(operation_ == DbWrapper::EQUALITY ||
			   operation_ == DbWrapper::NEG_NOT_EQUALITY ||
			   operation_ == DbWrapper::PREFIX ||
			   operation_ == DbWrapper::SUBSTRING) {
				// edge-*-substring-* (if equality, prefix or substring)
				index.set(Index::PATH_EDGE | Index::KEY_SUBSTRING, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK) {
						index = iv->getIndex(index, searchMask);
						logIndexUse(container, index, operation_);
					}
					else index.set(Index::NODE_NONE, Index::NODE_MASK);
					return true;
				}
			}

		}
		else { // !isParentSet()

			if(operation_ != DbWrapper::SUBSTRING) {
				// node-*-equality-* (if not substring)
				index.set(Index::PATH_NODE | Index::KEY_EQUALITY, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK) {
						index = iv->getIndex(index, searchMask);
						logIndexUse(container, index, operation_);
					}
					else index.set(Index::NODE_NONE, Index::NODE_MASK);
					return true;
				}
			}

			if(operation_ == DbWrapper::EQUALITY ||
			   operation_ == DbWrapper::NEG_NOT_EQUALITY ||
			   operation_ == DbWrapper::PREFIX ||
			   operation_ == DbWrapper::SUBSTRING) {
				// node-*-substring-* (if equality, prefix or substring)
				index.set(Index::PATH_NODE | Index::KEY_SUBSTRING, Index::PK_MASK);
				if(iv->isEnabled(index, searchMask)) {
					if(searchMask == Index::PNKS_MASK) {
						index = iv->getIndex(index, searchMask);
						logIndexUse(container, index, operation_);
					}
					else index.set(Index::NODE_NONE, Index::NODE_MASK);
					return true;
				}
			}
		}
	}

	index.set(Index::NODE_NONE, Index::NODE_MASK);
	return false;
}

QueryPlan *ValueQP::resolveFromResult(Result &result, const Log &log, DynamicContext *context, const LocationInfo *location)
{
	Item::Ptr item = result->next(context);
	if(item.isNull()) {
		if(operation_ == DbWrapper::NEG_NOT_EQUALITY) {
			// If there are no values and this is a NEG_NOT_EQUALITY then this is equivalent
			// to a presence lookup
			QueryPlan *result = new (memMgr_) PresenceQP(nodeType_, 0, childUriName_, documentIndex_, getPresenceParent(isn_), flags_, memMgr_);
			result->setLocationInfo(this);
			logTransformation(log, result);
			return result;
		} else {
			// Otherwise it's empty
			EmptyQP *empty = new (memMgr_) EmptyQP(0, memMgr_);
			empty->setLocationInfo(this);
			logTransformation(log, empty);
			return empty;
		}
	}

	DbWrapper::Operation op;
	OperationQP *resultOp;
	if(operation_ == DbWrapper::NEG_NOT_EQUALITY) {
		op = DbWrapper::EQUALITY;
		resultOp = new (memMgr_) IntersectQP(0, memMgr_);
	}
	else {
		op = operation_;
		resultOp = new (memMgr_) UnionQP(0, memMgr_);
	}

	while(!item.isNull()) {
		const XMLCh *value16;
		Syntax::Type syntax = value_.getSyntax();
		if(item->isNode()) {
			value16 = ((const Node*)item.get())->dmStringValue(context);
			if(syntax == Syntax::NONE) {
				syntax = Syntax::STRING;
			}
		}
		else {
			value16 = item->asString(context);
			if(syntax == Syntax::NONE) {
				syntax = Value::getSyntaxType(item);
			}
		}

		XMLChToUTF8 value8(value16);
		QueryPlan *arg = new (memMgr_) ValueQP(nodeType_, parentUriName_, childUriName_, documentIndex_,
			syntax, op, value_.isGeneralComp(), value8.str(), value8.len(), isn_, flags_, memMgr_);
		arg->setLocationInfo(this);
		resultOp->addArg(arg);

		item = result->next(context);
		if(!value_.isGeneralComp() && !item.isNull()) {
			XQThrow3(XPath2TypeMatchException,X("ValueQP::getKeys"),
				X("A parameter of the operator is not a single atomic value [err:XPTY0004]"),
				location);
		}
	}

	resultOp->compress();
	logTransformation(log, resultOp);
	return resultOp;
}

QueryPlan *ValueQP::resolveValues(const Log &log, DynamicContext *context)
{
	// Not worth doing if the indexes aren't resolved yet
	if(!indexesResolved()) return this;

	bool success = true;
	Result result(0);

	const ASTNode *values = value_.getASTNode();
	if(values != 0 && values->isConstant()) {
		try {
			result = values->createResult(context);
		}
		catch(...) {
			success = false;
		}
	} else {
		success = false;
	}

	if(!success) return this;

	return resolveFromResult(result, log, context, value_.getASTNode());
}

static bool operationSubset(DbWrapper::Operation l, DbWrapper::Operation r)
{
	if(l == r) return true;

	switch(l) {
	case DbWrapper::LTX: {
		if(r == DbWrapper::LTE) return true;
		break;
	}
	case DbWrapper::GTX: {
		if(r == DbWrapper::GTE) return true;
		break;
	}
	case DbWrapper::PREFIX: {
		if(r == DbWrapper::SUBSTRING) return true;
		break;
	}
	case DbWrapper::EQUALITY: {
		if(r != DbWrapper::NEG_NOT_EQUALITY) return true;
	}
	default: break;
	}
	return false;
}

bool ValueQP::isSubsetOfValue(const PresenceQP *step, DbWrapper::Operation myOp, DbWrapper::Operation hisOp) const
{
	if(char_equals(step->getChildName(), childUriName_) &&
	   step->getNodeType() == nodeType_) {

		if(hisOp == myOp) {

			// x-y-(equality|substring) <isSubsetOf> x-y-(equality|substring)
			if(isParentSet() && step->isParentSet() &&
			   char_equals(parentUriName_, step->getParentName())) {
				return true;
			}

			// *-x-(equality|substring) <isSubsetOf> node-x-(equality|substring)
			if(!step->isParentSet()) {
				return true;
			}
		}

		// x-y-(equality|substring) (op1) <isSubsetOf> x-y-(equality|substring) (op2) iff operationSubset(op1, op2)
		if(isParentSet() == step->isParentSet() &&
		   char_equals(parentUriName_, step->getParentName()) &&
		   operationSubset(myOp, hisOp)) {
			return true;
		}
	}

	return false;
}

bool ValueQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == QueryPlan::VALUE) {
		const ValueQP *step = (const ValueQP*)o;

		if(container_ != step->getContainerBase()) return false;

		bool thisReturnsDocs = getReturnType() == ImpliedSchemaNode::METADATA;
		bool stepReturnsDocs = step->getReturnType() == ImpliedSchemaNode::METADATA;
		if(thisReturnsDocs != stepReturnsDocs) return false;

		if(value_.equals(step->value_) &&
		   isSubsetOfValue(step, operation_, step->operation_)) {
			return true;
		}
	}
	else if(o->getType() == QueryPlan::RANGE) {
		const RangeQP *step = (const RangeQP*)o;

		if(container_ != step->getContainerBase()) return false;
		if(documentIndex_ != step->isDocumentIndex()) return false;

		if(value_.equals(step->getValue()) &&
		   isSubsetOfValue(step, operation_, step->getOperation()) &&
		   value_.equals(step->getValue2()) &&
		   isSubsetOfValue(step, operation_, step->getOperation2())) {
			return true;
		}
	}
	else {
		return PresenceQP::isSubsetOf(o);
	}

	return false;
}

QueryPlan *ValueQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	ValueQP *result = new (mm) ValueQP(QueryPlan::VALUE, nodeType_,
		parentUriName_, childUriName_, documentIndex_, key_,
		operation_, value_, isn_, container_, flags_, mm);
	result->cost_ = cost_;
	result->costSet_ = costSet_;
	result->setLocationInfo(this);
	return result;
}

void ValueQP::release()
{
	value_.release();
	_src.clear();
	memMgr_->deallocate(this);
}

string ValueQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	if(documentIndex_) s << in << "<DocumentValueQP";
	else s << in << "<ValueQP";

	if(container_ != 0) {
		s << " container=\"" << container_->getName() << "\"";
	}
	if(key_.getIndex() != 0) {
		s << " index=\"" << key_.getIndex().asString() << "\"";
	}
	if(operation_ != DbWrapper::NONE) {
		s << " operation=\"" << DbWrapper::operationToWord(operation_) << "\"";
	}
	if(parentUriName_ != 0) {
		s << " parent=\"" << parentUriName_ << "\"";
	}
	if(childUriName_ != 0) {
		s << " child=\"" << childUriName_ << "\"";
	}

	if(value_.getASTNode() == 0) {
		s << " value=\"" << string(value_.getValue(), value_.getLength()) << "\"";
		s << "/>" << endl;
	} else {
		s << ">" << endl;
		s << DbXmlPrintAST::print(value_.getASTNode(), context, indent + INDENT);

		if(documentIndex_) s << in << "</DocumentValueQP>" << endl;
		else s << in << "</ValueQP>" << endl;
	}

	return s.str();
}

string ValueQP::toString(bool brief) const
{
	ostringstream s;

	if(documentIndex_) s << "Vd(";
	else s << "V(";

	if(!brief && value_.getSyntax() != Syntax::NONE && key_.getIndex() != 0) {
		s << key_.getIndex().asString() << ",";
	}

	if(isParentSet()) {
		s << parentUriName_ << ".";
	}

	if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		s << "@";
	}
	else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << "metadata::";
	}
	else if(nodeType_ == ImpliedSchemaNode::DESCENDANT) {
		s << "descendant::";
	}

	s << childUriName_ << "," << DbWrapper::operationToString(operation_);
	s << ",'" << value_.asString() << "')";

	return s.str();
}

/////////////////////////////////
// RangeQP

RangeQP::RangeQP(ImpliedSchemaNode::Type type,
	const char *parent, const char *child, bool documentIndex,
	const QPKey &key1, DbWrapper::Operation operation1,
	const QPValue &value1, DbWrapper::Operation operation2,
	const QPValue &value2, ImpliedSchemaNode *isn1, ImpliedSchemaNode *isn2,
	ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm)
	: ValueQP(QueryPlan::RANGE, type, parent, child, documentIndex, key1, operation1, value1, isn1, cont, flags, mm),
	  isn2_(isn2),
	  value2_(value2, mm),
	  operation2_(operation2)
{
}

RangeQP::RangeQP(const ValueQP *gt, const ValueQP *lt, XPath2MemoryManager *mm)
	: ValueQP(QueryPlan::RANGE, gt->getNodeType(), gt->getParentName(), gt->getChildName(), gt->isDocumentIndex(),
		  gt->getKey(), gt->getOperation(), gt->getValue(), gt->getImpliedSchemaNode(), gt->getContainerBase(), gt->getFlags() | lt->getFlags(), mm),
	  isn2_(lt->getImpliedSchemaNode()),
	  value2_(lt->getValue(), mm),
	  operation2_(lt->getOperation())
{
}

QueryPlan *RangeQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	if(value_.getASTNode() != 0) {
		value_.setASTNode(const_cast<ASTNode*>(value_.getASTNode())->staticTyping(context, styper));
	}

	if(value2_.getASTNode() != 0) {
		value2_.setASTNode(const_cast<ASTNode*>(value2_.getASTNode())->staticTyping(context, styper));
	}

	staticTypingLite(context);
	return this;
}

void RangeQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	if(value_.getASTNode() != 0) {
		_src.add(value_.getASTNode()->getStaticAnalysis());
	}

	if(value2_.getASTNode() != 0) {
		_src.add(value2_.getASTNode()->getStaticAnalysis());
	}

	if(documentIndex_) {
		_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE);
	} else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		_src.getStaticType() = StaticType(StaticType::DOCUMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::PEER | StaticAnalysis::SUBTREE);
	} else if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		_src.getStaticType() = StaticType(StaticType::ATTRIBUTE_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::SUBTREE);
	} else {
		_src.getStaticType() = StaticType(StaticType::ELEMENT_TYPE, 0, StaticType::UNLIMITED);
		_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
			StaticAnalysis::SUBTREE);
	}

	if(isParentSet() && char_equals(parentUriName_, metaDataName_uri_root)) {
		_src.setProperties(_src.getProperties() | StaticAnalysis::PEER);
	}
}

QueryPlan *RangeQP::optimize(OptimizationContext &opt)
{
	if(opt.getQueryPlanOptimizer()) {
		if(value_.getASTNode() != 0) {
			value_.setASTNode(opt.getQueryPlanOptimizer()->optimize(const_cast<ASTNode*>(value_.getASTNode())));
		}
		if(value2_.getASTNode() != 0) {
			value2_.setASTNode(opt.getQueryPlanOptimizer()->optimize(const_cast<ASTNode*>(value2_.getASTNode())));
		}
	}

	if(container_ == 0) container_ = opt.getContainerBase();

	if(opt.getContainerBase() && !indexesResolved()) {
		if(!resolveIndexes(*opt.getContainerBase(), opt.getIndexSpecification())) {
			if(isParentSet()) {
				// else convert to node-*-equality-* and try again
				RangeQP *range = new (memMgr_) RangeQP(nodeType_, 0, childUriName_, documentIndex_, key_, operation_,
					value_, operation2_, value2_, isn_, isn2_, container_, flags_, memMgr_);
				range->setLocationInfo(this);
				logTransformation(opt.getLog(), range);
				return range->optimize(opt);
			} else {
				// else convert to node-*-presence with two value filters and try again
				QueryPlan *result = new (memMgr_) PresenceQP(nodeType_, 0, childUriName_, documentIndex_, getPresenceParent(isn_), flags_, memMgr_);
				result->setLocationInfo(this);
				result = new (memMgr_) ValueFilterQP(result, isn_, 0, memMgr_);
				result->setLocationInfo(this);
				result = new (memMgr_) ValueFilterQP(result, isn2_, 0, memMgr_);
				result->setLocationInfo(this);
				logTransformation(opt.getLog(), result);
				return result->optimize(opt);
			}
		}

		// Convert document indexes to node indexes -
		// only done just after we successfully resolve the index to use
		if(!container_->nodesIndexed() && !documentIndex_) {
			documentIndex_ = true;

			QueryPlan *result = createSS(isn_, this, memMgr_);
			result = new (memMgr_) ValueFilterQP(result, isn2_, 0, memMgr_);
			result->setLocationInfo(this);
			result = new (memMgr_) DescendantOrSelfJoinQP(this, result, 0, memMgr_);
			result->setLocationInfo(this);
			logTransformation(opt.getLog(), result);
			return result->optimize(opt);
		}
		// Add extra document indexes for every node index if
		// this is a DLS+ container
		if(container_->nodesIndexed() && !documentIndex_ &&
			container_->getContainer()->isWholedocContainer()) {

			QueryPlan *result = copy();
			documentIndex_ = true;
			result = new (memMgr_) DescendantOrSelfJoinQP(this, result, 0, memMgr_);
			result->setLocationInfo(this);
			logTransformation(opt.getLog(), result);
			return result->optimize(opt);
		}
	}

	return this;
}

NodeIterator *RangeQP::createNodeIterator(DynamicContext *context) const
{
	DBXML_ASSERT(indexesResolved());

	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	int timezone = ((Manager&)conf->getManager()).getImplicitTimezone();
	Key key1 = key_.createKey(value_, timezone);
	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		key1.setIDsFromNames(conf->getOperationContext(), *container_, parentUriName_, childUriName_);
		const_cast<QPKey&>(key_).setID1(key1.getID1());
		const_cast<QPKey&>(key_).setID2(key1.getID2());
	}

	if(key1.getID1() == 0 || (isParentSet() && key1.getID2() == 0)) {
		return 0;
	} else {
		Key key2 = key_.createKey(value2_, timezone);
		return container_->createIndexIterator(key1.getSyntaxType(), context,
			documentIndex_, this, operation_, key1, operation2_, key2);
	}
}

Cost RangeQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	if(!costSet_ && indexesResolved()) {
		int timezone = ((Manager&)qec.getContext().getManager()).getImplicitTimezone();
		Key key = key_.createKey(value_, timezone);
		if(key.getID1() == 0 || (isParentSet() && key.getID2() == 0)) {
			key.setIDsFromNames(context, *container_, parentUriName_, childUriName_);
			const_cast<QPKey&>(key_).setID1(key.getID1());
			const_cast<QPKey&>(key_).setID2(key.getID2());
		}
		Key key2 = key_.createKey(value2_, timezone);
		cost_ = container_->getIndexCost(context, operation_, key, operation2_, key2);
		costSet_ = true;
	}
	return cost_;
}

// Order of preference for indexes
//
// edge-*-equality-*
// else convert to n(node-*-equality-*, edge-*-presence) and try again
// ---- or ----
// node-*-equality-*
// else convert to node-*-presence and try again
bool RangeQP::resolveIndexes(const ContainerBase &container, const IndexSpecification &is)
{
	Index &index = key_.getIndex();

	if(index.getNode() != Index::NODE_NONE) return true;

	container_ = &const_cast<ContainerBase&>(container);

	const IndexVector *iv = is.getIndexOrDefault(childUriName_);

	if(iv) {
		index.set(nodeTypeToIndexType(nodeType_), Index::NODE_MASK);

		if(isParentSet()) {
			// edge-*-equality-*
			index.set(Index::PATH_EDGE | Index::KEY_EQUALITY, Index::PK_MASK);
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				logIndexUse(container, key_.getIndex(), operation_);
				logIndexUse(container, key_.getIndex(), operation2_);
				return true;
			}

		}
		else { // !isParentSet()
			// node-*-equality-*
			index.set(Index::PATH_NODE | Index::KEY_EQUALITY, Index::PK_MASK);
			if(iv->isEnabled(index, Index::PNKS_MASK)) {
				logIndexUse(container, key_.getIndex(), operation_);
				logIndexUse(container, key_.getIndex(), operation2_);
				return true;
			}
		}
	}

	index.set(Index::NODE_NONE, Index::NODE_MASK);
	return false;
}

void RangeQP::findQueryPlanRoots(QPRSet &qprset) const
{
	qprset.insert(((ImpliedSchemaNode*)isn_->getRoot())->getQueryPlanRoot());
	qprset.insert(((ImpliedSchemaNode*)isn2_->getRoot())->getQueryPlanRoot());
}

bool RangeQP::isSubsetOf(const QueryPlan *o) const
{
	// Behaves like the intersection of two ValueQP objects
	// (which is what it is, really)

	if(o->getType() == QueryPlan::VALUE) {
		const ValueQP *step = (const ValueQP*)o;

		if(container_ != step->getContainerBase()) return false;

		bool thisReturnsDocs = getReturnType() == ImpliedSchemaNode::METADATA;
		bool stepReturnsDocs = step->getReturnType() == ImpliedSchemaNode::METADATA;
		if(thisReturnsDocs != stepReturnsDocs) return false;

		if(value_.equals(step->getValue()) &&
		   isSubsetOfValue(step, operation_, step->getOperation())) {
			return true;
		}
		if(key_.getIndex().getSyntax() == step->getKey().getIndex().getSyntax() &&
		   step->getValue().equals(value2_) &&
		   isSubsetOfValue(step, operation2_, step->getOperation())) {
			return true;
		}
	}
	else if(o->getType() == QueryPlan::RANGE) {
		const RangeQP *step = (const RangeQP*)o;

		if(container_ != step->getContainerBase()) return false;
		if(documentIndex_ != step->isDocumentIndex()) return false;

		if(value_.equals(step->getValue()) &&
		   isSubsetOfValue(step, operation_, step->getOperation()) &&
		   value2_.equals(step->getValue2()) &&
		   isSubsetOfValue(step, operation2_, step->getOperation2())) {
			return true;
		}
		if(key_.getIndex().getSyntax() == step->getKey().getIndex().getSyntax() &&
		   value2_.equals(step->getValue()) &&
		   isSubsetOfValue(step, operation2_, step->getOperation()) &&
		   value_.equals(step->getValue2()) &&
		   isSubsetOfValue(step, operation_, step->getOperation2())) {
			return true;
		}
	}
	else {
		return ValueQP::isSubsetOf(o);
	}

	return false;
}

QueryPlan *RangeQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	RangeQP *result = new (mm) RangeQP(nodeType_, parentUriName_, childUriName_, documentIndex_,
		key_, operation_, value_, operation2_, value2_, isn_, isn2_, container_, flags_, mm);
	result->cost_ = cost_;
	result->costSet_ = costSet_;
	result->setLocationInfo(this);
	return result;
}

void RangeQP::release()
{
	value_.release();
	value2_.release();
	_src.clear();
	memMgr_->deallocate(this);
}

string RangeQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	if(documentIndex_) s << in << "<DocumentRangeQP";
	else s << in << "<RangeQP";

	if(container_ != 0) {
		s << " container=\"" << container_->getName() << "\"";
	}
	if(key_.getIndex() != 0) {
		s << " index=\"" << key_.getIndex().asString() << "\"";
	}
	if(operation_ != DbWrapper::NONE) {
		s << " operation=\"" << DbWrapper::operationToWord(operation_) << "\"";
	}
	if(operation2_ != DbWrapper::NONE) {
		s << " operation2=\"" << DbWrapper::operationToWord(operation2_) << "\"";
	}
	if(parentUriName_ != 0) {
		s << " parent=\"" << parentUriName_ << "\"";
	}
	if(childUriName_ != 0) {
		s << " child=\"" << childUriName_ << "\"";
	}

	if(value_.getASTNode() == 0) {
		s << " value=\"" << string(value_.getValue(), value_.getLength()) << "\"";
	}
	if(value2_.getASTNode() == 0) {
		s << " value2=\"" << string(value2_.getValue(), value2_.getLength()) << "\"";
	}
	if(value_.getASTNode() == 0 && value2_.getASTNode() == 0) {
		s << "/>" << endl;
	} else {
		s << ">" << endl;
		if(value_.getASTNode() != 0) {
			s << DbXmlPrintAST::print(value_.getASTNode(), context, indent + INDENT);
		}
		if(value2_.getASTNode() != 0) {
			s << DbXmlPrintAST::print(value2_.getASTNode(), context, indent + INDENT);
		}

		if(documentIndex_) s << in << "</DocumentRangeQP>";
		else s << in << "</RangeQP>";
	}

	return s.str();
}

string RangeQP::toString(bool brief) const
{
	ostringstream s;

	if(documentIndex_) s << "Rd(";
	else s << "R(";

	if(!brief && key_.getIndex() != 0) {
		s << key_.getIndex().asString() << ",";
	}

	if(isParentSet()) {
		s << parentUriName_ << ".";
	}

	if(nodeType_ == ImpliedSchemaNode::ATTRIBUTE) {
		s << "@";
	}
	else if(nodeType_ == ImpliedSchemaNode::METADATA) {
		s << "metadata::";
	}
	else if(nodeType_ == ImpliedSchemaNode::DESCENDANT) {
		s << "descendant::";
	}

	s << childUriName_ << ",";
	s << DbWrapper::operationToString(operation_) << ",'" << value_.asString() << "',";
	s << DbWrapper::operationToString(operation2_) << ",'" << value2_.asString() << "')";

	return s.str();
}

/////////////////////////////////
// QPKey

QPKey::QPKey()
	: nodeLookup_(false),
	  index_(Index::NONE),
	  id1_(0),
	  id2_(0)
{
}

QPKey::QPKey(Syntax::Type syntax)
	: nodeLookup_(false),
	  index_(Index::NONE),
	  id1_(0),
	  id2_(0)
{
	if(syntax != Syntax::NONE)
		index_.set(syntax, Index::SYNTAX_MASK);
}

void QPKey::setIDsFromNames(OperationContext &context, const ContainerBase &container,
	const char *parentUriName, const char *childUriName)
{
	const_cast<ContainerBase&>(container).lookupID(
		context, childUriName, ::strlen(childUriName), id1_);
	if(parentUriName == 0 || *parentUriName == 0) {
		id2_.reset();
	} else {
		const_cast<ContainerBase&>(container).lookupID(
			context, parentUriName, ::strlen(parentUriName), id2_);
	}
}

Key QPKey::createKey(int timezone) const
{
	Key result(timezone);
	result.setNodeLookup(nodeLookup_);
	result.setIndex(index_);
	result.setID1(id1_);
	result.setID2(id2_);
	return result;
}

Key QPKey::createKey(const QPValue &value, int timezone) const
{
	Key result(timezone);
	result.setNodeLookup(nodeLookup_);
	result.setIndex(index_);
	result.setID1(id1_);
	result.setID2(id2_);
	result.setValue(value.getValue(), value.getLength());
	return result;
}

Key QPKey::createKey(const char *value, size_t length, int timezone) const
{
	Key result(timezone);
	result.setNodeLookup(nodeLookup_);
	result.setIndex(index_);
	result.setID1(id1_);
	result.setID2(id2_);
	result.setValue(value, length);
	return result;
}

/////////////////////////////////
// QPValue

QPValue::QPValue(Syntax::Type syntax, const char *value, size_t vlen, bool generalComp, XPath2MemoryManager *mm)
	: syntax_(syntax),
	  value_(char_clone(value, vlen, mm)),
	  len_(vlen),
	  di_(0),
	  generalComp_(generalComp),
	  mm_(mm)
{
}

QPValue::QPValue(Syntax::Type syntax, const ASTNode *di, bool generalComp, XPath2MemoryManager *mm)
	: syntax_(syntax),
	  value_(0),
	  len_(0),
	  di_(di),
	  generalComp_(generalComp),
	  mm_(mm)
{
}

QPValue::QPValue(const QPValue &o, XPath2MemoryManager *mm)
	: syntax_(o.syntax_),
	  value_(o.value_),
	  len_(o.len_),
	  di_(o.di_),
	  generalComp_(o.generalComp_),
	  mm_(mm)
{
	if(value_ != 0) value_ = char_clone(value_, len_, mm_);
}

void QPValue::release()
{
	if(value_ != 0) mm_->deallocate((void*)value_);
}

bool QPValue::equals(const QPValue &o) const
{
	if(di_ != 0)
		return di_ == o.di_ &&
			generalComp_ == o.generalComp_;
	else return syntax_ == o.syntax_ &&
		     char_equals(value_, len_, o.value_, o.len_) &&
		     generalComp_ == o.generalComp_;
}

std::string QPValue::asString() const
{
	ostringstream oss;

	if(di_ != 0) {
		oss << "[to be calculated]";
	}
	else if(value_ != 0) {
		oss << string(value_, len_);
	}

	return oss.str();
}
