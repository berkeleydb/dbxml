//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "PredicateFilterQP.hpp"
#include "NodePredicateFilterQP.hpp"
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
#include <xqilla/ast/XQEffectiveBooleanValue.hpp>

#include <sstream>
#include <math.h> // for ceil()

using namespace DbXml;
using namespace std;

XERCES_CPP_NAMESPACE_USE;

static const int INDENT = 1;

PredicateFilterQP::PredicateFilterQP(QueryPlan *arg, ASTNode *pred, const XMLCh *uri, const XMLCh *name,
	u_int32_t flags, XPath2MemoryManager *mm)
	: FilterQP(PREDICATE_FILTER, arg, flags, mm),
	  pred_(pred),
	  uri_(uri),
	  name_(name),
	  varSrc_(mm)
{
}

NodeIterator *PredicateFilterQP::createNodeIterator(DynamicContext *context) const
{
	if(name_ == 0)
		return new PredicateFilter(arg_->createNodeIterator(context), pred_, this);

	return new VarPredicateFilter(arg_->createNodeIterator(context), pred_, uri_, name_, this);
}

QueryPlan *PredicateFilterQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	PredicateFilterQP *result = new (mm) PredicateFilterQP(arg_->copy(mm), pred_,
		mm->getPooledString(uri_), mm->getPooledString(name_), flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void PredicateFilterQP::release()
{
	arg_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *PredicateFilterQP::staticTyping(StaticContext *context, StaticTyper *styper)
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

void PredicateFilterQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	StaticAnalysis newSrc(context->getMemoryManager());
	if(name_ == 0) {
		newSrc.addExceptContextFlags(pred_->getStaticAnalysis());
	} else {
		newSrc.add(pred_->getStaticAnalysis());
		newSrc.removeVariable(uri_,name_);
	}

	_src.add(newSrc);
}

QueryPlan *PredicateFilterQP::optimize(OptimizationContext &opt)
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	switch((int)pred_->getType()) {
	case ((int)DbXmlASTNode::QP_TO_AST): {
		QueryPlan *result = new (mm) NodePredicateFilterQP(arg_, ((QueryPlanToAST*)pred_)->getQueryPlan(),
			uri_, name_, flags_, mm);
		result->setLocationInfo(this);
		return result->optimize(opt);
	}
	case ((int)ASTNode::FUNCTION): {
		XQFunction *item = (XQFunction*)pred_;
		VectorOfASTNodes &args = const_cast<VectorOfASTNodes &>(item->getArguments());
		const XMLCh *uri = item->getFunctionURI();
		const XMLCh *name = item->getFunctionName();

		if(uri == XQFunction::XMLChFunctionURI) {
			if(name == FunctionNot::name || name == FunctionEmpty::name) {
				if(args[0]->getType() == (ASTNode::whichType)DbXmlASTNode::QP_TO_AST) {
					QueryPlan *result = new (mm) NegativeNodePredicateFilterQP(arg_,
						((QueryPlanToAST*)args[0])->getQueryPlan(), uri_, name_, flags_, mm);
					result->setLocationInfo(this);
					return result->optimize(opt);
				}
			}
			else if(name == FunctionExists::name) {
				if(args[0]->getType() == (ASTNode::whichType)DbXmlASTNode::QP_TO_AST) {
					QueryPlan *result = new (mm) NodePredicateFilterQP(arg_,
						((QueryPlanToAST*)args[0])->getQueryPlan(), uri_, name_, flags_, mm);
					result->setLocationInfo(this);
					return result->optimize(opt);
				}
			}
		}

		break;
	}
	default: break;
	}

	// Optimize the argument
	arg_ = arg_->optimize(opt);

	if(opt.getQueryPlanOptimizer())
		pred_ = opt.getQueryPlanOptimizer()->optimize(pred_);

	return this;
}

static const double predCostPerKey = 1;

void PredicateFilterQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans argAltArgs;
	arg_->createAlternatives(maxAlternatives, opt, argAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		PredicateFilterQP *newPred = new (mm)
			PredicateFilterQP(*it, pred_, uri_, name_, flags_, mm);
		newPred->setLocationInfo(this);

		combinations.push_back(newPred);
	}
}

Cost PredicateFilterQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost cost = arg_->cost(context, qec);

	// The predicate expression costs some amount per input key - we don't know how much so
	// just guess
// 	cost.pages += ceil(cost.keys * predCostPerKey);
	cost.pagesForKeys += cost.keys * predCostPerKey;

	// Take a token key away from the result, because it is likely to
	// return less nodes than the argument - we just don't know how many less.
	if(cost.keys > 1) cost.keys -= 1;

	return cost;
}

string PredicateFilterQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<PredicateFilterQP";
	if(name_ != 0) {
		s << " uri=\"" << XMLChToUTF8(uri_).str() << "\"";
		s << " name=\"" << XMLChToUTF8(name_).str() << "\"";
	}
	s << ">" << endl;
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << DbXmlPrintAST::print(pred_, context, indent + INDENT);
	s << in << "</PredicateFilterQP>" << endl;

	return s.str();
}

string PredicateFilterQP::toString(bool brief) const
{
	ostringstream s;

	s << "PF(";
	if(name_ != 0) {
		if(uri_ != 0) {
			s << "{" << XMLChToUTF8(uri_).str() << "}";
		}
		s << XMLChToUTF8(name_).str() << ",";
	}

	s << arg_->toString(brief);

	s << ",'" << "[to be calculated]" << "')";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NumericPredicateFilterQP::NumericPredicateFilterQP(QueryPlan *arg, ASTNode *pred, bool reverse,
	u_int32_t flags, XPath2MemoryManager *mm)
	: FilterQP(NUMERIC_PREDICATE_FILTER, arg, flags, mm),
	  pred_(pred),
	  reverse_(reverse)
{
}

NodeIterator *NumericPredicateFilterQP::createNodeIterator(DynamicContext *context) const
{
	return new NumericPredicateFilter(arg_->createNodeIterator(context), pred_, reverse_, this);
}

QueryPlan *NumericPredicateFilterQP::copy(XPath2MemoryManager *mm) const
{
	if(!mm) {
		mm = memMgr_;
	}

	NumericPredicateFilterQP *result = new (mm) NumericPredicateFilterQP(arg_->copy(mm), pred_, reverse_, flags_, mm);
	result->setLocationInfo(this);
	return result;
}

void NumericPredicateFilterQP::release()
{
	arg_->release();
	_src.clear();
	memMgr_->deallocate(this);
}

QueryPlan *NumericPredicateFilterQP::staticTyping(StaticContext *context, StaticTyper *styper)
{
	_src.clear();

	arg_ = arg_->staticTyping(context, styper);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	StaticType oldContextItemType = context->getContextItemType();
	context->setContextItemType(arg_->getStaticAnalysis().getStaticType());

	pred_ = pred_->staticTyping(context, styper);
	_src.addExceptContextFlags(pred_->getStaticAnalysis());

	context->setContextItemType(oldContextItemType);

	return this;
}

void NumericPredicateFilterQP::staticTypingLite(StaticContext *context)
{
	_src.clear();

	arg_->staticTypingLite(context);
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	_src.addExceptContextFlags(pred_->getStaticAnalysis());
}

QueryPlan *NumericPredicateFilterQP::optimize(OptimizationContext &opt)
{
	// Optimize the argument
	arg_ = arg_->optimize(opt);

	if(opt.getQueryPlanOptimizer())
		pred_ = opt.getQueryPlanOptimizer()->optimize(pred_);

	return this;
}

void NumericPredicateFilterQP::createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const
{
	XPath2MemoryManager *mm = opt.getMemoryManager();

	// Generate the alternatives for the arguments
	QueryPlans argAltArgs;
	arg_->createAlternatives(maxAlternatives, opt, argAltArgs);

	// Generate the combinations of all the alternatives for the arguments
	QueryPlans::iterator it;
	for(it = argAltArgs.begin(); it != argAltArgs.end(); ++it) {
		NumericPredicateFilterQP *newPred = new (mm)
			NumericPredicateFilterQP(*it, pred_, reverse_, flags_, mm);
		newPred->setLocationInfo(this);

		combinations.push_back(newPred);
	}
}

Cost NumericPredicateFilterQP::cost(OperationContext &context, QueryExecutionContext &qec) const
{
	Cost cost = arg_->cost(context, qec);

	// The predicate expression costs some amount per input key - we don't know how much so
	// just guess
// 	cost.pages += ceil(cost.keys * predCostPerKey);
	cost.pagesForKeys += cost.keys * predCostPerKey;

	// Since this is (most likely) a numeric predicate, it will return at most one key
	if(cost.keys > 1) cost.keys = 1;

	return cost;
}

string NumericPredicateFilterQP::printQueryPlan(const DynamicContext *context, int indent) const
{
	ostringstream s;

	string in(PrintAST::getIndent(indent));

	s << in << "<NumericPredicateFilterQP";
	if(reverse_) {
		s << " reverse=\"true\"";
	}
	s << ">" << endl;
	s << arg_->printQueryPlan(context, indent + INDENT);
	s << DbXmlPrintAST::print(pred_, context, indent + INDENT);
	s << in << "</NumericPredicateFilterQP>" << endl;

	return s.str();
}

string NumericPredicateFilterQP::toString(bool brief) const
{
	ostringstream s;

	if(reverse_) {
		s << "RNPF(";
	} else {
		s << "NuPF(";
	}
	s << arg_->toString(brief);
	s << ",'" << "[to be calculated]" << "')";

	return s.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PredicateFilter::PredicateFilter(NodeIterator *parent, const ASTNode *pred, const LocationInfo *location)
	: DbXmlNodeIterator(location),
	  parent_(parent),
	  pred_(pred),
	  toDo_(true)
{
}

PredicateFilter::~PredicateFilter()
{
	delete parent_;
}

bool PredicateFilter::next(DynamicContext *context)
{
	if(!parent_->next(context)) return false;
	return doNext(context);
}

bool PredicateFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!parent_->seek(container, did, nid, context)) return false;
	return doNext(context);
}

bool PredicateFilter::doNext(DynamicContext *context)
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
			Result predResult = new EffectiveBooleanValueResult(location_, pred_->createResult(context));
			if(((ATBooleanOrDerived*)predResult->next(context).get())->isTrue()) {
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

VarPredicateFilter::VarPredicateFilter(NodeIterator *parent, const ASTNode *pred, const XMLCh *uri, const XMLCh *name, const LocationInfo *location)
	: DbXmlNodeIterator(location),
	  parent_(parent),
	  pred_(pred),
	  uri_(uri),
	  name_(name),
	  scope_(0)
{
}

VarPredicateFilter::~VarPredicateFilter()
{
	delete parent_;
}

Result VarPredicateFilter::getVar(const XMLCh *namespaceURI, const XMLCh *name) const
{
	if(XPath2Utils::equals(name, name_) && XPath2Utils::equals(namespaceURI, uri_))
		return (Item::Ptr)node_.get();

	return scope_->getVar(namespaceURI, name);
}

void VarPredicateFilter::getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const
{
	variables.push_back(std::pair<const XMLCh*, const XMLCh*>(uri_, name_));
	scope_->getInScopeVariables(variables);
}

bool VarPredicateFilter::next(DynamicContext *context)
{
	if(!parent_->next(context)) return false;
	return doNext(context);
}

bool VarPredicateFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(!parent_->seek(container, did, nid, context)) return false;
	return doNext(context);
}

bool VarPredicateFilter::doNext(DynamicContext *context)
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
		Result predResult = new EffectiveBooleanValueResult(location_, pred_->createResult(context));
		if(((ATBooleanOrDerived*)predResult->next(context).get())->isTrue()) {
			return true;
		}

		context->setVariableStore(scope_);
		if(!parent_->next(context)) break;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

NumericPredicateFilter::NumericPredicateFilter(NodeIterator *parent, const ASTNode *pred, bool reverse,
	const LocationInfo *location)
	: DbXmlNodeIterator(location),
	  parent_(parent),
	  pred_(pred),
	  toDo_(true),
	  contextPos_(0),
	  contextSize_(0),
	  reverse_(reverse)
{
}

NumericPredicateFilter::~NumericPredicateFilter()
{
	delete parent_;
}

bool NumericPredicateFilter::next(DynamicContext *context)
{
	if(toDo_ && (reverse_ || pred_->getStaticAnalysis().isContextSizeUsed())) {
		// We need the context size, so convert to a Sequence to work it out
		Result parentRes(new QueryPlanToASTResult(parent_, location_));
		parent_ = 0;

		// TBD Use a DB BTree instead? - jpcs
		Sequence seq(parentRes->toSequence(context));
		contextSize_ = seq.getLength();
		if(reverse_) contextPos_ = contextSize_ + 1;

		parent_ = new ASTToQueryPlanIterator(seq, location_);
	}

	if(parent_ == 0 || !parent_->next(context)) return false;
	return doNext(context);
}

bool NumericPredicateFilter::seek(int container, const DocID &did, const NsNid &nid, DynamicContext *context)
{
	if(toDo_ && (reverse_ || pred_->getStaticAnalysis().isContextSizeUsed())) {
		// We need the context size, so convert to a Sequence to work it out
		Result parentRes(new QueryPlanToASTResult(parent_, location_));
		parent_ = 0;

		// TBD Use a DB BTree instead? - jpcs
		Sequence seq(parentRes->toSequence(context));
		contextSize_ = seq.getLength();
		if(reverse_) contextPos_ = contextSize_ + 1;

		parent_ = new ASTToQueryPlanIterator(seq, location_);
	}

	if(parent_ == 0 || !parent_->seek(container, did, nid, context)) return false;
	return doNext(context);
}

bool NumericPredicateFilter::doNext(DynamicContext *context)
{
	AutoContextInfoReset autoReset(context);

	bool contextUsed = pred_->getStaticAnalysis().isContextItemUsed() ||
		pred_->getStaticAnalysis().isContextPositionUsed();

	while(true) {
		context->testInterrupt();

		if(reverse_) --contextPos_;
		else ++contextPos_;

		node_ = parent_->asDbXmlNode(context);

		if(contextUsed || toDo_) {
			toDo_ = false;
			
			context->setContextSize(contextSize_);
			context->setContextPosition(contextPos_);
			context->setContextItem(node_);

			Result pred_result = pred_->createResult(context);
			first_ = pred_result->next(context);
			if(first_.notNull()) {
				second_ = pred_result->next(context);
			}

			autoReset.resetContextInfo();
		}

		// 3.2.2 ...
		// The predicate truth value is derived by applying the following rules, in order:
		// 1) If the value of the predicate expression is an atomic value of a numeric type, the predicate truth
		// value is true if and only if the value of the predicate expression is equal to the context position.
		if(first_.notNull() && second_.isNull() && first_->isAtomicValue() &&
			((const AnyAtomicType::Ptr)first_)->isNumericValue()) {
			const Numeric *num = (const Numeric*)first_.get();
			int cmp = num->compare(context->getItemFactory()->createInteger((long)contextPos_, context), context);
			if(cmp == 0) {
				if(!contextUsed) {
					delete parent_;
					parent_ = 0;
				}
				return true;
			} else if((cmp > 0) == reverse_  && !contextUsed) {
				return false;
			}
		} else {
			// 2) Otherwise, the predicate truth value is the effective boolean value of the predicate expression
			if(XQEffectiveBooleanValue::get(first_, second_, context, location_)) {
				return true;
			} else if(!contextUsed) {
				return false;
			}
		}

		if(!parent_->next(context)) return false;
	}

	// Never reached
	return false;
}
