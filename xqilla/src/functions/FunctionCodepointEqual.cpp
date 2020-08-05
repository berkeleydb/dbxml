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
 * $Id: FunctionCodepointEqual.cpp 816 2009-11-13 14:58:39Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionCodepointEqual.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>

const XMLCh FunctionCodepointEqual::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_d, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_q, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_u, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_l, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull };
const unsigned int FunctionCodepointEqual::minArgs = 2;
const unsigned int FunctionCodepointEqual::maxArgs = 2;

/**
 * fn:codepoint-equal($comparand1 as xs:string?, $comparand2 as xs:string?) as xs:boolean?
**/

FunctionCodepointEqual::FunctionCodepointEqual(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?,string?", args, memMgr),
    collation_(0)
{
}

FunctionCodepointEqual::FunctionCodepointEqual(Collation *collation, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "string?,string?", args, memMgr),
    collation_(collation)
{
}

ASTNode* FunctionCodepointEqual::staticResolution(StaticContext *context)
{
  collation_ = context->getCollation(CodepointCollation::getCodepointCollationName(), this);

  return resolveArguments(context);
}

ASTNode *FunctionCodepointEqual::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType(StaticType::BOOLEAN_TYPE, 0, 1);

  return calculateSRCForArguments(context);
}

Sequence FunctionCodepointEqual::createSequence(DynamicContext* context, int flags) const
{
    Item::Ptr str1 = getParamNumber(1,context)->next(context);
    Item::Ptr str2 = getParamNumber(2,context)->next(context);
    if(str1.isNull() || str2.isNull())
        return Sequence(context->getMemoryManager());

    const XMLCh* string1 = str1->asString(context);
    const XMLCh* string2 = str2->asString(context);
    
    return Sequence(context->getItemFactory()->createBoolean(collation_->compare(string1,string2)==0, context),
                    context->getMemoryManager());
}
