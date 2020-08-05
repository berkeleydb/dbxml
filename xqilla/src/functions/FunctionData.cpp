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
 * $Id: FunctionData.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionData.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQAtomize.hpp>

const XMLCh FunctionData::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionData::minArgs = 1;
const unsigned int FunctionData::maxArgs = 1;

/*
 * fn:data($arg as item()*) as xdt:anyAtomicType*
 */

FunctionData::FunctionData(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

ASTNode* FunctionData::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  ASTNode *result = new (mm) XQAtomize(_args.front(), mm);
  result->setLocationInfo(this);

  return result->staticResolution(context);
}

ASTNode *FunctionData::staticTypingImpl(StaticContext *context)
{
  // Should never happen
  return this;
}

