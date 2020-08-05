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
 * $Id: XQGlobalVariable.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/runtime/Sequence.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/VariableStore.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/functions/XQUserFunction.hpp>

XQGlobalVariable::XQGlobalVariable(const XMLCh* varQName, SequenceType* seqType, ASTNode* value, XPath2MemoryManager *mm, bool isParam)
  : isParam_(isParam),
    required_(!isParam),
    xpath1Compat_(false),
    m_szQName(mm->getPooledString(varQName)),
    m_szURI(0),
    m_szLocalName(0),
    m_Type(seqType),
    m_Value(value),
    _src(mm),
    staticTyped_(false)
{
}

static const XMLCh err_XPTY0004[] = { 'e', 'r', 'r', ':', 'X', 'P', 'T', 'Y', '0', '0', '0', '4', 0 };

void XQGlobalVariable::execute(DynamicContext* context) const
{
  try {
    if(m_Value == NULL || isParam_) {
      // It's an external declaration, so check the user has set the value in the variable store
      Result value = context->getGlobalVariableStore()->getVar(m_szURI, m_szLocalName);
      if(!value.isNull()) {
        if(m_Type != NULL) {
          if(isParam_) {
            // Convert the external value using the function conversion rules
            Result matchesRes = m_Type->convertFunctionArg(value, context, xpath1Compat_, m_Type, err_XPTY0004);
            context->setExternalVariable(m_szURI, m_szLocalName, matchesRes->toSequence(context));
          }
          else {
            // Check the external value's type
            Result matchesRes = m_Type->matches(value, m_Type, err_XPTY0004);
            while(matchesRes->next(context).notNull()) {}
          }
        }

        return;
      }

      if(m_Value == NULL) {
        XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
        errMsg.set(X("A value for the external variable "));
        if(m_szQName != 0) {
          errMsg.append(m_szQName);
        }
        else {
          errMsg.append('{');
          errMsg.append(m_szURI);
          errMsg.append('}');
          errMsg.append(m_szLocalName);
        }
        errMsg.append(X(" has not been provided [err:XPTY0002]"));
        XQThrow(IllegalArgumentException,X("XQGlobalVariable::createSequence"),errMsg.getRawBuffer());
      }
    }

    // TBD Could use our own VariableStore implementation - jpcs
    // TBD Use a closure rather than toSequence() - jpcs
    context->setExternalVariable(m_szURI, m_szLocalName, m_Value->createResult(context)->
                                 toSequence(context));
  }
  catch(const XPath2TypeMatchException &ex) {
    XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer errMsg;
    errMsg.set(X("The value for the global variable "));
    if(m_szQName != 0) {
      errMsg.append(m_szQName);
    }
    else {
      errMsg.append('{');
      errMsg.append(m_szURI);
      errMsg.append('}');
      errMsg.append(m_szLocalName);
    }
    errMsg.append(X(" does not match the declared type: "));
    errMsg.append(ex.getError());
    XQThrow(XPath2TypeMatchException,X("XQGlobalVariable::createSequence"),errMsg.getRawBuffer());
  }
}

void XQGlobalVariable::staticResolution(StaticContext* context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  xpath1Compat_ = context->getXPath1CompatibilityMode();

  if(m_Type) m_Type->staticResolution(context);

  // variables with no prefix are in no namespace
  if(m_szLocalName == 0) {
    const XMLCh* prefix=XPath2NSUtils::getPrefix(m_szQName, mm);
    if(prefix && *prefix)
      m_szURI = context->getUriBoundToPrefix(prefix, this);
    m_szLocalName = XPath2NSUtils::getLocalName(m_szQName);
  }

  if(m_Value != NULL) {
    if(m_Type != NULL) {
      m_Value = new (mm) XQTreatAs(m_Value, m_Type, mm);
      m_Value->setLocationInfo(this);
    }
    m_Value = m_Value->staticResolution(context);
  }
}

void XQGlobalVariable::staticTypingOnce(StaticContext* context, StaticTyper *styper)
{
  if(staticTyped_) return;
  staticTyped_ = true;
  staticTyping(context, styper);
}

void XQGlobalVariable::staticTyping(StaticContext* context, StaticTyper *styper)
{
  VariableTypeStore* varStore = context->getVariableTypeStore();

  if(m_Value != NULL) {
    XQUserFunction::staticTypeFunctionCalls(m_Value, context, styper);

    m_Value = m_Value->staticTyping(context, styper);
    _src.copy(m_Value->getStaticAnalysis());

    if(m_Value->getStaticAnalysis().isUpdating()) {
      XQThrow(StaticErrorException,X("XQGlobalVariable::staticTyping"),
              X("It is a static error for the initializing expression of a global variable "
                "to be an updating expression [err:XUST0001]"));
    }
  }

  if(m_Value == 0 || !required_) {
    if(m_Type != 0) {
      bool isPrimitive;
      m_Type->getStaticType(_src.getStaticType(), context, isPrimitive, m_Type);
    }
    else {
      _src.getStaticType() = StaticType(StaticType::ITEM_TYPE, 0, StaticType::UNLIMITED);
    }
  }

  varStore->declareGlobalVar(m_szURI, m_szLocalName, _src);
}

const XMLCh* XQGlobalVariable::getVariableName() const
{
  return m_szQName;
}

bool XQGlobalVariable::isExternal() const
{
  return (m_Value==NULL);
}

void XQGlobalVariable::setVariableExpr(ASTNode* value)
{
  m_Value=value;
}

const SequenceType *XQGlobalVariable::getSequenceType() const
{
  return m_Type;
}

const ASTNode *XQGlobalVariable::getVariableExpr() const
{
  return m_Value;
}

const XMLCh *XQGlobalVariable::getVariableURI() const
{
  return m_szURI;
}

const XMLCh *XQGlobalVariable::getVariableLocalName() const
{
  return m_szLocalName;
}
