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
 * $Id: FunctionDefaultCollation.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionDefaultCollation.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/ItemFactory.hpp>

const XMLCh FunctionDefaultCollation::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionDefaultCollation::minArgs = 0;
const unsigned int FunctionDefaultCollation::maxArgs = 0;

/**
 * fn:default-collation() as xs:string
**/

FunctionDefaultCollation::FunctionDefaultCollation(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "empty()", args, memMgr)
{
  _src.getStaticType() = StaticType::STRING_TYPE;
}

Sequence FunctionDefaultCollation::createSequence(DynamicContext* context, int flags) const
{
  Collation* collation = context->getDefaultCollation(this);
  if(collation != NULL)
    return Sequence(context->getItemFactory()->createString(collation->getCollationName(), context),
                    context->getMemoryManager());

  return Sequence(context->getItemFactory()->createString(CodepointCollation::getCodepointCollationName(), context),
                  context->getMemoryManager());
}
