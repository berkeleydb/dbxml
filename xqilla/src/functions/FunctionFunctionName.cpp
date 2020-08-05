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
 * $Id: FunctionFunctionName.cpp 659 2008-10-06 00:11:22Z jpcs $
 */
#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionFunctionName.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/items/FunctionRef.hpp>

XERCES_CPP_NAMESPACE_USE;
using namespace std;

const XMLCh FunctionFunctionName::name[] = {
  'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', '-', 'n', 'a', 'm', 'e', 0
};
const unsigned int FunctionFunctionName::minArgs = 1;
const unsigned int FunctionFunctionName::maxArgs = 1;

/**
 * fn:function-name($function as function()) as xs:QName?
 */
FunctionFunctionName::FunctionFunctionName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "function()", args, memMgr)
{
}

ASTNode* FunctionFunctionName::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionFunctionName::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType::DECIMAL_TYPE;

  return calculateSRCForArguments(context);
}

Sequence FunctionFunctionName::createSequence(DynamicContext* context, int flags) const
{
  FunctionRef::Ptr func = (FunctionRef*)getParamNumber(1, context)->next(context).get();
  return Sequence(func->getName(context), context->getMemoryManager());
}
