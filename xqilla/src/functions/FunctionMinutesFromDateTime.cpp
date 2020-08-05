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
 * $Id: FunctionMinutesFromDateTime.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionMinutesFromDateTime.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATDateTimeOrDerived.hpp>

const XMLCh FunctionMinutesFromDateTime::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_T, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionMinutesFromDateTime::minArgs = 1;
const unsigned int FunctionMinutesFromDateTime::maxArgs = 1;

/*
 * fn:minutes-from-dateTime($arg as xs:dateTime?) as xs:integer?
 */

FunctionMinutesFromDateTime::FunctionMinutesFromDateTime(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "dateTime?", args, memMgr)
{
}

ASTNode* FunctionMinutesFromDateTime::staticResolution(StaticContext *context)
{
  return resolveArguments(context, /*checkTimezone*/true);
}

ASTNode *FunctionMinutesFromDateTime::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType(StaticType::DECIMAL_TYPE, 0, 1);
  return calculateSRCForArguments(context, /*checkTimezone*/true);
}

Sequence FunctionMinutesFromDateTime::createSequence(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Item::Ptr arg = getParamNumber(1, context)->next(context);
  if(arg.isNull()) return Sequence(memMgr);

  return Sequence(((const ATDateTimeOrDerived*)arg.get())->getMinutes(context), memMgr);
}
















