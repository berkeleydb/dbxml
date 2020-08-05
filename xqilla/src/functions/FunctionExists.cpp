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
 * $Id: FunctionExists.cpp 808 2009-10-26 17:24:59Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/ContextHelpers.hpp>

const XMLCh FunctionExists::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_x, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionExists::minArgs = 1;
const unsigned int FunctionExists::maxArgs = 1;

/*
 * fn:exists($arg as item()*) as xs:boolean
 */

FunctionExists::FunctionExists(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*", args, memMgr)
{
}

ASTNode* FunctionExists::staticResolution(StaticContext *context) {
  AutoNodeSetOrderingReset orderReset(context);
  return resolveArguments(context);
}

ASTNode *FunctionExists::staticTypingImpl(StaticContext *context)
{
  _src.clear();
  _src.getStaticType() = StaticType::BOOLEAN_TYPE;

  ASTNode *result = calculateSRCForArguments(context);
  if(context && result == this) {
    const StaticAnalysis &sa = _args[0]->getStaticAnalysis();
    const StaticType &sType = sa.getStaticType();
    if((sType.getMin() > 0 || sType.getMax() == 0) && !sa.areDocsOrCollectionsUsed() && !sa.isNoFoldingForced()) {
      XPath2MemoryManager* mm = context->getMemoryManager();
      return XQLiteral::create(sType.getMin() > 0, mm, this);
    }
  }

  return result;
}

Sequence FunctionExists::createSequence(DynamicContext* context, int flags) const
{
  Result items = getParamNumber(1,context);
  return Sequence(context->getItemFactory()->createBoolean(items->next(context).notNull(), context),
                  context->getMemoryManager());
}
















