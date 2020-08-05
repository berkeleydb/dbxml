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
 * $Id: FunctionMatches.cpp 756 2009-05-27 14:38:43Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionMatches.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/util/regx/RegularExpression.hpp>
#include <xercesc/util/ParseException.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLUni.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh FunctionMatches::name[] = {
  chLatin_m, chLatin_a, chLatin_t, 
  chLatin_c, chLatin_h, chLatin_e, 
  chLatin_s, chNull 
};
const unsigned int FunctionMatches::minArgs = 2;
const unsigned int FunctionMatches::maxArgs = 3;

/**
 * fn:matches($input as xs:string?, $pattern as xs:string) as xs:boolean
 * fn:matches($input as xs:string?, $pattern as xs:string, $flags as xs:string) as xs:boolean
 */
  
FunctionMatches::FunctionMatches(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string, string", args, memMgr)
{
  _src.getStaticType() = StaticType::BOOLEAN_TYPE;
}

Sequence FunctionMatches::createSequence(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  const XMLCh* input = XMLUni::fgZeroLenString;
  Item::Ptr inputItem = getParamNumber(1,context)->next(context);
  if(inputItem.notNull())
    input=inputItem->asString(context);

  const XMLCh* pattern = getParamNumber(2,context)->next(context)->asString(context);

  const XMLCh* options = XMLUni::fgZeroLenString;
  if(getNumArgs()>2)
    options=getParamNumber(3,context)->next(context)->asString(context);

  //Check that the options are valid - throw an exception if not (can have s,m,i and x)
  //Note: Are allowed to duplicate the letters.
  const XMLCh* cursor=options;
  for(; *cursor != 0; ++cursor){
    switch(*cursor) {
    case chLatin_s:
    case chLatin_m:
    case chLatin_i:
    case chLatin_x:
      break;
    default:
      XQThrow(FunctionException, X("FunctionMatches::createSequence"),X("Invalid regular expression flags [err:FORX0001]."));
    }
  }

  try {
    return Sequence(context->getItemFactory()->createBoolean(matches(input, pattern, options), context), memMgr);
  } catch (ParseException &e){ 
    XMLBuffer buf(1023, memMgr);
    buf.set(X("Invalid regular expression: "));
    buf.append(e.getMessage());
    buf.append(X(" [err:FORX0002]"));
    XQThrow(FunctionException, X("FunctionMatches::createSequence"), buf.getRawBuffer());
  } catch (XMLException &e){ 
    XQThrow(FunctionException, X("FunctionMatches::createSequence"), e.getMessage());  
  }  

  //do not get here

}

bool FunctionMatches::matches(const XMLCh *input, const XMLCh *pattern, const XMLCh *options)
{
  // If the value of $operand2 is the zero-length string, then the function returns true
  if(pattern == 0 || *pattern == 0) return true;

  if(input == 0) input = XMLUni::fgZeroLenString;

  // Always turn off head character optimisation, since it is broken
  XMLBuffer optionsBuf;
  optionsBuf.set(options);
  optionsBuf.append(chLatin_H);

  //Build the Regular Expression
  RegularExpression regEx(pattern, optionsBuf.getRawBuffer());
  return regEx.matches(input);
}
