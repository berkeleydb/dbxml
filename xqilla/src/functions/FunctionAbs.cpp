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
 * $Id: FunctionAbs.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionAbs.hpp>

#include <xqilla/items/Numeric.hpp>
#include <xqilla/context/DynamicContext.hpp>

const XMLCh FunctionAbs::name[] = {
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_a, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionAbs::minArgs = 1;
const unsigned int FunctionAbs::maxArgs = 1;

/*
 * fn:abs($arg as numeric?) as numeric?
 */

FunctionAbs::FunctionAbs(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : NumericFunction(name, minArgs, maxArgs, "anyAtomicType?", args, memMgr)
{
  _src.getStaticType() = StaticType(StaticType::NUMERIC_TYPE, 0, 1);
}

Sequence FunctionAbs::createSequence(DynamicContext* context, int flags) const
{
  Numeric::Ptr numericArg = getNumericParam(1, context);

  //If the argument is the empty sequence, the empty sequence is returned.
  if(numericArg.isNull()) {
    return Sequence(context->getMemoryManager());
  }

  if(numericArg->isNaN())
    return Sequence(numericArg, context->getMemoryManager());
    
  if(numericArg->isNegative())
    return Sequence(numericArg->invert(context), context->getMemoryManager());
  return Sequence(numericArg, context->getMemoryManager());
}
