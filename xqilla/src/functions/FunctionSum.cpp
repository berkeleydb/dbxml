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
 * $Id: FunctionSum.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionSum.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/mapm/m_apm.h>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

XERCES_CPP_NAMESPACE_USE;

const XMLCh FunctionSum::name[] = {
  chLatin_s, chLatin_u, chLatin_m, 
  chNull 
};
const unsigned int FunctionSum::minArgs = 1;
const unsigned int FunctionSum::maxArgs = 2;

/**
 * fn:sum($arg as xdt:anyAtomicType*) as xdt:anyAtomicType
 * fn:sum($arg as xdt:anyAtomicType*, $zero as xdt:anyAtomicType?) as xdt:anyAtomicType?
**/

FunctionSum::FunctionSum(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "anyAtomicType*,anyAtomicType?", args, memMgr)
{
}

ASTNode* FunctionSum::staticResolution(StaticContext *context) {
  AutoNodeSetOrderingReset orderReset(context);
  return resolveArguments(context);
}

ASTNode *FunctionSum::staticTypingImpl(StaticContext *context)
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

  if(_args[0]->getStaticAnalysis().getStaticType().getMin() == 0) {
    if(_args.size() == 1)
      _src.getStaticType() |= StaticType::DECIMAL_TYPE;
    else
      _src.getStaticType() |= _args[1]->getStaticAnalysis().getStaticType();
  }

  return this;
}

Sequence FunctionSum::createSequence(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Item::Ptr result = sum(getParamNumber(1, context), context, this);
  if(result.isNull())
    if(getNumArgs() == 1)
      return Sequence(context->getItemFactory()->createInteger(0, context), memMgr);
    else
      return getParamNumber(2,context)->toSequence(context);

  return Sequence(result, memMgr);
}

static inline bool checkUntypedAndNaN(Item::Ptr &item, const Item::Ptr &sum, DynamicContext *context)
{
  if(((AnyAtomicType*)item.get())->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
    if(XPath2Utils::equals(item->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC) &&
       XPath2Utils::equals(item->getTypeURI(), SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
      item = ((AnyAtomicType*)item.get())->castAs(AnyAtomicType::DOUBLE, context);
  }
  if(((AnyAtomicType*)item.get())->isNumericValue()) {
    if(((Numeric*)item.get())->isNaN()) {
      if(((AnyAtomicType*)sum.get())->getPrimitiveTypeIndex() == AnyAtomicType::DOUBLE)
        item = ((Numeric*)item.get())->castAs(AnyAtomicType::DOUBLE, context);
      return true;
    }
  }

  return false;
}

Item::Ptr FunctionSum::sum(const Result &res, DynamicContext *context, const LocationInfo *info, unsigned int *count)
{
  Result result = res;

  unsigned int tmpCount;
  if(count == 0) count = &tmpCount;
  *count = 0;

  Item::Ptr sum = result->next(context);
  if(sum.isNull()) return 0;

  ++(*count);
  if(checkUntypedAndNaN(sum, sum, context))
    return sum;

  Item::Ptr item = result->next(context);
  if(item.isNull()) {
    // check for a type that doesn't support addition
    const AnyAtomicType *atom = (const AnyAtomicType*)sum.get();
    if(!atom->isNumericValue() && 
       atom->getPrimitiveTypeIndex() != AnyAtomicType::DAY_TIME_DURATION &&
       atom->getPrimitiveTypeIndex() != AnyAtomicType::YEAR_MONTH_DURATION)
      XQThrow3(::IllegalArgumentException, X("FunctionSum::createSequence"),
               X("Invalid argument to fn:sum() function [err:FORG0006]"), info);
    return sum;
  }

  do {
    ++(*count);
    if(checkUntypedAndNaN(item, sum, context))
      return item;

    try {
      sum = Plus::plus(sum, item, context, info);
    }
    catch(XQException &e) {
      XQThrow3(::IllegalArgumentException, X("FunctionSum::createSequence"),
               X("Invalid argument to fn:sum() function [err:FORG0006]"), info);
    }
  } while((item = result->next(context)).notNull());

  return sum;
}
