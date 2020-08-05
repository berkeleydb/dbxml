/*
 * Copyright (c) 2001-2008
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2008
 *     Oracle. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id: XQTypeswitch.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include <xqilla/ast/XQTypeswitch.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/runtime/ResultBuffer.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/update/PendingUpdateList.hpp>
#include <xqilla/runtime/ClosureResult.hpp>

XQTypeswitch::  XQTypeswitch(ASTNode *expr, Cases *cases, Case *defaultCase, XPath2MemoryManager *mm)
  : ASTNodeImpl(TYPESWITCH, mm),
    expr_(expr),
    cases_(cases),
    default_(defaultCase)
{
}

Result XQTypeswitch::createResult(DynamicContext *context, int flags) const
{
  return new TypeswitchResult(this);
}

ASTNode* XQTypeswitch::staticResolution(StaticContext *context)
{
  // Statically resolve the test expression
  expr_ = expr_->staticResolution(context);

  // Call static resolution on the clauses
  for(Cases::iterator it = cases_->begin(); it != cases_->end(); ++it) {
    (*it)->staticResolution(context);
  }

  default_->staticResolution(context);

  return this;
}

static const XMLCh no_err[] = { 0 };

ASTNode *XQTypeswitch::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  // Statically resolve the test expression
  const StaticAnalysis &exprSrc = expr_->getStaticAnalysis();

  if(exprSrc.isUpdating()) {
    XQThrow(StaticErrorException,X("XQTypeswitch::staticTyping"),
            X("It is a static error for the operand expression of a typeswitch expression "
              "to be an updating expression [err:XUST0001]"));
  }

  // Call static resolution on the clauses
  bool possiblyUpdating = true;
  _src.add(exprSrc);

  default_->staticTyping(expr_->getStaticAnalysis(), context, _src, possiblyUpdating, /*first*/true);

  Cases::iterator it;
  for(it = cases_->begin(); it != cases_->end(); ++it) {
    (*it)->staticTyping(expr_->getStaticAnalysis(), context, _src, possiblyUpdating, /*first*/false);
  }

  if(!context) {
    return this;
  }

  if(exprSrc.isUsed()) {
    // Do basic static type checking on the clauses,
    // to eliminate ones which will never be matches,
    // and find ones which will always be matched.
    const StaticType &sType = expr_->getStaticAnalysis().getStaticType();

    Cases newCases(XQillaAllocator<Case*>(context->getMemoryManager()));
    for(it = cases_->begin(); it != cases_->end(); ++it) {
      bool isExact;
      StaticType cseType;
      (*it)->getSequenceType()->getStaticType(cseType, context, isExact, *it);

      if(isExact && sType.isType(cseType) &&
         sType.getMin() >= cseType.getMin() &&
         sType.getMax() <= cseType.getMax()) {
        // It always matches, so set this clause as the
        // default clause and remove all the others
        newCases.clear();
        default_ = *it;
        default_->setSequenceType(0);
        break;
      }
      else if((cseType.getMin() > sType.getMax()) ||
              (cseType.getMax() < sType.getMin()) ||
              (!sType.containsType(cseType) &&
               (cseType.getMin() > 0 || sType.getMin() > 0))) {
        // It never matches, so don't add it to the new clauses
      }
      else {
        newCases.push_back(*it);
      }
    }

    if(newCases.size() != cases_->size()) {
      *cases_ = newCases;

      // Call static resolution on the new clauses
      possiblyUpdating = true;
      _src.clear();
      _src.add(exprSrc);

      default_->staticTyping(expr_->getStaticAnalysis(), context, _src, possiblyUpdating, /*first*/true);

      for(it = cases_->begin(); it != cases_->end(); ++it) {
        (*it)->staticTyping(expr_->getStaticAnalysis(), context, _src, possiblyUpdating, /*first*/false);
      }
    }

    return this;
  }
  else {
    // If it's constant, we can narrow it down to the correct clause
    AutoDelete<DynamicContext> dContext(context->createDynamicContext());
    dContext->setMemoryManager(context->getMemoryManager());
    Sequence value = expr_->createResult(dContext)->toSequence(dContext);

    Case *match = 0;
    for(Cases::iterator it = cases_->begin(); it != cases_->end(); ++it) {
      try {
        (*it)->getSequenceType()->matches(value, (*it)->getSequenceType(), no_err)->toSequence(dContext);
        match = *it;
        break;
      }
      catch(const XPath2TypeMatchException &ex) {
        // Well, it doesn't match that one then...
      }
    }

    // Replace the default with the matched clause and
    // remove the remaining clauses, as they don't match
    if(match) {
      default_ = match;
      default_->setSequenceType(0);
    }
    cases_->clear();

    // Statically resolve the default clause
    possiblyUpdating = true;
    _src.clear();
    default_->staticTyping(expr_->getStaticAnalysis(), context, _src, possiblyUpdating, /*first*/true);

    return this;
  }
}

XQTypeswitch::Case::Case(const XMLCh *qname, SequenceType *seqType, ASTNode *expr)
  : qname_(qname),
    uri_(0),
    name_(0),
    seqType_(seqType),
    expr_(expr)
{
}

XQTypeswitch::Case::Case(const XMLCh *qname, const XMLCh *uri, const XMLCh *name, SequenceType *seqType, ASTNode *expr)
  : qname_(qname),
    uri_(uri),
    name_(name),
    seqType_(seqType),
    expr_(expr)
{
}

void XQTypeswitch::Case::staticResolution(StaticContext* context)
{
  if(seqType_)
    seqType_->staticResolution(context);
  expr_ = expr_->staticResolution(context);

  if(qname_ != 0) {
    uri_ = context->getUriBoundToPrefix(XPath2NSUtils::getPrefix(qname_, context->getMemoryManager()), this);
    name_ = XPath2NSUtils::getLocalName(qname_);
  }
}

void XQTypeswitch::Case::staticTyping(const StaticAnalysis &var_src, StaticContext* context,
                                      StaticAnalysis &src, bool &possiblyUpdating, bool first)
{
  if(seqType_ != 0) {
    if(src.isUpdating()) {
      if(!expr_->getStaticAnalysis().isUpdating() && !expr_->getStaticAnalysis().isPossiblyUpdating())
        XQThrow(StaticErrorException, X("XQTypeswitch::Case::staticTyping"),
                X("Mixed updating and non-updating operands [err:XUST0001]"));
    }
    else {
      if(expr_->getStaticAnalysis().isUpdating() && !possiblyUpdating)
        XQThrow(StaticErrorException, X("XQTypeswitch::Case::staticTyping"),
                X("Mixed updating and non-updating operands [err:XUST0001]"));
    }
  }

  if(qname_ != 0) {
    // Remove the local variable from the StaticAnalysis
    if(!expr_->getStaticAnalysis().isVariableUsed(uri_, name_)) {
      // If the variable isn't used, don't bother setting it when we execute
      qname_ = 0;
    }
  }

  if(possiblyUpdating)
    possiblyUpdating = expr_->getStaticAnalysis().isPossiblyUpdating();

  if(first) {
    src.getStaticType() = expr_->getStaticAnalysis().getStaticType();
    src.setProperties(expr_->getStaticAnalysis().getProperties());
  } else {
    src.getStaticType() |= expr_->getStaticAnalysis().getStaticType();
    src.setProperties(src.getProperties() & expr_->getStaticAnalysis().getProperties());
  }
  if(qname_ != 0) {
    src.addExceptVariable(uri_, name_, expr_->getStaticAnalysis());
  }
  else {
    src.add(expr_->getStaticAnalysis());
  }
}

const XQTypeswitch::Case *XQTypeswitch::chooseCase(DynamicContext *context, Sequence &resultSeq) const
{
  // retrieve the value of the operand expression
  ResultBuffer value(expr_->createResult(context));

  const Case *cse = 0;

  // find the effective case
  for(Cases::const_iterator it = cases_->begin(); it != cases_->end(); ++it) {
    try {
      (*it)->getSequenceType()->matches(value.createResult(), (*it)->getSequenceType(), no_err)->toSequence(context);
      cse = *it;
      break;
    }
    catch(const XPath2TypeMatchException &ex) {
      // Well, it doesn't match that one then...
    }
  }

  // if no case is satisfied, use the default one
  if(cse == 0) {
      cse = default_;
  }

  // Bind the variable
  if(cse->isVariableUsed()) {
    resultSeq = value.createResult()->toSequence(context);

//     varStore->declareVar(cse->getURI(), cse->getName(), value.createResult()->toSequence(context), context);
  }

  return cse;
}

EventGenerator::Ptr XQTypeswitch::generateEvents(EventHandler *events, DynamicContext *context,
                                            bool preserveNS, bool preserveType) const
{
  SingleVarStore scope;
  const Case *cse = chooseCase(context, scope.value);

  AutoVariableStoreReset reset(context);
  if(cse->isVariableUsed())
    scope.setAsVariableStore(cse->getURI(), cse->getName(), context);

  return new ClosureEventGenerator(cse->getExpression(), context, preserveNS, preserveType);
}

PendingUpdateList XQTypeswitch::createUpdateList(DynamicContext *context) const
{
  SingleVarStore scope;
  const Case *cse = chooseCase(context, scope.value);

  AutoVariableStoreReset reset(context);
  if(cse->isVariableUsed())
    scope.setAsVariableStore(cse->getURI(), cse->getName(), context);

  return cse->getExpression()->createUpdateList(context);
}

XQTypeswitch::TypeswitchResult::TypeswitchResult(const XQTypeswitch *di)
  : ResultImpl(di),
    _di(di)
{
}

Item::Ptr XQTypeswitch::TypeswitchResult::nextOrTail(Result &tail, DynamicContext *context)
{
  SingleVarStore scope;
  const Case *cse = _di->chooseCase(context, scope.value);

  AutoVariableStoreReset reset(context);
  if(cse->isVariableUsed()) {
    scope.setAsVariableStore(cse->getURI(), cse->getName(), context);
  }

  tail = ClosureResult::create(cse->getExpression(), context);
  return 0;
}

std::string XQTypeswitch::TypeswitchResult::asString(DynamicContext *context, int indent) const
{
  return "TypeswitchResult";
}
