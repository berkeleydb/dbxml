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
 * $Id: FunctionFunctionArity.cpp 734 2009-01-30 18:08:37Z gmfeinberg $
 */
#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionFunctionArity.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/items/FunctionRef.hpp>

XERCES_CPP_NAMESPACE_USE;
using namespace std;

const XMLCh FunctionFunctionArity::name[] = {
  'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', '-', 'a', 'r', 'i', 't', 'y', 0
};
const unsigned int FunctionFunctionArity::minArgs = 1;
const unsigned int FunctionFunctionArity::maxArgs = 1;

/**
 * fn:function-arity($function as function()) as xs:integer
 */
FunctionFunctionArity::FunctionFunctionArity(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "function()", args, memMgr)
{
}

ASTNode* FunctionFunctionArity::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionFunctionArity::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType::DECIMAL_TYPE;

  return calculateSRCForArguments(context);
}

Sequence FunctionFunctionArity::createSequence(DynamicContext* context, int flags) const
{
  FunctionRef::Ptr func = (FunctionRef*)getParamNumber(1, context)->next(context).get();
  return Sequence(context->getItemFactory()->createInteger((const int)func->getNumArgs(), context),
                  context->getMemoryManager());
}
