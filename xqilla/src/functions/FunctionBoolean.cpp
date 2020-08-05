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
 * $Id: FunctionBoolean.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionBoolean.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/XQEffectiveBooleanValue.hpp>

XERCES_CPP_NAMESPACE_USE;

const XMLCh FunctionBoolean::name[] = {
  chLatin_b, chLatin_o, chLatin_o, 
  chLatin_l, chLatin_e, chLatin_a, 
  chLatin_n, chNull 
};
const unsigned int FunctionBoolean::minArgs = 1;
const unsigned int FunctionBoolean::maxArgs = 1;

/**
 * fn:boolean($arg as item()*) as xs:boolean
**/

FunctionBoolean::FunctionBoolean(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

ASTNode* FunctionBoolean::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  ASTNode *result = new (mm) XQEffectiveBooleanValue(_args.front(), mm);
  result->setLocationInfo(this);

  return result->staticResolution(context);
}

ASTNode *FunctionBoolean::staticTypingImpl(StaticContext *context)
{
  // Shouldn't happen
  return this;
}
