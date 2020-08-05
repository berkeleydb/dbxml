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
 * $Id: ConstantFoldingFunction.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/ast/ConstantFoldingFunction.hpp>
#include <xqilla/context/StaticContext.hpp>

ConstantFoldingFunction::ConstantFoldingFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, argsFrom, argsTo, paramDecl, args, memMgr)
{
}


ConstantFoldingFunction::~ConstantFoldingFunction()
{
}

ASTNode* ConstantFoldingFunction::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *ConstantFoldingFunction::staticTypingImpl(StaticContext *context)
{
  return calculateSRCForArguments(context);
}
