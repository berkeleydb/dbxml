/*
 * Copyright (c) 2001-2007
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2007
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
 * $Id: XQInlineFunction.cpp 734 2009-01-30 18:08:37Z gmfeinberg $
 */

#include <xqilla/ast/XQInlineFunction.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/functions/XQUserFunction.hpp>
#include <xqilla/runtime/SingleResult.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#include "../items/impl/FunctionRefImpl.hpp"

XERCES_CPP_NAMESPACE_USE;
using namespace std;

XQInlineFunction::XQInlineFunction(XQUserFunction *func, XPath2MemoryManager *mm)
  : ASTNodeImpl(INLINE_FUNCTION, mm),
    func_(func),
    numArgs_((unsigned int)(func->getArgumentSpecs() ? func->getArgumentSpecs()->size() : 0)),
    instance_(0)
{
}

XQInlineFunction::XQInlineFunction(XQUserFunction *func, unsigned int numArgs, ASTNode *instance, XPath2MemoryManager *mm)
  : ASTNodeImpl(INLINE_FUNCTION, mm),
    func_(func),
    numArgs_(numArgs),
    instance_(instance)
{
}

ASTNode *XQInlineFunction::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  func_->staticResolutionStage1(context);

  instance_ = FunctionRefImpl::createInstance(func_, numArgs_, mm, this);
  instance_ = instance_->staticResolution(context);

  func_->staticResolutionStage2(context);

  return this;
}

ASTNode *XQInlineFunction::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.addExceptContextFlags(instance_->getStaticAnalysis());

  // TBD Using getMemoryManager() might not be thread safe in DB XML - jpcs

  // Remove the argument variables
  XPath2MemoryManager *mm = getMemoryManager();
  for(unsigned int i = 0; i < numArgs_; ++i) {
    XMLBuffer buf(20);
    buf.set(FunctionRefImpl::argVarPrefix);
    XPath2Utils::numToBuf(i, buf);

    _src.removeVariable(0, mm->getPooledString(buf.getRawBuffer()));
  }

  _src.getStaticType() = StaticType(mm, numArgs_, instance_->getStaticAnalysis().getStaticType());

  return this;
}

Result XQInlineFunction::createResult(DynamicContext *context, int flags) const
{
  return (Item::Ptr)new FunctionRefImpl(instance_, numArgs_, _src, context);
}
