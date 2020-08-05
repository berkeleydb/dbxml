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
 * $Id: FunctionCurrentDateTime.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionCurrentDateTime.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/ATDateTimeOrDerived.hpp>
#include "../utils/DateUtils.hpp"
#include <xqilla/ast/StaticAnalysis.hpp>

const XMLCh FunctionCurrentDateTime::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionCurrentDateTime::minArgs = 0;
const unsigned int FunctionCurrentDateTime::maxArgs = 0;

/**
 * fn:current-dateTime() as xs:dateTime
**/

FunctionCurrentDateTime::FunctionCurrentDateTime(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
}

ASTNode* FunctionCurrentDateTime::staticResolution(StaticContext *context) {
  return resolveArguments(context);
}

ASTNode *FunctionCurrentDateTime::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType::DATE_TIME_TYPE;
  _src.currentTimeUsed(true);
  return calculateSRCForArguments(context);
}

Sequence FunctionCurrentDateTime::createSequence(DynamicContext* context, int flags) const
{
  return Sequence(DateUtils::getCurrentDateTime(context), context->getMemoryManager());
}
















