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
 * $Id: NumericFunction.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/ast/NumericFunction.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/items/AnyAtomicType.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

NumericFunction::NumericFunction(const XMLCh* name, unsigned int argsFrom, unsigned int argsTo, const char* paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name,argsFrom, argsTo, paramDecl, args, memMgr) 
{ 
}

ASTNode* NumericFunction::staticResolution(StaticContext *context) {
  return resolveArguments(context, /*checkTimezone*/false, /*numericfunction*/true);
}

ASTNode *NumericFunction::staticTypingImpl(StaticContext *context)
{
  ASTNode *result = calculateSRCForArguments(context, /*checkTimezone*/false, /*numericfunction*/true);
  if(result == this) {
    if(!_args[0]->getStaticAnalysis().getStaticType().containsType(StaticType::NUMERIC_TYPE) &&
       _args[0]->getStaticAnalysis().getStaticType().getMin() > 0)
      XQThrow(FunctionException,X("NumericFunction::staticTyping"), X("Non-numeric argument in numeric function [err:XPTY0004]"));
  }
  return result;
}

Numeric::Ptr NumericFunction::getNumericParam(unsigned int number, DynamicContext *context, int flags) const
{
  Result arg = XQFunction::getParamNumber(number, context, flags);
  Item::Ptr item = arg->next(context);

  if(item.isNull()) {
    return 0;
  }
  else if(item->isAtomicValue() && ((const AnyAtomicType *)item.get())->isNumericValue()) {
    return (const Numeric *)item.get();
  } else {
    XQThrow(FunctionException,X("NumericFunction::getParamNumber"), X("Non-numeric argument in numeric function [err:XPTY0004]"));
  }
}

