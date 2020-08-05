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
 * $Id: FunctionMax.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/functions/FunctionMax.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/utils/XPath2Utils.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

XERCES_CPP_NAMESPACE_USE;

const XMLCh FunctionMax::name[] = {
  chLatin_m, chLatin_a, chLatin_x, 
  chNull 
};
const unsigned int FunctionMax::minArgs = 1;
const unsigned int FunctionMax::maxArgs = 2;

/**
 * fn:max($arg as xdt:anyAtomicType*) as xdt:anyAtomicType?
 * fn:max($arg as xdt:anyAtomicType*, $collation as string) as xdt:anyAtomicType?
**/

FunctionMax::FunctionMax(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "anyAtomicType*, string", args, memMgr)
{
}

ASTNode* FunctionMax::staticResolution(StaticContext *context) {
  AutoNodeSetOrderingReset orderReset(context);
  return resolveArguments(context);
}

ASTNode *FunctionMax::staticTypingImpl(StaticContext *context)
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

static inline bool checkUntypedAndNaN(Item::Ptr &item, Item::Ptr &max, DynamicContext *context)
{
  if(((AnyAtomicType*)item.get())->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
    if(XPath2Utils::equals(item->getTypeName(), ATUntypedAtomic::fgDT_UNTYPEDATOMIC) &&
       XPath2Utils::equals(item->getTypeURI(), SchemaSymbols::fgURI_SCHEMAFORSCHEMA))
      item = ((AnyAtomicType*)item.get())->castAs(AnyAtomicType::DOUBLE, context);
  }
  if(((AnyAtomicType*)item.get())->isNumericValue()) {
    if(((Numeric*)item.get())->isNaN()) {
      if(((AnyAtomicType*)max.get())->getPrimitiveTypeIndex() == AnyAtomicType::DOUBLE)
        item = ((Numeric*)item.get())->castAs(AnyAtomicType::DOUBLE, context);
      return true;
    }
    if(((AnyAtomicType*)max.get())->isNumericValue()) {
      Numeric::Ptr num = ((Numeric*)item.get())->promoteTypeIfApplicable(((Numeric*)max.get())->getPrimitiveTypeIndex(), context);
      if(num.notNull()) item = num;
      else {
        num = ((Numeric*)max.get())->promoteTypeIfApplicable(((Numeric*)item.get())->getPrimitiveTypeIndex(), context);
        if(num.notNull()) max = num;
      }
    }
  }

  return false;
}

Sequence FunctionMax::createSequence(DynamicContext* context, int flags) const
{
  XPath2MemoryManager* memMgr = context->getMemoryManager();

  Result args = getParamNumber(1,context);

  Item::Ptr max = args->next(context);
  if(max.isNull()) return Sequence(memMgr);

  checkUntypedAndNaN(max, max, context);

  Collation* collation = NULL;
  if(getNumArgs() > 1)
    collation = context->getCollation(getParamNumber(2,context)->next(context)->asString(context), this);
  else collation = context->getDefaultCollation(this);

  Item::Ptr item = args->next(context);
  if(item.isNull()) {
    // check for a type that doesn't support comparison
    try {
      GreaterThan::greater_than(max, max, collation, context, this);
    }
    catch(XQException &e) {
      XQThrow(::IllegalArgumentException, X("FunctionMax::createSequence"),
              X("Invalid argument to fn:max() function [err:FORG0006]"));
    }
    return Sequence(max, memMgr);
  }

  do {
    if(checkUntypedAndNaN(item, max, context)) {
      max = item;
    }
    else {
      try {
        if(GreaterThan::greater_than(item, max, collation, context, this)) {
          max = item;
        }
      }
      catch(XQException &e) {
        XQThrow(::IllegalArgumentException, X("FunctionMax::createSequence"),
                X("Invalid argument to fn:max() function [err:FORG0006]"));
      }
    }
  } while((item = args->next(context)).notNull());

  return Sequence(max, memMgr);
}
