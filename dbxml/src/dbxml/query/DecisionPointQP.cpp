//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DecisionPointQP.hpp"
#include "QueryPlanHolder.hpp"
#include "../QueryContext.hpp"
#include "../Manager.hpp"
#include "../Container.hpp"
#include "../ReferenceMinder.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../dataItem/DbXmlUserData.hpp"
#include "../optimizer/QueryPlanOptimizer.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/framework/XPath2MemoryManagerImpl.hpp>

#include <sstream>

using namespace DbXml;
using namespace std;

static const int INDENT = 1;

DecisionPointSource *QueryPlanDPSource::copy(XPath2MemoryManager *mm) const
{
	return new (mm) QueryPlanDPSource(parent_->copy(mm), mm);
}

void QueryPlanDPSource::release()
{
	parent_->release();
	mm_->deallocate(this);
}

void QueryPlanDPSource::staticTyping(StaticContext *context, StaticTyper *styper, StaticAnalysis &src)
{
	parent_ = parent_->staticTyping(context, styper);
	// The parent's StaticAnalysis is added by the DecisionPointEndQP objects
}

void QueryPlanDPSource::staticTypingLite(StaticContext *context, StaticAnalysis &src)
{
	parent_->staticTypingLite(context);
	// The parent's StaticAnalysis is added by the DecisionPointEndQP objects
}

void QueryPlanDPSource::optimize(OptimizationContext &opt)
{
	parent_ = parent_->optimize(opt);
}

DecisionPointSource *QueryPlanDPSource::chooseAlternative(OptimizationContext &opt) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();
	return new (mm) QueryPlanDPSource(parent_->chooseAlternative(opt, "QueryPlanDPSource"), mm);
}

ContainerIterator *QueryPlanDPSource::createContainerIterator(DynamicContext *context) const
{
	return new QueryPlanContainerIterator(parent_->createNodeIterator(context));
}

string QueryPlanDPSource::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<QueryPlanDPSource>" << endl;
	s << parent_->printQueryPlan(context, indent + INDENT);
	s << in << "</QueryPlanDPSource>" << endl;

	return s.str();
}

string QueryPlanDPSource::toString(bool brief) const
{
	ostringstream s;

	s << "QPS(";
	s << parent_->toString(brief);
	s << ")";

	return s.str();
}


////////////////////////////////////////////////////////////////////////////////////////////////////

DecisionPointQP::DecisionPointQP(QueryPlan *arg, DecisionPointSource *dps, u_int32_t flags, XPath2MemoryManager *mm)
	: QueryPlan(DECISION_POINT, flags, mm),
	  dps_(dps),
	  arg_(arg),
	  removed_(false),
	  qpList_(0),
	  qpListDone_(false),
	  compileTimeMinder_(0),
	  compileTimeContext_(0)
{
	_src.add(arg->getStaticAnalysis());
}

QueryPlan *DecisionPointQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	if(dps_ != 0) dps_->staticTyping(context, styper, _src);

	if(arg_ != 0) {
		arg_ = arg_->staticTyping(context, styper);
		_src.copy(arg_->getStaticAnalysis());
	}

	for(ListItem *li = qpList_; li != 0; li = li->next) {
		li->qp = li->qp->staticTyping(context, styper);
		_src.copy(li->qp->getStaticAnalysis());
	}

	return this;
}

void DecisionPointQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	if(dps_ != 0) dps_->staticTypingLite(context, _src);

	if(arg_ != 0) {
		arg_->staticTypingLite(context);
		_src.copy(arg_->getStaticAnalysis());
	}

	for(ListItem *li = qpList_; li != 0; li = li->next) {
		li->qp->staticTypingLite(context);
		_src.copy(li->qp->getStaticAnalysis());
	}
}

QueryPlan *DecisionPointQP::optimize(OptimizationContext &opt)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Optimise the parent QueryPlan
	if(dps_) dps_->optimize(opt);

	// Save the compile time mutex, in case we need to just-in-time
	// optimise at runtime
	if(compileTimeMinder_ == 0) {
		compileTimeMinder_ = GET_CONFIGURATION(opt.getContext())->getMinder();
		compileTimeContext_ = opt.getContext();
	}

	bool unknownContainers = true;
	if(!qpListDone_ && opt.getPhase() == OptimizationContext::RESOLVE_INDEXES) {
		qpListDone_ = true;

		// Find the QueryPlanRoots, before we optimise the argument and
		// get rid of the PathsQP objects.
		QPRSet qprset;
		arg_->findQueryPlanRoots(qprset);

		// Work out what containers we will need to operate on
		unknownContainers = false;
		for(QPRSet::iterator it = qprset.begin(); it != qprset.end(); ++it) {
			const QueryPlanRoot *qpr = *it;
			if(qpr == 0 || qpr->getContainerBase() == 0) {
				unknownContainers = true;
			} else {
				// Insert the ListItem in order of the container pointer
				ListItem **li = &qpList_;
				while(*li != 0 && (*li)->container->getContainerID() < qpr->getContainerBase()->getContainerID()) {
					li = &(*li)->next;
				}

				if(*li == 0 || (*li)->container->getContainerID() != qpr->getContainerBase()->getContainerID()) {
					*li = new (mm) ListItem(const_cast<ContainerBase*>(qpr->getContainerBase()), *li);
				}
			}
		}

		// Work out if this DecisionPointQP is really needed, and set a flag if not.
		// The DecisionPointQP is needed if the contained QueryPlan will need to work on
		// unknown containers or more than one container, and it has container specific
		// QueryPlan objects in it.
		if(!unknownContainers && (qpList_ == 0 || qpList_->next == 0)) {
			removed_ = true;
		}
	}

	AutoDecisionPointReset reset(GET_CONFIGURATION(opt.getContext()), this);

	// Partially optimise the argument (anything we can do without
	// already knowing the container)
	if(arg_ != 0) {
		OptimizationContext newOpt(opt.getPhase(), opt.getContext(), opt.getQueryPlanOptimizer());
		arg_->logQP(newOpt.getLog(), "RQP", arg_, opt.getPhase());

		arg_ = arg_->optimize(newOpt);

		if(qpList_ == 0)
			arg_->logQP(newOpt.getLog(), "OQP", arg_, opt.getPhase());
		else arg_->logQP(newOpt.getLog(), "POQP", arg_, opt.getPhase());

		if(opt.getPhase() == OptimizationContext::RESOLVE_INDEXES) {
			// Copy the QueryPlan to the ListItems
			for(ListItem *li = qpList_; li != 0; li = li->next) {
				li->qp = arg_->copy(mm);
			}
		}
	}

	// Optimize the query plans for each known container
	for(ListItem *li = qpList_; li != 0; li = li->next) {
		OptimizationContext newOpt(opt.getPhase(), opt.getContext(), opt.getQueryPlanOptimizer(), li->container);
		li->qp = li->qp->optimize(newOpt);
		li->qp->logQP(newOpt.getLog(), "OQP", li->qp, opt.getPhase());
	}

	if(removed_) {
		if(qpList_ == 0) return arg_;
		return qpList_->qp;
	}

	if(!unknownContainers && arg_ != 0) {
		arg_->release();
		arg_ = 0;
	}

	return this;
}

void DecisionPointQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	// DecisionPointQP doesn't participate in any of the optimisations of it's parents,
	// so we make the decision of what to pick here.

	XPath2MemoryManager *mm = opt.getMemoryManager();

	DecisionPointQP *result = new (mm) DecisionPointQP(this, opt, mm);
	result->setLocationInfo(this);

	combinations.push_back(result);
}

class AutoMemoryManagerReset
{
public:
  AutoMemoryManagerReset(StaticContext* context, XPath2MemoryManager *mm)
  {
    context_ = context;
    mm_ = context->getMemoryManager();
    context->setMemoryManager(mm);
  }

  ~AutoMemoryManagerReset()
  {
    context_->setMemoryManager(mm_);
  }

protected:
  StaticContext* context_;
  XPath2MemoryManager *mm_;
};

DecisionPointQP::ListItem *DecisionPointQP::justInTimeOptimize(int contID, DynamicContext *context)
{
	// **** IMPORTANT - This algorithm is very carefully arranged to avoid
	// **** deadlocks and race-conditions. Don't rearrange things unless you
	// **** know what you are doing!
	
	// Get the runtime configuration
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);

	// Lookup the container
	ScopedContainer scont((Manager&)conf->getManager(), contID, /*mustExist*/true);

	// Just-in-time optimise the QueryPlan, using a temporary memory manager for thread safety
	XPath2MemoryManagerImpl tmpMM;
	QueryPlan *qp;
	{
		AutoMemoryManagerReset resetMM(context, &tmpMM);

		qp = arg_->copy(&tmpMM);
		try {
			AutoDecisionPointReset reset(conf, this);
			justInTimeOptimize(qp, scont.get(), context);
		}
		catch(XmlException &e) {
			if(e.getQueryLine() == 0)
				e.setLocationInfo(this);
			throw;
		}
	}

	// Hold the compile time mutex whilst altering the query plan.
	// This protects the compile time XPath2MemoryManager as well
	// as the query plan itself.
	//
	// (The mutex in the runtime configuration is the same as the
	// one from the compile time configuration.)
	MutexLock lock(conf->getMutex());

	// Now we hold the lock, re-search qpList_ for the container,
	// in case someone beat us to creating it.
	DecisionPointQP::ListItem **li = &qpList_;
	while(*li != 0 && (*li)->container->getContainerID() < contID) {
		li = &(*li)->next;
	}

	if(*li == 0 || (*li)->container->getContainerID() != contID) {
		// Add the container to the compile time ReferenceMinder, in case it has been auto-opened
		if (contID > 0)
			compileTimeMinder_->addContainer(scont.getContainer());

		// Create a new ListItem and copy the optimised QueryPlan using the
		// compile time memory manager - so that they can both become a
		// permanent part of the query's AST
		XPath2MemoryManager *compile_mm = compileTimeContext_->getMemoryManager();
		DecisionPointQP::ListItem *newListItem = new (compile_mm) DecisionPointQP::ListItem(scont.get(), *li);
		newListItem->qp = qp->copy(compile_mm);
		newListItem->qp->staticTypingLite(compileTimeContext_);

		// Only add the new ListItem to the linked list once it is fully optimised
		// and ready to execute - otherwise another thread could come along and try
		// to use it.
		*li = newListItem;
	}
	else {
		// All our work was in vain! Someone beat us to creating a QueryPlan
		// for this container. Oh well, we'll just use the existing one then...
	}

	qp->release();
	return *li;
}

void DecisionPointQP::justInTimeOptimize(QueryPlan *&qp, ContainerBase *container, DynamicContext *context)
{
	// This needs to be kept in sync with QueryExpression::createOptimizer()
	{
		OptimizationContext opt(OptimizationContext::RESOLVE_INDEXES, context, 0, container);
		qp = qp->optimize(opt);
		qp->logQP(opt.getLog(), "OQP", qp, opt.getPhase());
	}
	qp->staticTypingLite(context);
	{
		OptimizationContext opt(OptimizationContext::ALTERNATIVES, context, 0, container);
		opt.setCheckForSS(container->getContainerID() == 0);
		qp = qp->chooseAlternative(opt, "decision point");
		qp->logQP(opt.getLog(), "OQP", qp, opt.getPhase());
	}
	{
		OptimizationContext opt(OptimizationContext::REMOVE_REDUNDENTS, context, 0, container);
		qp = qp->optimize(opt);
		qp->logQP(opt.getLog(), "OQP", qp, opt.getPhase());
	}
	qp->staticTypingLite(context);

	// ProjectionSchemaAdjuster not needed
}

NodeIterator *DecisionPointQP::createNodeIterator(DynamicContext *context) const
{
	return new DecisionPointIterator(dps_ == 0 ? 0 : dps_->createContainerIterator(context), this);
}

/** Sets the QueryPlanDPSource object on all it's DecisionPointEndQP objects */
class DPReferenceSetter : public NodeVisitingOptimizer
{
public:
	virtual void run(DecisionPointQP *item)
	{
		dps = (QueryPlanDPSource*)item->getSource();

		if(item->getArg() != 0)
			item->setArg(optimizeQP(item->getArg()));

		for(DecisionPointQP::ListItem *li = item->getList(); li != 0; li = li->next)
			li->qp = optimizeQP(li->qp);
	}

private:
	virtual QueryPlan *optimizeDecisionPoint(DecisionPointQP *item)
	{
		if(item->getSource())
			item->setSource(optimizeDPS(item->getSource()));

		QueryPlanDPSource *old = dps;
		dps = 0;

		if(item->getArg() != 0)
			item->setArg(optimizeQP(item->getArg()));

		for(DecisionPointQP::ListItem *li = item->getList(); li != 0; li = li->next)
			li->qp = optimizeQP(li->qp);

		dps = old;
		return item;
	}

	virtual QueryPlan *optimizeDecisionPointEnd(DecisionPointEndQP *item)
	{
		if(dps) item->setDPSource(dps);
		return item;
	}

	QueryPlanDPSource *dps;
};

// Acts like a copy, but chooses the best alternative instead of copying arguments
DecisionPointQP::DecisionPointQP(const DecisionPointQP *o, OptimizationContext &opt, XPath2MemoryManager *mm)
	: QueryPlan(DECISION_POINT, o->getFlags(), mm),
	  dps_(o->dps_ ? o->dps_->chooseAlternative(opt) : 0),
	  arg_(o->arg_ ? o->arg_->copy(mm) : 0),
	  removed_(false),
	  qpList_(0),
	  qpListDone_(o->qpListDone_),
	  compileTimeMinder_(o->compileTimeMinder_),
	  compileTimeContext_(o->compileTimeContext_)
{
	if(arg_ != 0)
		_src.add(arg_->getStaticAnalysis());
	
	bool checkForSS = opt.checkForSS();
	
	try {
		ListItem **li = &qpList_;
		for(ListItem *oli = o->qpList_; oli != 0; oli = oli->next) {
			opt.setCheckForSS(oli->container->getContainerID() == 0);
		
			*li = new (mm) ListItem(oli->container, 0);
			(*li)->qp = oli->qp->chooseAlternative(opt, "decision point");

			_src.add((*li)->qp->getStaticAnalysis());

			li = &(*li)->next;
		}	

		DPReferenceSetter().run(this);
	}
	catch(...) {
		opt.setCheckForSS(checkForSS);
		throw;
	}
	opt.setCheckForSS(checkForSS);
}

DecisionPointQP::DecisionPointQP(const DecisionPointQP *o, XPath2MemoryManager *mm)
	: QueryPlan(DECISION_POINT, o->getFlags(), mm),
	  dps_(o->dps_ ? o->dps_->copy(mm) : 0),
	  arg_(o->arg_ ? o->arg_->copy(mm) : 0),
	  removed_(false),
	  qpList_(0),
	  qpListDone_(o->qpListDone_),
	  compileTimeMinder_(o->compileTimeMinder_),
	  compileTimeContext_(o->compileTimeContext_)
{
	if(arg_ != 0)
		_src.add(arg_->getStaticAnalysis());

	ListItem **li = &qpList_;
	for(ListItem *oli = o->qpList_; oli != 0; oli = oli->next) {
		*li = new (mm) ListItem(oli->container, 0);
		(*li)->qp = oli->qp->copy(mm);

		_src.add((*li)->qp->getStaticAnalysis());

		li = &(*li)->next;
	}

	DPReferenceSetter().run(this);
}

QueryPlan *DecisionPointQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	DecisionPointQP *result = new (mm) DecisionPointQP(this, mm);
	result->setLocationInfo(this);
	return result;
}

void DecisionPointQP::release()
{
	if(dps_ != 0) dps_->release();

	if(arg_ != 0) arg_->release();

	ListItem *li = qpList_;
	while(li != 0) {
		li->qp->release();

		ListItem *tmp = li->next;
		memMgr_->deallocate(li);
		li = tmp;
	}

	_src.clear();
	memMgr_->deallocate(this);
}

void DecisionPointQP::findQueryPlanRoots(QPRSet &qprset) const
{
}

Cost DecisionPointQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	// We only return the keys here, because the pages is constant for
	// all possible optimisation alternatives - and is therefore irrelevent

	Cost cost;
	if(arg_ != 0)
		cost.keys += arg_->cost(context, qec).keys;

	ListItem *li = qpList_;
	while(li != 0) {
		cost.keys += li->qp->cost(context, qec).keys;

		li = li->next;
	}

	return cost;
}

bool DecisionPointQP::isSubsetOf(const QueryPlan *o) const
{
	return arg_ != 0 ? arg_->isSubsetOf(o) : false;
}

string DecisionPointQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<DecisionPointQP>" << endl;
	if(dps_)
		s << dps_->printQueryPlan(context, indent + INDENT);
	if(arg_ != 0) {
		s << in << "  <UnknownContainer>" << endl;
		s << arg_->printQueryPlan(context, indent + INDENT + INDENT);
		s << in << "  </UnknownContainer>" << endl;
	}
	for(const ListItem *li = qpList_; li != 0; li = li->next) {
		s << in << "  <Container name=\"" << li->container->getName() << "\">" << endl;
		s << li->qp->printQueryPlan(context, indent + INDENT + INDENT);
		s << in << "  </Container>" << endl;
	}
	s << in << "</DecisionPointQP>" << endl;

	return s.str();
}

string DecisionPointQP::toString(bool brief) const
{
	ostringstream s;

	s << "DP(";

	if(dps_) {
		s << dps_->toString(brief);
		if(arg_ != 0) s << ",";
	}

	if(arg_ != 0) s << arg_->toString(brief);
	s << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NodeIterator *DecisionPointEndQP::createNodeIterator(DynamicContext *context) const
{
	DecisionPointIterator *result = GET_CONFIGURATION(context)->getDecisionPointIterator();
	DBXML_ASSERT(result != 0);

	QueryPlanContainerIterator *ci = (QueryPlanContainerIterator*)result->getContainerIterator();

	return new DecisionPointEndIterator(ci, this);
}

QueryPlan *DecisionPointEndQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	DecisionPointEndQP *result = new (mm) DecisionPointEndQP(dps_, flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void DecisionPointEndQP::release()
{
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *DecisionPointEndQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	_src.copy(dps_->getParent()->getStaticAnalysis());

	return this;
}

void DecisionPointEndQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	_src.copy(dps_->getParent()->getStaticAnalysis());
}

QueryPlan *DecisionPointEndQP::optimize(OptimizationContext &opt)
{
	DecisionPointQP *dp = GET_CONFIGURATION(opt.getContext())->getDecisionPoint();
	DBXML_ASSERT(dp);

	dps_ = (QueryPlanDPSource*)dp->getSource();

	// Remove this DecisionPointEndQP if our DecisionPointQP has been removed
	if(dp->isRemoved()) {
		return dps_->getParent();
	}

	return this;
}

void DecisionPointEndQP::findQueryPlanRoots(QPRSet &qprset) const
{
}

Cost DecisionPointEndQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	// We only return the keys here, because the pages is constant for
	// all possible optimisation alternatives - and is therefore irrelevent

	Cost cost;
	cost.keys = dps_->getParent()->cost(context, qec).keys;
	return cost;
}

bool DecisionPointEndQP::isSubsetOf(const QueryPlan *o) const
{
	return dps_->getParent()->isSubsetOf(o);
}

string DecisionPointEndQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<DecisionPointEndQP/>" << endl;

	return s.str();
}

string DecisionPointEndQP::toString(bool brief) const
{
	ostringstream s;

	s << "DPE";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

QueryPlanContainerIterator::~QueryPlanContainerIterator()
{
	delete parent_;
}

NodeIterator *QueryPlanContainerIterator::parentPeekNext(DynamicContext *context)
{
	if(!peekDone_ && parent_) {
		parentMoved_ = true;
		if(!parent_->next(context)) {
			delete parent_;
			parent_ = 0;
		} else {
			peekDone_ = true;
		}
	}
	return parent_;
}

NodeIterator *QueryPlanContainerIterator::parentPeekSeek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!peekDone_ && parent_) {
		parentMoved_ = true;
		if(!parent_->seek(container, did, nid, context)) {
			delete parent_;
			parent_ = 0;
		} else {
			peekDone_ = true;
		}
	}
	return parent_;
}

void QueryPlanContainerIterator::parentTakePeek()
{
	peekDone_ = false;
}

bool QueryPlanContainerIterator::nextContainerID(int &contID, DynamicContext *context)
{
	// Make sure that the parent_ iterator will at least move forward from where we were
	// last time
	if(!parentMoved_) peekDone_ = false;

	// Find the next container (it doesn't matter that we might not be searching on a "real"
	// container ID)
	if(parentPeekSeek(prevCont_ + 1, DocID(), *NsNid::getRootNid(), context) == 0) return false;

	contID = parent_->getContainerID();
	prevCont_ = contID;
	parentMoved_ = false;
	return true;
}

bool QueryPlanContainerIterator::seekContainerID(int &contID, int toSeek, DynamicContext *context)
{
	// Make sure that the parent_ iterator will at least move forward from where we were
	// last time
	if(!parentMoved_) peekDone_ = false;

	// Seek the required container
	if(parentPeekSeek(toSeek, DocID(), *NsNid::getRootNid(), context) == 0) return false;

	contID = parent_->getContainerID();
	prevCont_ = contID;
	parentMoved_ = false;
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

DecisionPointIterator::~DecisionPointIterator()
{
	delete contIt_;
	delete result_;
}

bool DecisionPointIterator::nextContainer(int contID, DynamicContext *context)
{
	DbXmlConfiguration *conf = GET_CONFIGURATION(context);

	{
		// Find the correct QueryPlan for the container
		//
		// Hold the compile time mutex whilst searching the linked list.
		// This stops the list from being modified by just-in-time
		// optimisation while we traverse it.
		//
		// (The mutex in the runtime configuration is the same as the
		// one from the compile time configuration.)
		MutexLock lock(conf->getMutex());
		while(list_ != 0 && list_->container->getContainerID() < contID) {
			list_ = list_->next;
		}
	}

	if(list_ == 0 || list_->container->getContainerID() != contID) {
		list_ = const_cast<DecisionPointQP*>(dp_)->justInTimeOptimize(contID, context);
	}
	AutoDecisionPointIteratorReset reset(conf, this);
	result_ = list_->qp->createNodeIterator(context);

	return true;
}

bool DecisionPointIterator::next(DynamicContext *context)
{
	int contID;
	while(result_ != 0 || (contIt_ && contIt_->nextContainerID(contID, context) && nextContainer(contID, context))) {
		AutoDecisionPointIteratorReset reset(GET_CONFIGURATION(context), this);
		if(result_->next(context)) return true;

		// We have another container to move on to
		delete result_;
		result_ = 0;
	}

	return false;
}

bool DecisionPointIterator::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	int contID;
	while(result_ != 0 || (contIt_ && contIt_->seekContainerID(contID, container, context) && nextContainer(contID, context))) {

		if(result_->seek(container, did, nid, context)) return true;

		// We have another container to move on to
		delete result_;
		result_ = 0;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool DecisionPointEndIterator::next(DynamicContext *context)
{
	result_ = ci_->parentPeekNext(context);
	if(result_ == 0 || result_->getContainerID() != ci_->getLastContainerID()) {
		result_ = 0;
		return false;
	}

	ci_->parentTakePeek();
	return true;
}

bool DecisionPointEndIterator::seek(int container, const DocID &did, const NsNid &nid,
	DynamicContext *context)
{
	result_ = ci_->parentPeekSeek(container, did, nid, context);
	if(result_ == 0 || result_->getContainerID() != ci_->getLastContainerID()) {
		result_ = 0;
		return false;
	}

	ci_->parentTakePeek();
	return true;
}
