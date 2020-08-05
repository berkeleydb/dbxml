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
 * $Id: Range.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/operators/Range.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/Numeric.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <assert.h>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

/*static*/ const XMLCh Range::name[]={ chLatin_t, chLatin_o, chNull };

Range::Range(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : XQOperator(name, args, memMgr)
{
}

ASTNode* Range::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    SequenceType *integerType = new (mm) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA, 
                                                      SchemaSymbols::fgDT_INTEGER,
                                                      SequenceType::QUESTION_MARK, mm);
    integerType->setLocationInfo(this);

    *i = integerType->convertFunctionArg(*i, context, /*numericfunction*/false, *i);

    *i = (*i)->staticResolution(context);
  }

  return this;
}

ASTNode *Range::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType(StaticType::DECIMAL_TYPE, 0, StaticType::UNLIMITED);

  for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
    _src.add((*i)->getStaticAnalysis());

    if((*i)->getStaticAnalysis().isUpdating()) {
      XQThrow(StaticErrorException,X("Range::staticTyping"),
              X("It is a static error for an operand of an operator "
                "to be an updating expression [err:XUST0001]"));
    }

    if((*i)->isDateOrTimeAndHasNoTimezone(context))
      _src.implicitTimezoneUsed(true);
  }

  return this;
}

Result Range::createResult(DynamicContext* context, int flags) const
{
  return new RangeResult(this, context);
}

Range::RangeResult::RangeResult(const Range *op, DynamicContext *context)
  : ResultImpl(op),
    _op(op),
    _last(0),
    _step(context->getItemFactory()->createInteger(1, context)),
    _end(0)
{  
}

Item::Ptr Range::RangeResult::next(DynamicContext *context)
{
  context->testInterrupt();

  if(_end == NULLRCP) {
    // initialise
    _last = (const Numeric::Ptr )_op->getArgument(0)->createResult(context)->next(context);
    _end = (const Numeric::Ptr )_op->getArgument(1)->createResult(context)->next(context);
    if(_last.isNull() || _end.isNull() || _last->greaterThan(_end, context))
      _last = 0;
  }
  else if(_last != NULLRCP) {
    if(_last->equals((const AnyAtomicType::Ptr)_end, context)) {
      _last = 0;
    }
    else {
      _last = _last->add(_step, context);
    }
  }

  return (const Item::Ptr)_last;
}

