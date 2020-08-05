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
 * $Id: FunctionAvg.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionAvg.hpp>
#include <xqilla/functions/FunctionSum.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/ContextHelpers.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

const XMLCh FunctionAvg::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_v, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_g,
  XERCES_CPP_NAMESPACE_QUALIFIER chNull
};
const unsigned int FunctionAvg::minArgs = 1;
const unsigned int FunctionAvg::maxArgs = 1;

/**
 * fn:avg($arg as xdt:anyAtomicType*) as xdt:anyAtomicType?
**/

FunctionAvg::FunctionAvg(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "anyAtomicType*", args, memMgr)
{
}

ASTNode* FunctionAvg::staticResolution(StaticContext *context)
{
  AutoNodeSetOrderingReset orderReset(context);
  return resolveArguments(context);
}

ASTNode *FunctionAvg::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  ASTNode *result = calculateSRCForArguments(context);
  if(result != this) return result;

  _src.getStaticType() = _args[0]->getStaticAnalysis().getStaticType();
  _src.getStaticType().setCardinality(_src.getStaticType().getMin() == 0 ? 0 : 1, 1);

  _src.getStaticType().substitute(StaticType::UNTYPED_ATOMIC_TYPE, StaticType::DOUBLE_TYPE);

  if(_src.getStaticType().containsType(StaticType::DOUBLE_TYPE)) {
    _src.getStaticType().substitute(StaticType::DECIMAL_TYPE | StaticType::FLOAT_TYPE, StaticType::DOUBLE_TYPE);
  }
  if(_src.getStaticType().containsType(StaticType::FLOAT_TYPE)) {
    _src.getStaticType().substitute(StaticType::DECIMAL_TYPE, StaticType::FLOAT_TYPE);
  }

  return this;
}

Sequence FunctionAvg::createSequence(DynamicContext* context, int flags) const
{
  try {
    unsigned int count = 0;
    Item::Ptr sum = FunctionSum::sum(getParamNumber(1, context), context, this, &count);
    if(sum.isNull())
      return Sequence(context->getMemoryManager());

    if(count == 1)
      return Sequence(sum, context->getMemoryManager());

    Numeric::Ptr countNum = context->getItemFactory()->createDecimal((long)count, context);

    if(((AnyAtomicType*)sum.get())->isNumericValue()) {
      return Sequence(((Numeric*)sum.get())->divide(countNum, context), context->getMemoryManager());
    }
    else {
      // It must be a duration type
      return Sequence(((ATDurationOrDerived*)sum.get())->divide(countNum, context), context->getMemoryManager());
    }
  }
  catch(::IllegalArgumentException &) {
    XQThrow(IllegalArgumentException, X("FunctionAvg::createSequence()"), X("Invalid argument to fn:avg() function [err:FORG0006]"));
  }
}

