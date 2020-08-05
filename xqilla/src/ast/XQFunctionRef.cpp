/*
 * Copyright (c) 2001-2007
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2007
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
 * $Id: XQFunctionRef.cpp 681 2008-11-25 10:57:27Z jpcs $
 */

#include <xqilla/ast/XQFunctionRef.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/runtime/SingleResult.hpp>
#include <xqilla/context/VariableTypeStore.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/exceptions/StaticErrorException.hpp>

#include "../items/impl/FunctionRefImpl.hpp"

XERCES_CPP_NAMESPACE_USE;
using namespace std;

XQFunctionRef::XQFunctionRef(const XMLCh *qname, unsigned int numArgs, XPath2MemoryManager *mm)
  : ASTNodeImpl(FUNCTION_REF, mm),
    qname_(qname),
    uri_(0),
    name_(0),
    numArgs_(numArgs),
    instance_(0)
{
}

XQFunctionRef::XQFunctionRef(const XMLCh *uri, const XMLCh *name, unsigned int numArgs, ASTNode *instance, XPath2MemoryManager *mm)
  : ASTNodeImpl(FUNCTION_REF, mm),
    qname_(0),
    uri_(uri),
    name_(name),
    numArgs_(numArgs),
    instance_(instance)
{
}

ASTNode *XQFunctionRef::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  prefix_ = XPath2NSUtils::getPrefix(qname_, mm);
  name_ = XPath2NSUtils::getLocalName(qname_);

  if(prefix_ == 0 || *prefix_ == 0) {
    uri_ = context->getDefaultFuncNS();
  }
  else {
    uri_ = context->getUriBoundToPrefix(prefix_, this);
  }

  instance_ = FunctionRefImpl::createInstance(uri_, name_, numArgs_, context, this);
  if(instance_ == 0) {
    XMLBuffer buf;
    buf.set(X("A function called {"));
    buf.append(uri_);
    buf.append(X("}"));
    buf.append(name_);
    buf.append(X(" with "));
    XPath2Utils::numToBuf(numArgs_, buf);
    buf.append(X(" arguments is not defined [err:XPST0017]"));

    XQThrow(StaticErrorException, X("XQFunctionRef::staticResolution"), buf.getRawBuffer());
  }

  instance_ = instance_->staticResolution(context);

  return this;
}

ASTNode *XQFunctionRef::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  // TBD Using getMemoryManager() might not be thread safe in DB XML - jpcs

  XPath2MemoryManager *mm = getMemoryManager();
  _src.getStaticType() = StaticType(mm, numArgs_, instance_->getStaticAnalysis().getStaticType());

  return this;
}

Result XQFunctionRef::createResult(DynamicContext *context, int flags) const
{
  return (Item::Ptr)new FunctionRefImpl(prefix_, uri_, name_,
                                        instance_, numArgs_, context);
}
