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
 * $Id: GeneralComp.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/context/impl/CodepointCollation.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>

/*static*/ const XMLCh GeneralComp::name[]={ XERCES_CPP_NAMESPACE_QUALIFIER chLatin_c, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_m, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_p, XERCES_CPP_NAMESPACE_QUALIFIER chNull };

GeneralComp::GeneralComp(ComparisonOperation operation, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr),
    operation_(operation),
    collation_(0),
    xpath1compat_(false)
{
}

GeneralComp::GeneralComp(ComparisonOperation operation, const VectorOfASTNodes &args, Collation *collation, bool xpath1Compat, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr),
    operation_(operation),
    collation_(collation),
    xpath1compat_(xpath1Compat)
{
}

bool GeneralComp::compare(GeneralComp::ComparisonOperation operation, AnyAtomicType::Ptr first, AnyAtomicType::Ptr second,
                          Collation* collation, DynamicContext *context, bool xpath1compat, const LocationInfo *info)
{
  // The magnitude relationship between two atomic values is determined as follows:
  // 1) If either atomic value has the dynamic type xdt:untypedAtomic, that value is cast to a required type, 
  //    which is determined as follows:
  //    - If the dynamic type of the other atomic value is a numeric type, the required type is xs:double.
  //    - If the dynamic type of the other atomic value is xdt:untypedAtomic, the required type is xs:string.
  //    - Otherwise, the required type is the dynamic type of the other atomic value.
  //    If the cast to the required type fails, a dynamic error is raised.
  // 2) If XPath 1.0 compatibility mode is true, and at least one of the atomic values has a numeric type, 
  //    then both atomic values are cast to to the type xs:double.
  // 3) After any necessary casting, the atomic values are compared using one of the value comparison operators 
  //    eq, ne, lt, le, gt, or ge, depending on whether the general comparison operator was 
  //    =, !=, <, <=, >, or >=. The values have the required magnitude relationship if the result of this 
  //    value comparison is true.

  if(first->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
    if (second->isNumericValue()) {
      first = first->castAs(AnyAtomicType::DOUBLE, context);
    }
    else if(second->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
      first = first->castAs(AnyAtomicType::STRING, context);
    }
    else {
      first = first->castAs(second->getPrimitiveTypeIndex(),
                            second->getTypeURI(),
                            second->getTypeName(), context);
    }
  }
  if(second->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
    if(first->isNumericValue()) {
      second = second->castAs(AnyAtomicType::DOUBLE, context);
    }
    else if(first->getPrimitiveTypeIndex() == AnyAtomicType::UNTYPED_ATOMIC) {
      second = second->castAs(AnyAtomicType::STRING, context);
    }
    else {
      second = second->castAs(first->getPrimitiveTypeIndex(),
                              first->getTypeURI(),
                              first->getTypeName(), context);
    }
  }
  if(xpath1compat && (first->isNumericValue() || second->isNumericValue())) {
    first = first->castAs(AnyAtomicType::DOUBLE, context);
    second = second->castAs(AnyAtomicType::DOUBLE, context);
  }
  bool result = false;
  switch(operation) {
  case GeneralComp::EQUAL:              result = Equals::equals(first,second,collation,context,info); break;
  case GeneralComp::NOT_EQUAL:          result = NotEquals::not_equals(first,second,collation,context,info); break;
  case GeneralComp::LESS_THAN:          result = LessThan::less_than(first,second,collation,context,info); break;
  case GeneralComp::LESS_THAN_EQUAL:    result = LessThanEqual::less_than_equal(first,second,collation,context,info); break;
  case GeneralComp::GREATER_THAN:       result = GreaterThan::greater_than(first,second,collation,context,info); break;
  case GeneralComp::GREATER_THAN_EQUAL: result = GreaterThanEqual::greater_than_equal(first,second,collation,context,info); break;
  default:                 assert(false);
  }

  return result;
}

ASTNode* GeneralComp::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  xpath1compat_ = context->getXPath1CompatibilityMode();

  AutoNodeSetOrderingReset orderReset(context);

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    *i = new (mm) XQAtomize(*i, mm);
    (*i)->setLocationInfo(this);
    *i = (*i)->staticResolution(context);
  }

  collation_ = context->getDefaultCollation(this);

  return this;
}

ASTNode *GeneralComp::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    _src.add((*i)->getStaticAnalysis());

    if((*i)->getStaticAnalysis().isUpdating()) {
      XQThrow(StaticErrorException,X("GeneralComp::staticTyping"),
              X("It is a static error for an operand of an operator "
                "to be an updating expression [err:XUST0001]"));
    }

    if((*i)->isDateOrTimeAndHasNoTimezone(context))
      _src.implicitTimezoneUsed(true);
  }

  _src.getStaticType() = StaticType::BOOLEAN_TYPE;

  return this;
}

class GeneralCompResult : public ResultImpl
{
public:
  GeneralCompResult(const GeneralComp *op)
    : ResultImpl(op),
      _op(op)
  {
  }

  virtual Item::Ptr nextOrTail(Result &tail, DynamicContext *context)
  {
    // Atomization is applied to each operand of a general comparison.
    Result arg1 = _op->getArgument(0)->createResult(context);
    Result arg2 = _op->getArgument(1)->createResult(context);

    // The result of the comparison is true if and only if there is a pair of atomic values, 
    // one belonging to the result of atomization of the first operand and the other belonging 
    // to the result of atomization of the second operand, that have the required magnitude relationship.
    // Otherwise the result of the general comparison is false.

    AnyAtomicType::Ptr item1 = (const AnyAtomicType::Ptr)arg1->next(context);
    if(item1 != NULLRCP) {
      // The first time we loop over arg2, we store it in a sequence
      AnyAtomicType::Ptr item2;
      Sequence arg2_sequence(context->getMemoryManager());
      while((item2 = (const AnyAtomicType::Ptr)arg2->next(context)) != NULLRCP) {
        if(GeneralComp::compare(_op->getOperation(), item1, item2, _op->getCollation(), context, _op->getXPath1CompatibilityMode(), this)) {
          tail = 0;
          return context->getItemFactory()->createBoolean(true, context);
        }
        arg2_sequence.addItem(item2);
      }

      // The second and subsequent times, we iterate over the sequence
      Sequence::iterator itSecond;
      while((item1 = (const AnyAtomicType::Ptr)arg1->next(context)) != NULLRCP) {
        for(itSecond = arg2_sequence.begin(); itSecond != arg2_sequence.end(); ++itSecond) {
          if(GeneralComp::compare(_op->getOperation(), item1, (const AnyAtomicType::Ptr)*itSecond, _op->getCollation(), context,
                     _op->getXPath1CompatibilityMode(), this)) {
            tail = 0;
            return context->getItemFactory()->createBoolean(true, context);
          }
        }
      }
    }
    tail = 0;
    return context->getItemFactory()->createBoolean(false, context);
  }

private:
  const GeneralComp *_op;
};

Result GeneralComp::createResult(DynamicContext* context, int flags) const
{
  return new GeneralCompResult(this);
}

