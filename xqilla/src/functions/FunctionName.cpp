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
 * $Id: FunctionName.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionName.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/ast/XQContextItem.hpp>

XERCES_CPP_NAMESPACE_USE

const XMLCh FunctionName::name[] = {
  chLatin_n, chLatin_a, chLatin_m, 
  chLatin_e, chNull 
};
const unsigned int FunctionName::minArgs = 0;
const unsigned int FunctionName::maxArgs = 1;

/**
 * fn:name() as xs:string
 * fn:name($arg as node()?) as xs:string
**/

FunctionName::FunctionName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "node()?", args, memMgr)
{
}

ASTNode* FunctionName::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  if(_args.empty()) {
    XQContextItem *ci = new (mm) XQContextItem(mm);
    ci->setLocationInfo(this);
    _args.push_back(ci);
  }

  return resolveArguments(context);
}

ASTNode *FunctionName::staticTypingImpl(StaticContext *context)
{
  _src.clear();
  _src.getStaticType() = StaticType::STRING_TYPE;
  return calculateSRCForArguments(context);
}

Sequence FunctionName::createSequence(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* mm = context->getMemoryManager();

  Item::Ptr arg = getParamNumber(1,context)->next(context);
  if(arg.isNull())
    return Sequence(context->getItemFactory()->createString(XMLUni::fgZeroLenString, context), mm);

  return Sequence(FunctionString::string_item(((Node*)arg.get())->dmNodeName(context), context), mm);
}







