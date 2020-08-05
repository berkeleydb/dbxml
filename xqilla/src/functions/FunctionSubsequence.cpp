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
 * $Id: FunctionSubsequence.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/functions/FunctionSubsequence.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/runtime/ClosureResult.hpp>

XERCES_CPP_NAMESPACE_USE;

const XMLCh FunctionSubsequence::name[] = {
  chLatin_s, chLatin_u, chLatin_b, 
  chLatin_s, chLatin_e, chLatin_q, 
  chLatin_u, chLatin_e, chLatin_n, 
  chLatin_c, chLatin_e, chNull 
};
const unsigned int FunctionSubsequence::minArgs = 2;
const unsigned int FunctionSubsequence::maxArgs = 3;

/**
 * fn:subsequence($sourceSeq as item()*, $startingLoc as xs:double) as item()*
 * fn:subsequence($sourceSeq as item()*, $startingLoc as xs:double, $length as xs:double) as item()*
**/

FunctionSubsequence::FunctionSubsequence(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*, double, double", args, memMgr)
{
}

ASTNode* FunctionSubsequence::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionSubsequence::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  ASTNode *result = calculateSRCForArguments(context);
  if(result == this) {
    _src.getStaticType() = _args.front()->getStaticAnalysis().getStaticType();
    _src.getStaticType().setCardinality(0, _src.getStaticType().getMax());
  }
  return result;
}

class SingleArgSubsequenceResult : public ResultImpl
{
public:
  SingleArgSubsequenceResult(const FunctionSubsequence *func)
    : ResultImpl(func),
      _func(func)
  {}

  Item::Ptr nextOrTail(Result &tail, DynamicContext *context)
  {
    Numeric::Ptr one = context->getItemFactory()->createDouble(1, context);
    Result source = ClosureResult::create(_func->getArguments()[0], context);

    Numeric::Ptr i = one;
    Numeric::Ptr position = ((Numeric*)_func->getParamNumber(2, context)->next(context).get())->round(context);

    while(i->lessThan(position, context) && source->next(context).notNull()) {
      i = i->add(one, context);
    }

    tail = source;
    return 0;
  }

private:
  const FunctionSubsequence *_func;
};

Result FunctionSubsequence::createResult(DynamicContext* context, int flags) const
{
  if(getNumArgs() == 2)
    return new SingleArgSubsequenceResult(this);
  return new SubsequenceResult(this);
}

FunctionSubsequence::SubsequenceResult::SubsequenceResult(const FunctionSubsequence *func)
  : ResultImpl(func),
    _func(func),
    _end(0),
    _one(0),
    _i(0),
    _source(0)
{
}

Item::Ptr FunctionSubsequence::SubsequenceResult::next(DynamicContext *context)
{
  if(_one.isNull()) {
    _one = context->getItemFactory()->createDouble(1, context);
    _source = _func->getParamNumber(1, context);

    _i = _one;
    const Numeric::Ptr position = ((const Numeric::Ptr )_func->getParamNumber(2, context)->next(context))->round(context);

    while(_i->lessThan(position, context) && _source->next(context) != NULLRCP) {
      _i = _i->add(_one, context);
    }

    _end = ((const Numeric::Ptr )_func->getParamNumber(3, context)->next(context))->round(context)
      ->add(position, context);
  }

  if(!_i->lessThan(_end, context)) {
    return 0;
  }

  _i = _i->add(_one, context);
  return _source->next(context);
}

