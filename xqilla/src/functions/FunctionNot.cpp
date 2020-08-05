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
 * $Id: FunctionNot.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionNot.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/XQEffectiveBooleanValue.hpp>

const XMLCh FunctionNot::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionNot::minArgs = 1;
const unsigned int FunctionNot::maxArgs = 1;

/**
 * fn:not($arg as item()*) as xs:boolean
**/

FunctionNot::FunctionNot(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

ASTNode* FunctionNot::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  _args[0] = new (mm) XQEffectiveBooleanValue(_args[0], mm);
  _args[0]->setLocationInfo(this);
  _args[0] = _args[0]->staticResolution(context);

  return this;
}

ASTNode *FunctionNot::staticTypingImpl(StaticContext *context)
{
  _src.clear();
  _src.copy(_args[0]->getStaticAnalysis());
  return this;
}

Sequence FunctionNot::createSequence(DynamicContext* context, int flags) const
{
  bool result = !((ATBooleanOrDerived*)getParamNumber(1,context)->next(context).get())->isTrue();
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  return Sequence(context->getItemFactory()->createBoolean(result, context), memMgr);
}
