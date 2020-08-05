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
 * $Id: XQInstanceOf.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/ast/XQInstanceOf.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/DatatypeFactory.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/context/ContextHelpers.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XQInstanceOf::XQInstanceOf(ASTNode* expr, SequenceType* exprType, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(INSTANCE_OF, memMgr),
  _expr(expr),
  _exprType(exprType)
{
}

Result XQInstanceOf::createResult(DynamicContext* context, int flags) const
{
  return new InstanceOfResult(this, flags);
}

ASTNode* XQInstanceOf::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  _exprType->staticResolution(context);

  _expr = new (mm) XQTreatAs(_expr, _exprType, mm);
  _expr->setLocationInfo(this);

  {
    AutoNodeSetOrderingReset orderReset(context);
    _expr = _expr->staticResolution(context);
  }

  return this;
}

ASTNode *XQInstanceOf::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.add(_expr->getStaticAnalysis());
  _src.getStaticType() = StaticType::BOOLEAN_TYPE;

  return this;
}

ASTNode *XQInstanceOf::getExpression() const {
  return _expr;
}

SequenceType *XQInstanceOf::getSequenceType() const {
  return _exprType;
}

void XQInstanceOf::setExpression(ASTNode *item) {
  _expr = item;
}

XQInstanceOf::InstanceOfResult::InstanceOfResult(const XQInstanceOf *di, int flags)
  : SingleResult(di),
    _flags(flags),
    _di(di)
{
}

Item::Ptr XQInstanceOf::InstanceOfResult::getSingleResult(DynamicContext *context) const
{
  try {
    Result result = _di->getExpression()->createResult(context);
    while(result->next(context).notNull()) {}
  }
  catch(const XPath2TypeMatchException &ex) {
    return (const Item::Ptr)context->getItemFactory()->createBoolean(false, context);
  }
  ATBooleanOrDerived::Ptr ptr = context->getItemFactory()->createBoolean(true, context);
  return Item::Ptr(ptr.get());
}

