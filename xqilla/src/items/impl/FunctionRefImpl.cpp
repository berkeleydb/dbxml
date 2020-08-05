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
 * $Id: FunctionRefImpl.cpp 641 2008-09-01 14:02:54Z jpcs $
 */

#include "../../config/xqilla_config.h"
#include "FunctionRefImpl.hpp"
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/functions/FuncFactory.hpp>
#include <xqilla/context/impl/VarStoreImpl.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ContextHelpers.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/events/EventHandler.hpp>
#include <xqilla/runtime/ClosureResult.hpp>

XERCES_CPP_NAMESPACE_USE;
using namespace std;

XMLCh FunctionRefImpl::argVarPrefix[] = { '#', 'a', 'r', 'g', 0 };

FunctionRefImpl::FunctionRefImpl(const XMLCh *prefix, const XMLCh *uri, const XMLCh *localname,
                                 const ASTNode *instance, size_t numArgs, DynamicContext *context)
  : prefix_(prefix),
    uri_(uri),
    name_(localname),
    instance_(instance),
    varStore_(context->getMemoryManager())
{
  for(unsigned int i = 0; i < numArgs; ++i) {
    args_.push_back(i);
  }  
}

FunctionRefImpl::FunctionRefImpl(const ASTNode *instance, size_t numArgs, const StaticAnalysis &sa, DynamicContext *context)
  : prefix_(0),
    uri_(0),
    name_(0),
    instance_(instance),
    varStore_(context->getMemoryManager())
{
  // Copy the variables we need into our local storage
  varStore_.cacheVariableStore(sa, context->getVariableStore());

  for(unsigned int i = 0; i < numArgs; ++i) {
    args_.push_back(i);
  }  
}

FunctionRefImpl::FunctionRefImpl(const FunctionRefImpl *other, const Result &argument, unsigned int argNum, DynamicContext *context)
  : prefix_(other->prefix_),
    uri_(other->uri_),
    name_(other->name_),
    instance_(other->instance_),
    args_(other->args_),
    varStore_(other->varStore_, context->getMemoryManager())
{
  vector<unsigned int>::iterator argsIt = args_.begin();
  for(unsigned int i = 0; i < argNum; ++i) {
    ++argsIt;
  }

  XMLBuffer buf(20);
  buf.set(FunctionRefImpl::argVarPrefix);
  XPath2Utils::numToBuf(*argsIt, buf);

  args_.erase(argsIt);

  varStore_.setVar(0, context->getMemoryManager()->getPooledString(buf.getRawBuffer()), argument);
}

class FunctionRefScope : public VariableStore
{
public:
  FunctionRefScope(const FunctionRefImpl::Ptr func, const VectorOfResults &args, DynamicContext *context)
    : func_(func),
      scope_(context->getMemoryManager(), context->getVariableStore())
  {
    XPath2MemoryManager *mm = context->getMemoryManager();

    VectorOfResults::const_iterator i = args.begin();
    vector<unsigned int>::const_iterator argsIt = func_->args_.begin();
    for(; i != args.end(); ++i) {
      XMLBuffer buf(20);
      buf.set(FunctionRefImpl::argVarPrefix);
      XPath2Utils::numToBuf(*argsIt, buf);
      ++argsIt;

      scope_.setVar(0, mm->getPooledString(buf.getRawBuffer()), *i);
    }
  }

  Result getVar(const XMLCh *namespaceURI, const XMLCh *name) const
  {
    Result result = func_->varStore_.getVar(namespaceURI, name);
    if(!result.isNull()) return result;
    return scope_.getVar(namespaceURI, name);
  }

  void getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const
  {
    func_->varStore_.getInScopeVariables(variables);
    scope_.getInScopeVariables(variables);
  }

private:
  FunctionRefImpl::Ptr func_;
  VarStoreImpl scope_;
};

Result FunctionRefImpl::execute(const VectorOfResults &args, DynamicContext *context, const LocationInfo *location) const
{
  if(args.size() != getNumArgs()) {
    XMLBuffer buf;
    buf.set(X("The function item invoked does not accept "));
    XPath2Utils::numToBuf((unsigned int)args.size(), buf);
    buf.append(X(" arguments - found item of type "));
    typeToBuffer(context, buf);
    buf.append(X(" [err:TBD]"));
    XQThrow3(XPath2TypeMatchException, X("FunctionRefImpl::execute"), buf.getRawBuffer(), location);
  }

  FunctionRefScope scope(this, args, context);
  return ClosureResult::create(instance_, context, &scope);
}

ATQNameOrDerived::Ptr FunctionRefImpl::getName(const DynamicContext *context) const
{
  if(name_ == 0) return 0;
  return context->getItemFactory()->createQName(uri_, prefix_, name_, context);
}

FunctionRef::Ptr FunctionRefImpl::partialApply(const Result &arg, unsigned int argNum, DynamicContext *context, const LocationInfo *location) const
{
  if(getNumArgs() < argNum) {
    XMLBuffer buf;
    buf.set(X("The function item argument to fn:partial-apply() must have an arity of at least "));
    XPath2Utils::numToBuf(argNum, buf);
    buf.append(X(" - found item of type "));
    typeToBuffer(context, buf);
    buf.append(X(" [err:TBD]"));
    XQThrow3(XPath2TypeMatchException, X("FunctionRefImpl::partialApply"), buf.getRawBuffer(), location);
  }

  return new FunctionRefImpl(this, arg, argNum - 1, context);
}

void FunctionRefImpl::generateEvents(EventHandler *events, const DynamicContext *context,
                                     bool preserveNS, bool preserveType) const
{
  // TBD What is the correct way to generate events for a FunctionRef? - jpcs
  events->atomicItemEvent(AnyAtomicType::STRING, asString(context), 0, 0);
}

const XMLCh *FunctionRefImpl::asString(const DynamicContext *context) const
{
  XMLBuffer buf;
  buf.append(X("function(#"));
  XPath2Utils::numToBuf((unsigned int)getNumArgs(), buf);
  buf.append(')');
  return context->getMemoryManager()->getPooledString(buf.getRawBuffer());
}

void FunctionRefImpl::typeToBuffer(DynamicContext *context, XMLBuffer &buffer) const
{
  buffer.append(X("function("));
  XPath2Utils::numToBuf((unsigned int)getNumArgs(), buffer);
  buffer.append(X(")"));
}

void *FunctionRefImpl::getInterface(const XMLCh *name) const
{
  return 0;
}

ASTNode *FunctionRefImpl::createInstance(const FuncFactory *factory, unsigned int numArgs, XPath2MemoryManager *mm, const LocationInfo *location)
{
  VectorOfASTNodes newArgs = VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm));
  for(unsigned int i = 0; i < numArgs; ++i) {
    XMLBuffer buf(20);
    buf.set(argVarPrefix);
    XPath2Utils::numToBuf(i, buf);

    XQVariable *var = new (mm) XQVariable(0, mm->getPooledString(buf.getRawBuffer()), mm);
    var->setLocationInfo(location);
    newArgs.push_back(var);
  }

  ASTNode *instance = factory->createInstance(newArgs, mm);
  instance->setLocationInfo(location);
  return instance;
}

ASTNode *FunctionRefImpl::createInstance(const XMLCh *uri, const XMLCh *name, unsigned int numArgs, StaticContext *context, const LocationInfo *location)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  VectorOfASTNodes newArgs = VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm));
  for(unsigned int i = 0; i < numArgs; ++i) {
    XMLBuffer buf(20);
    buf.set(argVarPrefix);
    XPath2Utils::numToBuf(i, buf);

    XQVariable *var = new (mm) XQVariable(0, mm->getPooledString(buf.getRawBuffer()), mm);
    var->setLocationInfo(location);
    newArgs.push_back(var);
  }

  ASTNode *instance = context->lookUpFunction(uri, name, newArgs);
  if(instance) instance->setLocationInfo(location);
  return instance;
}
