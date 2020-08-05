//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "NodePredicateFilterQP.hpp"
#include "PredicateFilterQP.hpp"
#include "QueryExecutionContext.hpp"
#include "StructuralJoinQP.hpp"
#include "QueryPlanToAST.hpp"
#include "ASTToQueryPlan.hpp"
#include "ContextNodeQP.hpp"
#include "VariableQP.hpp"
#include "BufferQP.hpp"
#include "EmptyQP.hpp"
#include "../Container.hpp"
#include "../dataItem/DbXmlPrintAST.hpp"
#include "../UTF8.hpp"
#include "../dataItem/DbXmlConfiguration.hpp"
#include "../optimizer/NodeVisitingOptimizer.hpp"
#include "../optimizer/QueryPlanOptimizer.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/functions/FunctionEmpty.hpp>
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#include <sstream>
#include <math.h> // for ceil()

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

NodePredicateFilterQP::NodePredicateFilterQP(QueryPlan *arg, QueryPlan *pred, const XMLCh *uri,
	const XMLCh *name, u_int32_t flags, XPath2MemoryManager *mm)
	: FilterQP(NODE_PREDICATE_FILTER, arg, flags, mm),
	  pred_(pred),
	  uri_(uri),
	  name_(name),
	  varSrc_(mm)
{
}

NodeIterator *NodePredicateFilterQP::createNodeIterator(DynamicContext *context) const
{
	if(name_ == 0)
		return new NodePredicateFilter(arg_->createNodeIterator(context), pred_, this);

	return new VarNodePredicateFilter(arg_->createNodeIterator(context), pred_, uri_, name_, this);
}

QueryPlan *NodePredicateFilterQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	NodePredicateFilterQP *result = new (mm) NodePredicateFilterQP(arg_->copy(mm), pred_->copy(mm),
		mm->getPooledString(uri_), mm->getPooledString(name_), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void NodePredicateFilterQP::release()
{
	arg_->release();
	pred_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *NodePredicateFilterQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	VariableTypeStore* varStore=context->getVariableTypeStore();

	_src.clear();

	arg_ = arg_->staticTyping(context, styper);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	varSrc_.getStaticType() = arg_->getStaticAnalysis().getStaticType();
	varSrc_.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
		StaticAnalysis::ONENODE | StaticAnalysis::SELF);

	StaticType oldContextItemType = context->getContextItemType();
	if(name_ == 0) {
		context->setContextItemType(varSrc_.getStaticType());
	} else {
		varStore->addLogicalBlockScope();
		varStore->declareVar(uri_, name_, varSrc_);
	}

	pred_ = pred_->staticTyping(context, styper);

	StaticAnalysis newSrc(context->getMemoryManager());
	if(name_ == 0) {
		newSrc.addExceptContextFlags(pred_->getStaticAnalysis());
		context->setContextItemType(oldContextItemType);
	} else {
		newSrc.add(pred_->getStaticAnalysis());
		newSrc.removeVariable(uri_, name_);
		varStore->removeScope();
	}

	_src.add(newSrc);
	return this;
}

void NodePredicateFilterQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	pred_->staticTypingLite(context);

	StaticAnalysis newSrc(context->getMemoryManager());
	if(name_ == 0) {
		newSrc.addExceptContextFlags(pred_->getStaticAnalysis());
	} else {
		newSrc.add(pred_->getStaticAnalysis());
		newSrc.removeVariable(uri_, name_);
	}

	_src.add(newSrc);
}

class ContextNodeAndVarReplacer : private NodeVisitingOptimizer
{
public:
	ContextNodeAndVarReplacer(const XMLCh *uri, const XMLCh *name)
		: NodeVisitingOptimizer(0),
		  uri_(uri),
		  name_(name),
		  buffer_(0),
		  mm_(0)
	{}

	virtual bool test(QueryPlan *qp) {
		buffer_ = 0;
		mm_ = 0;
		failed_ = false;

		NodeVisitingOptimizer::optimizeQP(qp);
		return !failed_;
	}

	virtual QueryPlan *run(QueryPlan *qp, BufferQP *buf, XPath2MemoryManager *mm) {
		buffer_ = buf;
		mm_ = mm;
		failed_ = false;

		return NodeVisitingOptimizer::optimizeQP(qp);
	}

private:
	virtual ASTNode *optimize(ASTNode *item)
	{
		if(name_ == 0) {
			if(item->getStaticAnalysis().areContextFlagsUsed())
				failed_ = true;
		} else {
			if(item->getStaticAnalysis().isVariableUsed(uri_, name_))
				failed_ = true;
		}

		return item;
	}

	virtual QueryPlan *optimizePredicateFilter(PredicateFilterQP *item)
	{
		item->setArg(optimizeQP(item->getArg()));
		// Skip item->getPred() if it uses the same variable
		if(!XPath2Utils::equals(name_, item->getName()) ||
			!XPath2Utils::equals(uri_, item->getURI())) {
			item->setPred(optimize(item->getPred()));
		}
		return item;
	}

	virtual QueryPlan *optimizeNodePredicateFilter(NodePredicateFilterQP *item)
	{
		item->setArg(optimizeQP(item->getArg()));
		// Skip item->getPred() if it uses the same variable
		if(!XPath2Utils::equals(name_, item->getName()) ||
			!XPath2Utils::equals(uri_, item->getURI())) {
			item->setPred(optimizeQP(item->getPred()));
		}
		return item;
	}

	virtual QueryPlan *optimizeNegativeNodePredicateFilter(NegativeNodePredicateFilterQP *item)
	{
		item->setArg(optimizeQP(item->getArg()));
		// Skip item->getPred() if it uses the same variable
		if(!XPath2Utils::equals(name_, item->getName()) ||
			!XPath2Utils::equals(uri_, item->getURI())) {
			item->setPred(optimizeQP(item->getPred()));
		}
		return item;
	}

	virtual QueryPlan *optimizeContextNode(ContextNodeQP *item)
	{
		if(mm_ != 0 && name_ == 0) {
			BufferReferenceQP *result = new (mm_) BufferReferenceQP(buffer_, 0, mm_);
			result->setBuffer(buffer_);
			result->setLocationInfo(item);
			return result;
		}

		return item;
	}

	virtual QueryPlan *optimizeVariableQP(VariableQP *item)
	{
		if(mm_ != 0 && XPath2Utils::equals(name_, item->getName()) &&
			XPath2Utils::equals(uri_, item->getURI())) {
			BufferReferenceQP *result = new (mm_) BufferReferenceQP(buffer_, 0, mm_);
			result->setBuffer(buffer_);
			result->setLocationInfo(item);
			return result;
		}

		return item;
	}

	virtual void resetInternal() {}

	const XMLCh *uri_;
	const XMLCh *name_;

	BufferQP *buffer_;
	XPath2MemoryManager *mm_;

	bool failed_;
};

class AddStructuralJoinFlag : private FilterSkipper
{
public:
	void run(QueryPlan *pred)
	{
		skip(pred);
	}

private:
	virtual QueryPlan *skipStructuralJoin(StructuralJoinQP *sj)
	{
		sj->addFlag(QueryPlan::SKIP_LEFT_TO_PREDICATE);
		return sj;
	}
};

QueryPlan *NodePredicateFilterQP::optimize(OptimizationContext &opt)
{
	// Optimize the argument
	arg_ = arg_->optimize(opt);

	// Optimize the predicate, skiping optimisations which would introduce another predicate
	pred_ = pred_->optimize(opt);

	AddStructuralJoinFlag().run(pred_);

	return this;
}

void NodePredicateFilterQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans argAltArgs;
	arg_->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, maxAlternatives, opt, argAltArgs);

	QueryPlans predAltArgs;
	pred_->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, maxAlternatives, opt, predAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		for(QueryPlans::iterator it2 = predAltArgs.begin(); it2 != predAltArgs.end(); ++it2) {
			NodePredicateFilterQP *newPred = new (mm)NodePredicateFilterQP((*it)->copy(mm),
				(*it2)->copy(mm), uri_, name_, flags_, mm);
			newPred->setLocationInfo(this);

			combinations.push_back(newPred);
		}
	}

	// Release the alternative arguments, since they've been copied
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		(*it)->release();
	}
	for(it = predAltArgs.begin(); it != predAltArgs.end(); ++it) {
		(*it)->release();
	}
}

void NodePredicateFilterQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Dissolve useless predicates
	if(pred_->getType() == VARIABLE) {
		VariableQP *var = (VariableQP*)pred_;
		if(XPath2Utils::equals(name_, var->getName()) && XPath2Utils::equals(uri_, var->getURI())) {
			alternatives.push_back(arg_);
			return;
		}
	}

	// Dissolve predicates in our predicate expression
	if(pred_->getType() == NODE_PREDICATE_FILTER) {
		NodePredicateFilterQP *npf = (NodePredicateFilterQP*)pred_;

		ContextNodeAndVarReplacer replacer(npf->getURI(), npf->getName());
		if((npf->getFlags() & QueryPlan::SKIP_NESTED_PREDICATES) == 0 &&
			replacer.test(npf->getPred())) {

			string before = logBefore(this);

			unsigned int bufferId = GET_CONFIGURATION(opt.getContext())->allocateBufferID();
			BufferQP *buffer = new (mm) BufferQP(npf->getArg()->copy(mm), 0, bufferId, 0, mm);
			buffer->setLocationInfo(npf->getArg());

			buffer->setArg(replacer.run(npf->getPred()->copy(mm), buffer, mm));

			pred_ = buffer;
			logTransformation(opt.getLog(), "Nested predicates", before, this);
			pred_ = npf;

			QueryPlans bufferAlts;
			buffer->applyConversionRules(maxAlternatives, opt, bufferAlts);

			for(QueryPlans::iterator it = bufferAlts.begin(); it != bufferAlts.end(); ++it) {
				NodePredicateFilterQP *result = new (mm) NodePredicateFilterQP(arg_->copy(mm), *it, uri_, name_, flags_, mm);
				result->setLocationInfo(this);
				result->applyConversionRules(maxAlternatives, opt, alternatives);
			}

			release();
			return;
		}
	}

	alternatives.push_back(this);

	{
		AutoRelease<QueryPlan> result(ReverseJoin().run(this, opt, mm));
		if(result.get() != 0) result->createAlternatives(maxAlternatives, opt, alternatives);
	}

	addFlag(SKIP_REVERSE_JOIN);
}

Cost NodePredicateFilterQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost cost = arg_->cost(context, qec);
	Cost predCost = pred_->cost(context, qec);

	// The predicate expression is run once for every argument key
// 	cost.pages += ceil(cost.keys * predCost.pages);
	cost.pagesForKeys += cost.keys * predCost.totalPages();

	// Take a token key away from the result, because it is likely to
	// return less nodes than the argument - we just don't know how many less.
	if(cost.keys > 1) cost.keys -= 1;

	// Add a token page to the result, because having a predicate takes more
	// time than not
	cost.pagesOverhead += 1;

	return cost;
}

bool NodePredicateFilterQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == NODE_PREDICATE_FILTER) {
		NodePredicateFilterQP *npf = (NodePredicateFilterQP*)o;

		return arg_->isSubsetOf(npf->arg_) && pred_->isSubsetOf(npf->pred_);
	}

	return arg_->isSubsetOf(o);
}

string NodePredicateFilterQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<NodePredicateFilterQP";
	if(name_ != 0) {
		s << " uri=\"" << XMLChToUTF8(uri_).str() << "\"";
		s << " name=\"" << XMLChToUTF8(name_).str() << "\"";
	}
	s << ">" << endl;
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << pred_->printQueryPlan(context, indent + INDENT);
	s << in << "</NodePredicateFilterQP>" << endl;

	return s.str();
}

string NodePredicateFilterQP::toString(bool brief) const
{
	ostringstream s;

	s << "NPF(";
	if(name_ != 0) {
		if(uri_ != 0) {
			s << "{" << XMLChToUTF8(uri_).str() << "}";
		}
		s << XMLChToUTF8(name_).str() << ",";
	}
	s << arg_->toString(brief) << ",";
	s << pred_->toString(brief) << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NegativeNodePredicateFilterQP::NegativeNodePredicateFilterQP(QueryPlan *arg, QueryPlan *pred, const XMLCh *uri,
	const XMLCh *name, u_int32_t flags, XPath2MemoryManager *mm)
	: FilterQP(NEGATIVE_NODE_PREDICATE_FILTER, arg, flags, mm),
	  pred_(pred),
	  uri_(uri),
	  name_(name),
	  varSrc_(mm)
{
}

NodeIterator *NegativeNodePredicateFilterQP::createNodeIterator(DynamicContext *context) const
{
	if(name_ == 0)
		return new NegativeNodePredicateFilter(arg_->createNodeIterator(context), pred_, this);

	return new VarNegativeNodePredicateFilter(arg_->createNodeIterator(context), pred_, uri_, name_, this);
}

QueryPlan *NegativeNodePredicateFilterQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	NegativeNodePredicateFilterQP *result = new (mm) NegativeNodePredicateFilterQP(arg_->copy(mm), pred_->copy(mm),
		mm->getPooledString(uri_), mm->getPooledString(name_), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void NegativeNodePredicateFilterQP::release()
{
	arg_->release();
	pred_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *NegativeNodePredicateFilterQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	VariableTypeStore* varStore=context->getVariableTypeStore();

	_src.clear();

	arg_ = arg_->staticTyping(context, styper);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	varSrc_.getStaticType() = arg_->getStaticAnalysis().getStaticType();
	varSrc_.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::PEER | StaticAnalysis::SUBTREE | StaticAnalysis::SAMEDOC |
		StaticAnalysis::ONENODE | StaticAnalysis::SELF);

	StaticType oldContextItemType = context->getContextItemType();
	if(name_ == 0) {
		context->setContextItemType(varSrc_.getStaticType());
	} else {
		varStore->addLogicalBlockScope();
		varStore->declareVar(uri_, name_, varSrc_);
	}

	pred_ = pred_->staticTyping(context, styper);

	StaticAnalysis newSrc(context->getMemoryManager());
	if(name_ == 0) {
		newSrc.addExceptContextFlags(pred_->getStaticAnalysis());
		context->setContextItemType(oldContextItemType);
	} else {
		newSrc.add(pred_->getStaticAnalysis());
		newSrc.removeVariable(uri_, name_);
		varStore->removeScope();
	}

	_src.add(newSrc);
	return this;
}

void NegativeNodePredicateFilterQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	pred_->staticTypingLite(context);

	StaticAnalysis newSrc(context->getMemoryManager());
	if(name_ == 0) {
		newSrc.addExceptContextFlags(pred_->getStaticAnalysis());
	} else {
		newSrc.add(pred_->getStaticAnalysis());
		newSrc.removeVariable(uri_, name_);
	}

	_src.add(newSrc);
}

QueryPlan *NegativeNodePredicateFilterQP::optimize(OptimizationContext &opt)
{
	// Optimize the argument
	arg_ = arg_->optimize(opt);

	// Optimize the predicate, skiping optimisations which would introduce another predicate
	pred_ = pred_->optimize(opt);

	return this;
}

void NegativeNodePredicateFilterQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans argAltArgs;
	arg_->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, maxAlternatives, opt, argAltArgs);

	QueryPlans predAltArgs;
	pred_->createReducedAlternatives(ARGUMENT_CUTOFF_FACTOR, maxAlternatives, opt, predAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		for(QueryPlans::iterator it2 = predAltArgs.begin(); it2 != predAltArgs.end(); ++it2) {
			NegativeNodePredicateFilterQP *newPred = new (mm) NegativeNodePredicateFilterQP((*it)->copy(mm),
				(*it2)->copy(mm), uri_, name_, flags_, mm);
			newPred->setLocationInfo(this);

			combinations.push_back(newPred);
		}
	}

	// Release the alternative arguments, since they've been copied
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		(*it)->release();
	}
	for(it = predAltArgs.begin(); it != predAltArgs.end(); ++it) {
		(*it)->release();
	}
}

void NegativeNodePredicateFilterQP::applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Dissolve useless predicates
	if(pred_->getType() == VARIABLE) {
		VariableQP *var = (VariableQP*)pred_;
		if(XPath2Utils::equals(name_, var->getName()) &&
			XPath2Utils::equals(uri_, var->getURI())) {

			EmptyQP *empty = new (mm) EmptyQP(0, mm);
			empty->setLocationInfo(this);
			alternatives.push_back(empty);
			return;
		}
	}

	// Dissolve predicates in our predicate expression
	if(pred_->getType() == NODE_PREDICATE_FILTER) {
		NodePredicateFilterQP *npf = (NodePredicateFilterQP*)pred_;

		ContextNodeAndVarReplacer replacer(npf->getURI(), npf->getName());
		if((npf->getFlags() & QueryPlan::SKIP_NESTED_PREDICATES) == 0 &&
			replacer.test(npf->getPred())) {

			string before = logBefore(this);

			unsigned int bufferId = GET_CONFIGURATION(opt.getContext())->allocateBufferID();
			BufferQP *buffer = new (mm) BufferQP(npf->getArg()->copy(mm), 0, bufferId, 0, mm);
			buffer->setLocationInfo(npf->getArg());

			buffer->setArg(replacer.run(npf->getPred()->copy(mm), buffer, mm));

			pred_ = buffer;
			logTransformation(opt.getLog(), "Nested predicates", before, this);
			pred_ = npf;

			QueryPlans bufferAlts;
			buffer->applyConversionRules(maxAlternatives, opt, bufferAlts);

			for(QueryPlans::iterator it = bufferAlts.begin(); it != bufferAlts.end(); ++it) {
				NegativeNodePredicateFilterQP *result = new (mm) NegativeNodePredicateFilterQP(arg_->copy(mm), *it, uri_, name_, flags_, mm);
				result->setLocationInfo(this);
				result->applyConversionRules(maxAlternatives, opt, alternatives);
			}

			release();
			return;
		}
	}

	alternatives.push_back(this);

	{
		AutoRelease<QueryPlan> result(ReverseJoin().run(this, opt, mm));
		if(result.get() != 0) result->createAlternatives(maxAlternatives, opt, alternatives);
	}

	addFlag(SKIP_REVERSE_JOIN);
}

Cost NegativeNodePredicateFilterQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost cost = arg_->cost(context, qec);
	Cost predCost = pred_->cost(context, qec);

	// The predicate expression is run once for every argument key
// 	cost.pages += ceil(cost.keys * predCost.pages);
	cost.pagesForKeys += cost.keys * predCost.totalPages();

	// Take a token key away from the result, because it is likely to
	// return less nodes than the argument - we just don't know how many less.
	if(cost.keys > 1) cost.keys -= 1;

	// Add a token page to the result, because having a predicate takes more
	// time than not
	cost.pagesOverhead += 1;

	return cost;
}

bool NegativeNodePredicateFilterQP::isSubsetOf(const QueryPlan *o) const
{
	if(o->getType() == NEGATIVE_NODE_PREDICATE_FILTER) {
		NegativeNodePredicateFilterQP *npf = (NegativeNodePredicateFilterQP*)o;

		return arg_->isSubsetOf(npf->arg_) && pred_->isSubsetOf(npf->pred_);
	}

	return arg_->isSubsetOf(o);
}

string NegativeNodePredicateFilterQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<NegativeNodePredicateFilterQP";
	if(name_ != 0) {
		s << " uri=\"" << XMLChToUTF8(uri_).str() << "\"";
		s << " name=\"" << XMLChToUTF8(name_).str() << "\"";
	}
	s << ">" << endl;
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << pred_->printQueryPlan(context, indent + INDENT);
	s << in << "</NegativeNodePredicateFilterQP>" << endl;

	return s.str();
}

string NegativeNodePredicateFilterQP::toString(bool brief) const
{
	ostringstream s;

	s << "NNPF(";
	if(name_ != 0) {
		if(uri_ != 0) {
			s << "{" << XMLChToUTF8(uri_).str() << "}";
		}
		s << XMLChToUTF8(name_).str() << ",";
	}
	s << arg_->toString(brief) << ",";
	s << pred_->toString(brief) << ")";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NodePredicateFilter::NodePredicateFilter(NodeIterator *parent, const QueryPlan *pred, const LocationInfo *location)
	: DbXmlNodeIterator(location),
	  parent_(parent),
	  pred_(pred),
	  toDo_(true)
{
}

NodePredicateFilter::~NodePredicateFilter()
{
	delete parent_;
}

bool NodePredicateFilter::next(DynamicContext *context)
{
	if(!parent_->next(context)) return false;
	return doNext(context);
}

bool NodePredicateFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!parent_->seek(container, did, nid, context)) return false;
	return doNext(context);
}

bool NodePredicateFilter::doNext(DynamicContext *context)
{
	AutoContextInfoReset autoReset(context);

	bool contextUsed = pred_->getStaticAnalysis().isContextItemUsed() ||
		pred_->getStaticAnalysis().isContextPositionUsed();

	while(true) {
		context->testInterrupt();

		node_ = parent_->asDbXmlNode(context);

		if(contextUsed || toDo_) {
			toDo_ = false;
			
			context->setContextItem(node_);

			// 2) Otherwise, the predicate truth value is the effective boolean value of the
			// predicate expression
			AutoDelete<NodeIterator> predResult(pred_->createNodeIterator(context));
			if(predResult->next(context)) {
				return true;
			} else if(!contextUsed) {
				return false;
			}

			autoReset.resetContextInfo();
		} else {
			return true;
		}

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VarNodePredicateFilter::VarNodePredicateFilter(NodeIterator *parent, const QueryPlan *pred, const XMLCh *uri,
	const XMLCh *name, const LocationInfo *location)
	: DbXmlNodeIterator(location),
	  parent_(parent),
	  pred_(pred),
	  uri_(uri),
	  name_(name),
	  scope_(0)
{
}

VarNodePredicateFilter::~VarNodePredicateFilter()
{
	delete parent_;
}

Result VarNodePredicateFilter::getVar(const XMLCh *namespaceURI, const XMLCh *name) const
{
	if(XPath2Utils::equals(name, name_) && XPath2Utils::equals(namespaceURI, uri_))
		return (Item::Ptr)node_.get();

	return scope_->getVar(namespaceURI, name);
}

void VarNodePredicateFilter::getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const
{
	variables.push_back(std::pair<const XMLCh*, const XMLCh*>(uri_, name_));
	scope_->getInScopeVariables(variables);
}

bool VarNodePredicateFilter::next(DynamicContext *context)
{
	if(!parent_->next(context)) return false;
	return doNext(context);
}

bool VarNodePredicateFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!parent_->seek(container, did, nid, context)) return false;
	return doNext(context);
}

bool VarNodePredicateFilter::doNext(DynamicContext *context)
{
	if(scope_ == 0) {
		scope_ = context->getVariableStore();
	}

	AutoVariableStoreReset reset(context);

	while(true) {
		context->testInterrupt();

		node_ = parent_->asDbXmlNode(context);

		// 2) Otherwise, the predicate truth value is the effective boolean value of the
		// predicate expression
		context->setVariableStore(this);
		{
			AutoDelete<NodeIterator> predResult(pred_->createNodeIterator(context));
			if(predResult->next(context)) {
				return true;
			}
		}

		context->setVariableStore(scope_);
		if(!parent_->next(context)) break;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NegativeNodePredicateFilter::NegativeNodePredicateFilter(NodeIterator *parent, const QueryPlan *pred, const LocationInfo *location)
	: DbXmlNodeIterator(location),
	  parent_(parent),
	  pred_(pred),
	  toDo_(true)
{
}

NegativeNodePredicateFilter::~NegativeNodePredicateFilter()
{
	delete parent_;
}

bool NegativeNodePredicateFilter::next(DynamicContext *context)
{
	if(!parent_->next(context)) return false;
	return doNext(context);
}

bool NegativeNodePredicateFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!parent_->seek(container, did, nid, context)) return false;
	return doNext(context);
}

bool NegativeNodePredicateFilter::doNext(DynamicContext *context)
{
	AutoContextInfoReset autoReset(context);

	bool contextUsed = pred_->getStaticAnalysis().isContextItemUsed() ||
		pred_->getStaticAnalysis().isContextPositionUsed();

	while(true) {
		context->testInterrupt();

		node_ = parent_->asDbXmlNode(context);

		if(contextUsed || toDo_) {
			toDo_ = false;
			
			context->setContextItem(node_);

			// 2) Otherwise, the predicate truth value is the effective boolean value of the
			// predicate expression
			AutoDelete<NodeIterator> predResult(pred_->createNodeIterator(context));
			if(!predResult->next(context)) {
				return true;
			} else if(!contextUsed) {
				return false;
			}

			autoReset.resetContextInfo();
		} else {
			return true;
		}

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VarNegativeNodePredicateFilter::VarNegativeNodePredicateFilter(NodeIterator *parent, const QueryPlan *pred, const XMLCh *uri,
	const XMLCh *name, const LocationInfo *location)
	: DbXmlNodeIterator(location),
	  parent_(parent),
	  pred_(pred),
	  uri_(uri),
	  name_(name),
	  scope_(0)
{
}

VarNegativeNodePredicateFilter::~VarNegativeNodePredicateFilter()
{
	delete parent_;
}

Result VarNegativeNodePredicateFilter::getVar(const XMLCh *namespaceURI, const XMLCh *name) const
{
	if(XPath2Utils::equals(name, name_) && XPath2Utils::equals(namespaceURI, uri_))
		return (Item::Ptr)node_.get();

	return scope_->getVar(namespaceURI, name);
}

void VarNegativeNodePredicateFilter::getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const
{
	variables.push_back(std::pair<const XMLCh*, const XMLCh*>(uri_, name_));
	scope_->getInScopeVariables(variables);
}

bool VarNegativeNodePredicateFilter::next(DynamicContext *context)
{
	if(!parent_->next(context)) return false;
	return doNext(context);
}

bool VarNegativeNodePredicateFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!parent_->seek(container, did, nid, context)) return false;
	return doNext(context);
}

bool VarNegativeNodePredicateFilter::doNext(DynamicContext *context)
{
	if(scope_ == 0) {
		scope_ = context->getVariableStore();
	}

	AutoVariableStoreReset reset(context);

	while(true) {
		context->testInterrupt();

		node_ = parent_->asDbXmlNode(context);

		// 2) Otherwise, the predicate truth value is the effective boolean value of the
		// predicate expression
		context->setVariableStore(this);
		{
			AutoDelete<NodeIterator> predResult(pred_->createNodeIterator(context));
			if(!predResult->next(context)) {
				return true;
			}
		}

		context->setVariableStore(scope_);
		if(!parent_->next(context)) break;
	}

	return false;
}

