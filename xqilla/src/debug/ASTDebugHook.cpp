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
 * $Id$
 */

#include <xqilla/debug/ASTDebugHook.hpp>

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/debug/DebugListener.hpp>
#include <xqilla/debug/StackFrame.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/update/PendingUpdateList.hpp>
#include <xqilla/utils/PrintAST.hpp>

ASTDebugHook::ASTDebugHook(ASTNode *astNode, XPath2MemoryManager *mm)
  : ASTNodeImpl(DEBUG_HOOK, mm),
    expr_(astNode)
{
  _src.copy(expr_->getStaticAnalysis());
  setLocationInfo(expr_);
}

bool ASTDebugHook::isConstant() const
{
  return expr_->isConstant();
}

bool ASTDebugHook::isDateOrTimeAndHasNoTimezone(StaticContext* context) const
{
  return expr_->isDateOrTimeAndHasNoTimezone(context);
}

ASTNode *ASTDebugHook::staticResolution(StaticContext* context)
{
  expr_ = expr_->staticResolution(context);
  return this;
}

ASTNode *ASTDebugHook::staticTypingImpl(StaticContext *context)
{
  _src.clear();
  _src.copy(expr_->getStaticAnalysis());
  return this;
}

class ASTStackFrame : public StackFrame {
public:
  ASTStackFrame(const ASTNode *ast, DynamicContext *context)
    : StackFrame(ast, context) {}

  void setPreviousFrame(DynamicContext *context)
  {
    prev_ = context->getStackFrame();
  }

  const ASTNode *getASTNode() const { return (ASTNode*)location_; }
  const TupleNode *getTupleNode() const { return 0; }
  virtual std::string getQueryPlan() const
  {
    return PrintAST::print(getASTNode(), context_, 0);
  }
};

class ASTDebugHookResult : public ResultImpl
{
public:
  ASTDebugHookResult(const ASTNode *expr, DynamicContext *context)
    : ResultImpl(expr),
      context_(context),
      frame_(expr, context),
      parent_(0)
  {
    DebugListener *dl = context->getDebugListener();

    AutoStackFrameReset reset(context, &frame_);
    if(dl) dl->start(&frame_, context);
    try {
      parent_ = expr->createResult(context);
    }
    catch(XQException &ex) {
      if(dl) dl->error(ex, &frame_, context);
      if(dl) dl->end(&frame_, context);
      throw;
    }
  }
  
  ASTDebugHookResult(const Result &contextItems, const ASTNode *expr, DynamicContext *context)
    : ResultImpl(expr),
      context_(context),
      frame_(expr, context),
      parent_(0)
  {
    DebugListener *dl = context->getDebugListener();

    AutoStackFrameReset reset(context, &frame_);
    if(dl) dl->start(&frame_, context);
    try {
      parent_ = expr->iterateResult(contextItems, context);
    }
    catch(XQException &ex) {
      if(dl) dl->error(ex, &frame_, context);
    }
  }

  ~ASTDebugHookResult()
  {
    parent_ = 0;

    DebugListener *dl = context_->getDebugListener();

    AutoStackFrameReset reset(context_, &frame_);
    if(dl) dl->end(&frame_, context_);
  }
  
  Item::Ptr next(DynamicContext *context)
  {
    DebugListener *dl = context->getDebugListener();

    frame_.setPreviousFrame(context);
    AutoStackFrameReset reset(context, &frame_);
    if(dl) dl->enter(&frame_, context);
    try {
      Item::Ptr item = parent_->next(context);
      if(dl) dl->exit(&frame_, context);
      return item;
    }
    catch(XQException &ex) {
      if(dl) dl->error(ex, &frame_, context);
    }
    return 0;
  }

  Item::Ptr nextOrTail(Result &tail, DynamicContext *context)
  {
    DebugListener *dl = context->getDebugListener();

    frame_.setPreviousFrame(context);
    AutoStackFrameReset reset(context, &frame_);
    if(dl) dl->enter(&frame_, context);
    try {
      ResultImpl *oldparent = parent_.get();
      Item::Ptr item = parent_->nextOrTail(parent_, context);
      if(dl) dl->exit(&frame_, context);
      if(parent_.get() != oldparent) tail = parent_;
      return item;
    }
    catch(XQException &ex) {
      if(dl) dl->error(ex, &frame_, context);
    }
    return 0;
  }

protected:
  DynamicContext *context_;
  ASTStackFrame frame_;
  Result parent_;
};

Result ASTDebugHook::createResult(DynamicContext *context, int flags) const
{
  DebugListener *dl = context->getDebugListener();

  if(!dl)
    return expr_->createResult(context);

  if(dl->doLazyEvaluation())
    return new ASTDebugHookResult(expr_, context);

  ASTStackFrame frame(expr_, context);
  AutoStackFrameReset reset(context, &frame);

  dl->start(&frame, context);
  dl->enter(&frame, context);
  Sequence result(context->getMemoryManager());
  try {
    result = expr_->createResult(context)->toSequence(context);
    dl->exit(&frame, context);
    dl->end(&frame, context);
  }
  catch(XQException &ex) {
    dl->error(ex, &frame, context);
  }

  return result;
}

Result ASTDebugHook::iterateResult(const Result &contextItems, DynamicContext *context) const
{
  DebugListener *dl = context->getDebugListener();

  if(!dl)
    return expr_->iterateResult(contextItems, context);

  if(dl->doLazyEvaluation() && dl->doFocusOptimizations())
    return new ASTDebugHookResult(contextItems, expr_, context);

  return ASTNodeImpl::iterateResult(contextItems, context);
}

EventGenerator::Ptr ASTDebugHook::generateEvents(EventHandler *events, DynamicContext *context,
                                                 bool preserveNS, bool preserveType) const
{
  DebugListener *dl = context->getDebugListener();

  if(!dl)
    return expr_->generateEvents(events, context, preserveNS, preserveType);

  ASTStackFrame frame(expr_, context);
  AutoStackFrameReset reset(context, &frame);

  dl->start(&frame, context);
  dl->enter(&frame, context);
  try {
    EventGenerator::Ptr result = expr_->generateEvents(events, context, preserveNS, preserveType);
    dl->exit(&frame, context);
    dl->end(&frame, context);
    return result;
  }
  catch(XQException &ex) {
    dl->error(ex, &frame, context);
  }

  return 0;
}

PendingUpdateList ASTDebugHook::createUpdateList(DynamicContext *context) const
{
  DebugListener *dl = context->getDebugListener();

  if(!dl)
    return expr_->createUpdateList(context);

  ASTStackFrame frame(expr_, context);
  AutoStackFrameReset reset(context, &frame);

  dl->start(&frame, context);
  dl->enter(&frame, context);
  PendingUpdateList pul;
  try {
    pul = expr_->createUpdateList(context);
    dl->exit(&frame, context);
    dl->end(&frame, context);
  }
  catch(XQException &ex) {
    dl->error(ex, &frame, context);
  }

  return pul;
}

