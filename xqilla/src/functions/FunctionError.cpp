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
 * $Id: FunctionError.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionError.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/update/PendingUpdateList.hpp>

const XMLCh FunctionError::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionError::minArgs = 0;
const unsigned int FunctionError::maxArgs = 3;

/*
 * fn:error() as none
 * fn:error($error as xs:QName) as none
 * fn:error($error as xs:QName?, $description as xs:string) as none
 * fn:error($error as xs:QName?, $description as xs:string, $error-object as item()*) as none
 */

FunctionError::FunctionError(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQFunction(name, minArgs, maxArgs, "QName?, string, item()*", args, memMgr)
{
}

ASTNode* FunctionError::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionError::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  // we need to specify item()*, or we get constant folded away all the time
  _src.getStaticType() = StaticType(StaticType::ITEM_TYPE, 0, StaticType::UNLIMITED);

  _src.forceNoFolding(true);
  _src.possiblyUpdating(true);
  return calculateSRCForArguments(context);
}

PendingUpdateList FunctionError::createUpdateList(DynamicContext *context) const
{
  createSequence(context); // doesn't return
  return PendingUpdateList();
}

Sequence FunctionError::createSequence(DynamicContext* context, int flags) const
{
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer exc_name(1023, context->getMemoryManager());
    exc_name.set(X("User-requested error"));
    switch(getNumArgs()) {
    case 3: // TODO: extra storage in the exception object for the user object
    case 2: {
      Sequence arg = getParamNumber(2,context)->toSequence(context);
      exc_name.append(X(": "));
      exc_name.append(arg.first()->asString(context));
    }
    case 1: {
      Sequence arg = getParamNumber(1,context)->toSequence(context);
      if(arg.isEmpty()) {
        if(getNumArgs() == 1)
          XQThrow(XPath2ErrorException, X("FunctionError::createSequence"), X("ItemType matching failed [err:XPTY0004]"));
        else
          exc_name.append(X(" [err:FOER0000]"));
      }
      else {
        exc_name.append(X(" ["));
        exc_name.append(arg.first()->asString(context));
        exc_name.append(X("]"));
      }
      break;
    }
    case 0:
      exc_name.append(X(" [err:FOER0000]"));
      break;
    }
    XQThrow(XPath2ErrorException, X("FunctionError::createSequence"), exc_name.getRawBuffer());
}


