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
 * $Id: XPath1Compat.cpp 659 2008-10-06 00:11:22Z jpcs $
 */

#include "../config/xqilla_config.h"

#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/functions/FunctionString.hpp>
#include <xqilla/functions/FunctionNumber.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

XPath1CompatConvertFunctionArg::XPath1CompatConvertFunctionArg(ASTNode* expr, SequenceType *seqType, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(XPATH1_CONVERT, memMgr),
    expr_(expr),
    seqType_(seqType)
{
}

ASTNode* XPath1CompatConvertFunctionArg::staticResolution(StaticContext *context)
{
  seqType_->staticResolution(context);
  expr_ = expr_->staticResolution(context);
  return this;
}

ASTNode *XPath1CompatConvertFunctionArg::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.copy(expr_->getStaticAnalysis());

  unsigned int min = _src.getStaticType().getMin() == 0 ? 0 : 1;
  _src.getStaticType().setCardinality(min, 1);

  if(seqType_->getItemTestType() == SequenceType::ItemType::TEST_ATOMIC_TYPE) {
    const XMLCh* typeURI = seqType_->getTypeURI();
    const XMLCh* typeName = seqType_->getConstrainingType()->getName();

    if(XPath2Utils::equals(typeName, SchemaSymbols::fgDT_STRING) &&
       XPath2Utils::equals(typeURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
      _src.getStaticType() = StaticType::STRING_TYPE;
      _src.setProperties(0);
    }
    else if(XPath2Utils::equals(typeName, SchemaSymbols::fgDT_DOUBLE) &&
            XPath2Utils::equals(typeURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
      _src.getStaticType() = StaticType::DOUBLE_TYPE;
      _src.setProperties(0);
    }
  }

  return this;
}

Result XPath1CompatConvertFunctionArg::createResult(DynamicContext* context, int flags) const
{
  return new XPath1CompatConvertFunctionArgResult(this, expr_->createResult(context, flags), seqType_);
}

XPath1CompatConvertFunctionArgResult::
XPath1CompatConvertFunctionArgResult(const LocationInfo *location, const Result &parent, const SequenceType *seqType)
  : ResultImpl(location),
    seqType_(seqType),
    parent_(parent),
    oneDone_(false)
{
}

Item::Ptr XPath1CompatConvertFunctionArgResult::next(DynamicContext *context)
{
  // If XPath 1.0 compatibility mode is true and an argument is not of the expected type, then the following
  // conversions are applied sequentially to the argument value V:

  // 1. If the expected type calls for a single item or optional single item (examples: xs:string,
  //    xs:string?, xdt:untypedAtomic, xdt:untypedAtomic?, node(), node()?, item(), item()?), then the
  //    value V is effectively replaced by V[1].
  if(oneDone_) {
    return 0;
  }

  Item::Ptr item = parent_->next(context);

  if(seqType_->getItemTestType() == SequenceType::ItemType::TEST_ATOMIC_TYPE) {
    const XMLCh* typeURI = seqType_->getTypeURI();
    const XMLCh* typeName = seqType_->getConstrainingType()->getName();

    // 2. If the expected type is xs:string or xs:string?, then the value V is effectively replaced by
    //    fn:string(V).
    if(XPath2Utils::equals(typeName, SchemaSymbols::fgDT_STRING) &&
       XPath2Utils::equals(typeURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
      item = FunctionString::string_item(item, context);
    }

    // 3. If the expected type is xs:double or xs:double?, then the value V is effectively replaced by
    //    fn:number(V).
    else if(XPath2Utils::equals(typeName, SchemaSymbols::fgDT_DOUBLE) &&
            XPath2Utils::equals(typeURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA)) {
      item = FunctionNumber::number(item, context, this);
    }
  }

  oneDone_ = true;
  return item;
}

std::string XPath1CompatConvertFunctionArgResult::asString(DynamicContext *context, int indent) const
{
  return "xpath1convertfunctionarg";
}
