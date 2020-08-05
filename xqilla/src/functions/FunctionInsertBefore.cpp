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
 * $Id: FunctionInsertBefore.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <sstream>

#include <xqilla/functions/FunctionInsertBefore.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/runtime/ClosureResult.hpp>

const XMLCh FunctionInsertBefore::name[] = { 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_i, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_n, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_s, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_t, 
  XERCES_CPP_NAMESPACE_QUALIFIER chDash,    XERCES_CPP_NAMESPACE_QUALIFIER chLatin_b, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_f, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_o, XERCES_CPP_NAMESPACE_QUALIFIER chLatin_r, 
  XERCES_CPP_NAMESPACE_QUALIFIER chLatin_e, XERCES_CPP_NAMESPACE_QUALIFIER chNull 
};
const unsigned int FunctionInsertBefore::minArgs = 3;
const unsigned int FunctionInsertBefore::maxArgs = 3;

/**
 * fn:insert-before($target as item()*, $position as xs:integer, $inserts as item()*) as item()*
 */

FunctionInsertBefore::FunctionInsertBefore(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
  : ConstantFoldingFunction(name, minArgs, maxArgs, "item()*, integer, item()*", args, memMgr)
{
}

ASTNode* FunctionInsertBefore::staticResolution(StaticContext *context)
{
  return resolveArguments(context);
}

ASTNode *FunctionInsertBefore::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  ASTNode *result = calculateSRCForArguments(context);
  if(result == this) {
    _src.getStaticType() = _args[0]->getStaticAnalysis().getStaticType();
    _src.getStaticType().typeConcat(_args[2]->getStaticAnalysis().getStaticType());
  }
  return result;
}

class InsertBeforeResult : public ResultImpl
{
public:
  InsertBeforeResult(const FunctionInsertBefore *func)
    : ResultImpl(func),
      _func(func),
      _position(0),
      _one(0),
      _i(0),
      _doInserts(false),
      _target(0),
      _inserts(0)
  {
  }

  virtual Item::Ptr nextOrTail(Result &tail, DynamicContext *context)
  {
    if(_position.isNull()) {
      _position = (const ATDecimalOrDerived*)_func->getParamNumber(2, context)->next(context).get();
      _one = context->getItemFactory()->createInteger(1, context);
      _i = _one;
      _target = ClosureResult::create(_func->getArguments()[0], context);

      if(!_position->greaterThan(_one, context)) {
        _inserts = _func->getParamNumber(3, context);
        _doInserts = true;
      }
    }

    Item::Ptr result = 0;
    while(result.isNull()) {
      if(_doInserts) {
        result = _inserts->next(context);
        if(result.isNull()) {
          // Tail call optimisation
          tail = _target;
          return 0;
        }
      }
      else {
        result = _target->next(context);
        _i = _i->add(_one, context);
        if(result.isNull()) {
          // Tail call optimisation
          tail = ClosureResult::create(_func->getArguments()[2], context);
          return 0;
        }
        else if(_position->equals((const AnyAtomicType*)_i.get(), context)) {
          _inserts = _func->getParamNumber(3, context);
          _doInserts = true;
        }
      }
    }

    return result;
  }

private:
  const FunctionInsertBefore *_func;
  Numeric::Ptr _position;
  Numeric::Ptr _one;
  Numeric::Ptr _i;
  bool _doInserts;
  Result _target;
  Result _inserts;
};

Result FunctionInsertBefore::createResult(DynamicContext* context, int flags) const
{
  return new InsertBeforeResult(this);
}

