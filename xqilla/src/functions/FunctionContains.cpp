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
 * $Id: FunctionContains.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionContains.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/ATAnyURIOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xqilla/context/ItemFactory.hpp>

const XMLCh FunctionContains::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionContains::minArgs = 2;
const unsigned int FunctionContains::maxArgs = 3;

/**
 * fn:contains($arg1 as xs:string?, $arg2 as xs:string?) as xs:boolean
 * fn:contains($arg1 as xs:string?, $arg2 as xs:string?, $collation as xs:string) as xs:boolean
**/

FunctionContains::FunctionContains(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?,string?,string", args, memMgr)
{
  _src.getStaticType() = StaticType::BOOLEAN_TYPE;
}

Sequence FunctionContains::createSequence(DynamicContext* context, int flags) const
{
    Sequence str1 = getParamNumber(1,context)->toSequence(context);
    Sequence str2 = getParamNumber(2,context)->toSequence(context);

    Collation* collation = NULL;
    if(getNumArgs()>2) {
        Sequence collArg = getParamNumber(3,context)->toSequence(context);
        const XMLCh* collName = collArg.first()->asString(context);
        try {
            context->getItemFactory()->createAnyURI(collName, context);
        } catch(XPath2ErrorException &e) {
            XQThrow(FunctionException, X("FunctionContains::createSequence"), X("Invalid collationURI"));  
        }
        collation=context->getCollation(collName, this);
        if(collation==NULL)
            XQThrow(FunctionException,X("FunctionContains::createSequence"),X("Collation object is not available"));
    }
    else
        collation=context->getCollation(CodepointCollation::getCodepointCollationName(), this);

    const XMLCh* container = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
    if(!str1.isEmpty())
        container=str1.first()->asString(context);
    const XMLCh* pattern = XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgZeroLenString;
    if(!str2.isEmpty())
        pattern=str2.first()->asString(context);

    if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(pattern)==0)
        return Sequence(context->getItemFactory()->createBoolean(true, context), context->getMemoryManager());
    else if(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::stringLen(container)==0)
        return Sequence(context->getItemFactory()->createBoolean(false, context), context->getMemoryManager());

    bool contains = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::patternMatch(container, pattern) > -1;
    return Sequence(context->getItemFactory()->createBoolean(contains, context), context->getMemoryManager());
}
