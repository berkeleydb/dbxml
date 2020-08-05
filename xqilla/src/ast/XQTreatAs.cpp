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
 * $Id: XQTreatAs.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/ast/XQFunctionDeref.hpp>
#include <xqilla/ast/XQInlineFunction.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/context/MessageListener.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/functions/XQUserFunction.hpp>
#include <xqilla/context/impl/VarStoreImpl.hpp>
#include "../items/impl/FunctionRefImpl.hpp"

#include <xercesc/validators/schema/SchemaSymbols.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace std;

const XMLCh XQTreatAs::err_XPDY0050[] = { 'e', 'r', 'r', ':', 'X', 'P', 'D', 'Y', '0', '0', '5', '0', 0 };
const XMLCh XQTreatAs::err_XPTY0004[] = { 'e', 'r', 'r', ':', 'X', 'P', 'T', 'Y', '0', '0', '0', '4', 0 };
const XMLCh XQTreatAs::funcVarName[] = { '#', 'f', 'u', 'n', 'c', 'V', 'a', 'r', 0 };

XQTreatAs::XQTreatAs(ASTNode* expr, SequenceType* exprType, XPath2MemoryManager* memMgr, const XMLCh *errorCode)
  : ASTNodeImpl(TREAT_AS, memMgr),
    _expr(expr),
    _exprType(exprType),
    _errorCode(errorCode),
    _doTypeCheck(true),
    _doCardinalityCheck(true),
    _funcConvert(0),
    _isExact(false)
{
  if(_errorCode == 0) _errorCode = err_XPTY0004;
}

XQTreatAs::XQTreatAs(ASTNode* expr, SequenceType *exprType, const XMLCh *errorCode, bool doTypeCheck, bool doCardinalityCheck,
                     ASTNode *funcConvert, const StaticType &treatType, bool isExact, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(TREAT_AS, memMgr),
    _expr(expr),
    _exprType(exprType),
    _errorCode(errorCode),
    _doTypeCheck(doTypeCheck),
    _doCardinalityCheck(doCardinalityCheck),
    _funcConvert(funcConvert),
    _treatType(treatType),
    _isExact(isExact)
{
}

ASTNode* XQTreatAs::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  _exprType->staticResolution(context);

  if(_exprType->getOccurrenceIndicator() == SequenceType::QUESTION_MARK ||
     _exprType->getOccurrenceIndicator() == SequenceType::EXACTLY_ONE) {
    AutoNodeSetOrderingReset orderReset(context);
    _expr = _expr->staticResolution(context);
  }
  else {
    _expr = _expr->staticResolution(context);
  }

  _exprType->getStaticType(_treatType, context, _isExact, this);

  const SequenceType::ItemType *type = _exprType->getItemType();

  if(type && type->getItemTestType() == SequenceType::ItemType::TEST_FUNCTION &&
     type->getReturnType() != 0) {
    // Construct an XQInlineFunction that will convert a function reference
    // stored in a variable to the correct type, and will throw type errors
    // if it isn't the correct type

    // Simultaneously create the XQInlineFunction parameter spec and the
    // XQFunctionDeref argument list
    XQUserFunction::ArgumentSpecs *paramList = new (mm) XQUserFunction::ArgumentSpecs(XQillaAllocator<XQUserFunction::ArgumentSpec*>(mm));
    VectorOfASTNodes *argList = new (mm) VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm));

    VectorOfSequenceTypes *argTypes = type->getArgumentTypes();
    unsigned int count = 0;
    for(VectorOfSequenceTypes::iterator i = argTypes->begin(); i != argTypes->end(); ++i) {
      XMLBuffer buf(20);
      buf.set(FunctionRefImpl::argVarPrefix);
      XPath2Utils::numToBuf(count, buf);
      ++count;
      const XMLCh *argName = mm->getPooledString(buf.getRawBuffer());

      XQUserFunction::ArgumentSpec *argSpec = new (mm) XQUserFunction::ArgumentSpec(argName, *i, mm);
      argSpec->setLocationInfo(*i);
      paramList->push_back(argSpec);

      XQVariable *argVar = new (mm) XQVariable(0, argName, mm);
      argVar->setLocationInfo(this);
      argList->push_back(argVar);
    }

    XQVariable *funcVar = new (mm) XQVariable(0, funcVarName, mm);
    funcVar->setLocationInfo(this);

    XQFunctionDeref *body = new (mm) XQFunctionDeref(funcVar, argList, mm);
    body->setLocationInfo(this);

    XQUserFunction *func = new (mm) XQUserFunction(0, paramList, body, type->getReturnType(), false, false, mm);
    func->setLocationInfo(this);

    _funcConvert = new (mm) XQInlineFunction(func, mm);
    _funcConvert->setLocationInfo(this);

    _funcConvert = _funcConvert->staticResolution(context);
  }

  return this;
}

ASTNode *XQTreatAs::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  // Do as much static time type checking as we can, given the
  // limited static typing that we implement
  const StaticType &actualType = _expr->getStaticAnalysis().getStaticType();

  StaticType::TypeMatch match = _treatType.matches(actualType);

  // Get a better static type by looking at our expression's type too
  _src.getStaticType() = _treatType;
  _src.getStaticType() &= actualType;

  _src.setProperties(_expr->getStaticAnalysis().getProperties());
  _src.add(_expr->getStaticAnalysis());

  if(!_expr->getStaticAnalysis().isUpdating()) {
    if(match.type == StaticType::NEVER || match.cardinality == StaticType::NEVER) {
      // It never matches
      XMLBuffer buf;
      buf.set(X("Expression does not match type "));
      _exprType->toBuffer(buf);
      buf.append(X(" - the expression has a static type of "));
      actualType.typeToBuf(buf);
      buf.append(X(" ["));
      buf.append(_errorCode);
      buf.append(X("]"));
      XQThrow(XPath2TypeMatchException, X("XQTreatAs::staticTyping"), buf.getRawBuffer());
    }

    MessageListener *mlistener = context ? context->getMessageListener() : 0;
    if(mlistener && context->getDoLintWarnings() && _errorCode != err_XPDY0050) {
      if(match.type == StaticType::PROBABLY_NOT || match.cardinality == StaticType::PROBABLY_NOT) {
        // It might not match
        XMLBuffer buf;
        buf.set(X("The expression might not match type "));
        _exprType->toBuffer(buf);
        buf.append(X(" - the expression has a static type of "));
        actualType.typeToBuf(buf);
        mlistener->warning(buf.getRawBuffer(), this);
      }
    }

    if(_isExact && match.type == StaticType::ALWAYS) {
      _doTypeCheck = false;
    }
    if(match.cardinality == StaticType::ALWAYS) {
      _doCardinalityCheck = false;
    }

    if(!_doTypeCheck && !_doCardinalityCheck) {
      // It always matches
      return substitute(_expr);
    }
  }

  return this;
}

ASTNode *XQTreatAs::getExpression() const {
  return _expr;
}

SequenceType *XQTreatAs::getSequenceType() const {
  return _exprType;
}

void XQTreatAs::setExpression(ASTNode *item) {
  _expr = item;
}

class FunctionConversionResult : public ResultImpl
{
public:
  FunctionConversionResult(const Result &parent, const ASTNode *funcConvert, const LocationInfo *location)
    : ResultImpl(location),
      parent_(parent),
      funcConvert_(funcConvert)
  {
  }

  virtual Item::Ptr next(DynamicContext *context)
  {
    Item::Ptr item = parent_->next(context);

    if(item.notNull() && item->isFunction()) {
      XPath2MemoryManager *mm = context->getMemoryManager();

      VarStoreImpl scope(mm, context->getVariableStore());
      scope.setVar(0, XQTreatAs::funcVarName, item);

      AutoVariableStoreReset vsReset(context, &scope);

      // funcConvert_ only returns one item
      item = funcConvert_->createResult(context)->next(context);
    }

    return item;
  }

  string asString(DynamicContext *context, int indent) const { return ""; }

private:
  Result parent_;
  const ASTNode *funcConvert_;
};

Result XQTreatAs::createResult(DynamicContext* context, int flags) const
{
  Result result = _expr->createResult(context, flags);
  if(_doCardinalityCheck &&
     (_exprType->getOccurrenceIndicator() != SequenceType::STAR ||
      _exprType->getItemType() == NULL)) {
    result = _exprType->occurrenceMatches(result, this, _errorCode);
  }
  if(_doTypeCheck) {
    result = _exprType->typeMatches(result, this, _errorCode);

    if(_funcConvert) {
      result = new FunctionConversionResult(result, _funcConvert, this);
    }
  }
  return result;
}

