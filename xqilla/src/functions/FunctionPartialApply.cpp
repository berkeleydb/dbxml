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
 * $Id: FunctionPartialApply.cpp 681 2008-11-25 10:57:27Z jpcs $
 */
#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionPartialApply.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/FunctionRef.hpp>
#include <xqilla/utils/UTF8Str.hpp>

#include <stdlib.h>

XERCES_CPP_NAMESPACE_USE;
using namespace std;

const XMLCh FunctionPartialApply::name[] = {
  'p', 'a', 'r', 't', 'i', 'a', 'l', '-', 'a', 'p', 'p', 'l', 'y', 0
};
const unsigned int FunctionPartialApply::minArgs = 2;
const unsigned int FunctionPartialApply::maxArgs = 3;

/**
 * fn:partial-apply($function as function(), $arg as item()*) as function()
 * fn:partial-apply($function as function(), $arg as item()*, $argNum as xs:integer) as function()
 */
FunctionPartialApply::FunctionPartialApply(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "function(),item()*,integer", args, memMgr)
{
}

ASTNode* FunctionPartialApply::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionPartialApply::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    _src.add((*i)->getStaticAnalysis());

    if((*i)->getStaticAnalysis().isUpdating()) {
      XQThrow(StaticErrorException,X("FunctionPartialApply::staticTyping"),
              X("It is a static error for an argument to a function "
                "to be an updating expression [err:XUST0001]"));
    }
  }

  const StaticType &inType = _args[0]->getStaticAnalysis().getStaticType();

  if(inType.getReturnType() == 0) {
    _src.getStaticType() = StaticType::FUNCTION_TYPE;
  }
  else {
    unsigned int minArgs = inType.getMinArgs();
    unsigned int maxArgs = inType.getMaxArgs();

    if(maxArgs == 0) {
      XMLBuffer buf;
      buf.set(X("The function item argument to fn:partial-apply() must accept one or more arguments - the expression has a static type of "));
      inType.typeToBuf(buf);
      buf.append(X(" [err:TBD]"));
      XQThrow(XPath2TypeMatchException, X("FunctionPartialApply::staticTyping"), buf.getRawBuffer());
    }

    if(minArgs > 0) --minArgs;
    if(maxArgs > 0) --maxArgs;

    // TBD Using getMemoryManager() might not be thread safe in DB XML - jpcs
    _src.getStaticType() = StaticType(getMemoryManager(), minArgs, maxArgs,
                                      *inType.getReturnType());
  }

  return this;
}

Result FunctionPartialApply::createResult(DynamicContext* context, int flags) const
{
  int argNum = 1;

  if(getNumArgs() == 3) {
    argNum = ((Numeric*)getParamNumber(3, context)->next(context).get())->asInt();

    if(argNum < 1) {
      XQThrow(FunctionException, X("FunctionPartialApply::staticTyping"), X("The argument number provided to fn:partial-apply() is less than 1 [err:TBD]"));
    }
  }

  Result arg = getParamNumber(2, context);

  return (Item::Ptr)((FunctionRef*)getParamNumber(1, context)->next(context).get())->
    partialApply(arg, argNum, context, this);
}
