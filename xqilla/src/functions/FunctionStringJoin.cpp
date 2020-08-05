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
 * $Id: FunctionStringJoin.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionStringJoin.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/framework/XMLBuffer.hpp>

const XMLCh FunctionStringJoin::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_j, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionStringJoin::minArgs = 2;
const unsigned int FunctionStringJoin::maxArgs = 2;

/**
 * fn:string-join($arg1 as xs:string*, $arg2 as xs:string) as xs:string
**/

FunctionStringJoin::FunctionStringJoin(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string*, string", args, memMgr)
{
  _src.getStaticType() = StaticType::STRING_TYPE;
}

Sequence FunctionStringJoin::createSequence(DynamicContext* context, int flags) const {
	XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer result(1023, context->getMemoryManager());
  XPath2MemoryManager* memMgr = context->getMemoryManager();
  Sequence strSeq = getParamNumber(1, context)->toSequence(context);
  const XMLCh* strSep = getParamNumber(2,context)->next(context)->asString(context);

  //if the sequence is non-empty, append the 1st string to the result
  if(!strSeq.isEmpty()) {
    Sequence::iterator end = strSeq.end();
    Sequence::iterator i = strSeq.begin();

    result.append((*i)->asString(context));

    //join the last length - 1 strings each prefixed by the separator
    for(++i; i != end; ++i) {
      result.append(strSep);
      result.append((*i)->asString(context));
    }
  }
  
  const ATStringOrDerived::Ptr strResult = context->getItemFactory()->createString(result.getRawBuffer(), context);
  return Sequence(strResult, memMgr);
}
