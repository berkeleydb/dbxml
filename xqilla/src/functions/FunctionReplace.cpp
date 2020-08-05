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
 * $Id: FunctionReplace.cpp 552 2008-07-07 15:26:36Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionReplace.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xercesc/util/regx/RegularExpression.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/ParseException.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/XMLUni.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

const XMLCh FunctionReplace::name[] = {
  chLatin_r, chLatin_e, chLatin_p, 
  chLatin_l, chLatin_a, chLatin_c, 
  chLatin_e, chNull 
};
const unsigned int FunctionReplace::minArgs = 3;
const unsigned int FunctionReplace::maxArgs = 4;

/** 
 * fn:replace($input as xs:string?, $pattern as xs:string, $replacement as xs:string) as xs:string
 * fn:replace($input as xs:string?, $pattern as xs:string, $replacement as xs:string, $flags as xs:string) as xs:string
 */
  
FunctionReplace::FunctionReplace(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "string?, string, string, string", args, memMgr)
{
  _src.getStaticType() = StaticType::STRING_TYPE;
}

Sequence FunctionReplace::createSequence(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  const XMLCh* input = XMLUni::fgZeroLenString;
  Item::Ptr inputString = getParamNumber(1,context)->next(context);
  if(inputString.notNull())
    input = inputString->asString(context);
  const XMLCh *pattern = getParamNumber(2,context)->next(context)->asString(context);
  const XMLCh *replacement = getParamNumber(3,context)->next(context)->asString(context);

  const XMLCh *ptr;
  for(ptr = replacement; *ptr != chNull; ++ptr) {
    if(*ptr == chDollarSign) {
      ++ptr;
      
      //check that after the '$' is a digit
      if(!XMLString::isDigit(*ptr))
        XQThrow(FunctionException, X("FunctionReplace::createSequence"), X("Invalid replacement pattern - '$' without following digit [err:FORX0004]"));
    }
    else if(*ptr == chBackSlash) {
      ++ptr;
      
      //check that after the '\' is a '$' or '\'
      if(*ptr != chDollarSign && *ptr != chBackSlash) {
        XQThrow(FunctionException, X("FunctionReplace::createSequence"), X("Invalid replacement pattern - '\\' without following '$' or '\\' [err:FORX0004]"));
      }
    }
  }

  const XMLCh *options = XMLUni::fgZeroLenString;
  if(getNumArgs()>3)
    options=getParamNumber(4,context)->next(context)->asString(context);
  
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
      XQThrow(FunctionException, X("FunctionReplace::createSequence"),X("Invalid regular expression flags [err:FORX0001]."));
    }
  }

  // Now attempt to replace
  try {
    AutoDeallocate<const XMLCh> result(replace(input, pattern, replacement, options, memMgr), memMgr);
    return Sequence(context->getItemFactory()->createString(result.get(), context), memMgr);

  } catch (ParseException &e){ 
    XMLBuffer buf(1023, memMgr);
    buf.set(X("Invalid regular expression: "));
    buf.append(e.getMessage());
    buf.append(X(" [err:FORX0002]"));
    XQThrow(FunctionException, X("FunctionReplace::createSequence"), buf.getRawBuffer());
  } catch (RuntimeException &e){ 
    if(e.getCode()==XMLExcepts::Regex_RepPatMatchesZeroString)
      XQThrow(FunctionException, X("FunctionReplace::createSequence"), X("The pattern matches the zero-length string [err:FORX0003]"));
    else if(e.getCode()==XMLExcepts::Regex_InvalidRepPattern)
      XQThrow(FunctionException, X("FunctionReplace::createSequence"), X("Invalid replacement pattern [err:FORX0004]"));
    else 
      XQThrow(FunctionException, X("FunctionReplace::createSequence"), e.getMessage());
  }

  // Never happens
}

const XMLCh *FunctionReplace::replace(const XMLCh *input, const XMLCh *pattern, const XMLCh *replacement, const XMLCh *options, MemoryManager *mm)
{
  // Always turn off head character optimisation, since it is broken
  XMLBuffer optionsBuf;
  optionsBuf.set(options);
  optionsBuf.append(chLatin_H);

  //Now attempt to replace
  RegularExpression regEx(pattern, optionsBuf.getRawBuffer(), mm);
#ifdef HAVE_ALLMATCHES
  return regEx.replace(input, replacement, mm);
#else
  return regEx.replace(input, replacement);
#endif
}
