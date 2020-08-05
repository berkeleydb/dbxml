//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "DbXmlPredicate.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/runtime/SequenceResult.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/ast/XQEffectiveBooleanValue.hpp>

using namespace DbXml;
using namespace std;

DbXmlPredicate::DbXmlPredicate(ASTNode *expr, ASTNode *predicate, const XMLCh *uri, const XMLCh *name, XPath2MemoryManager *memMgr)
	: DbXmlASTNode(DBXML_PREDICATE, memMgr),
	  expr_(expr),
	  pred_(predicate),
	  uri_(uri),
	  name_(name),
	  varSrc_(memMgr)
{
	_src.copy(expr_->getStaticAnalysis());

	if(name_ == 0) {
		_src.addExceptContextFlags(pred_->getStaticAnalysis());
	} else {
		StaticAnalysis newSrc(memMgr);
		newSrc.add(pred_->getStaticAnalysis());
		newSrc.removeVariable(uri_, name_);
		_src.add(newSrc);
	}
}

ASTNode *DbXmlPredicate::staticTypingImpl(StaticContext *context)
{
	_src.clear();

	_src.copy(expr_->getStaticAnalysis());
	_src.getStaticType().multiply(0, 1);

	const StaticAnalysis &predSrc = pred_->getStaticAnalysis();

	if(name_ == 0) {
		_src.addExceptContextFlags(predSrc);
	} else {
		_src.addExceptVariable(uri_, name_, predSrc);
	}

	return this;
}

Result DbXmlPredicate::createResult(DynamicContext* context, int flags) const
{
	Result result = expr_->createResult(context);

	if(name_ != 0) {
		return new VarPredicateResult(result, pred_, uri_, name_);
	}

	return new PredicateFilterResult(result, pred_, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VarPredicateResult::VarPredicateResult(const Result &parent, const ASTNode *pred, const XMLCh *uri, const XMLCh *name)
	: ResultImpl(pred),
	  parent_(parent),
	  pred_(pred),
	  uri_(uri),
	  name_(name),
	  item_(0),
	  scope_(0)
{
}

Result VarPredicateResult::getVar(const XMLCh *namespaceURI, const XMLCh *name) const
{
	if(XPath2Utils::equals(name, name_) && XPath2Utils::equals(namespaceURI, uri_))
		return item_;

	return scope_->getVar(namespaceURI, name);
}

void VarPredicateResult::getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const
{
	variables.push_back(std::pair<const XMLCh*, const XMLCh*>(uri_, name_));
	scope_->getInScopeVariables(variables);
}

Item::Ptr VarPredicateResult::next(DynamicContext *context)
{
	if(scope_ == 0) {
		scope_ = context->getVariableStore();
	}

	AutoVariableStoreReset reset(context, scope_);

	while((item_ = parent_->next(context)).notNull()) {
		context->testInterrupt();

		context->setVariableStore(this);
		Result predResult = new EffectiveBooleanValueResult(this, pred_->createResult(context));
		if(((ATBooleanOrDerived*)predResult->next(context).get())->isTrue())
			break;

		context->setVariableStore(scope_);
	}

	return item_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlNodeCheck::DbXmlNodeCheck(ASTNode *arg, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(NODE_CHECK, memMgr),
	  arg_(arg)
{
	_src.copy(arg_->getStaticAnalysis());
	_src.getStaticType().typeIntersect(StaticType::NODE_TYPE);
}

ASTNode *DbXmlNodeCheck::staticTypingImpl(StaticContext *context)
{
	_src.clear();

	_src.copy(arg_->getStaticAnalysis());

	if(_src.getStaticType().isType(StaticType::NODE_TYPE)) {
		return arg_;
	}

	_src.getStaticType().typeIntersect(StaticType::NODE_TYPE);

	return this;
}

Result DbXmlNodeCheck::createResult(DynamicContext* context, int flags) const
{
	return new IntermediateStepCheckResult(this, arg_->createResult(context));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DbXmlLastStepCheck::DbXmlLastStepCheck(ASTNode *arg, XPath2MemoryManager* memMgr)
	: DbXmlASTNode(LAST_STEP_CHECK, memMgr),
	  arg_(arg)
{
	_src.copy(arg_->getStaticAnalysis());
}

ASTNode *DbXmlLastStepCheck::staticTypingImpl(StaticContext *context)
{
	_src.clear();

	_src.copy(arg_->getStaticAnalysis());

	if(!_src.getStaticType().containsType(StaticType::NODE_TYPE) ||
		!_src.getStaticType().containsType(StaticType::ANY_ATOMIC_TYPE)) {
		return arg_;
	}

	return this;
}

Result DbXmlLastStepCheck::createResult(DynamicContext* context, int flags) const
{
	return new LastStepCheckResult(this, arg_->createResult(context));
}

