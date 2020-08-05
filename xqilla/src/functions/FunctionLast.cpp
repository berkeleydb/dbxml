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
 * $Id: FunctionLast.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionLast.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>

const XMLCh FunctionLast::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionLast::minArgs = 0;
const unsigned int FunctionLast::maxArgs = 0;

/**
 * fn:last() as xs:integer
**/

FunctionLast::FunctionLast(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

ASTNode* FunctionLast::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionLast::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType::DECIMAL_TYPE;
  _src.contextSizeUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionLast::createSequence(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  if(context->getContextItem()==NULLRCP)
    XQThrow(FunctionException,X("FunctionPosition::createSequence"), X("Undefined context item in fn:last [err:XPDY0002]"));
  return Sequence(context->getItemFactory()->createInteger((long)context->getContextSize(), context), memMgr);
}
















