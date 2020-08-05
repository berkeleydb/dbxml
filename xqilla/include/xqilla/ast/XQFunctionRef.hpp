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
 * $Id: XQFunctionRef.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef XQFUNCTIONREF_HPP
#define XQFUNCTIONREF_HPP

#include <xqilla/ast/ASTNodeImpl.hpp>

class XQFunctionRefResult;

class XQILLA_API XQFunctionRef : public ASTNodeImpl
{
public:
  XQFunctionRef(const XMLCh *qname, unsigned int numArgs, XPath2MemoryManager *mm);
  XQFunctionRef(const XMLCh *uri, const XMLCh *name, unsigned int numArgs, ASTNode *instance, XPath2MemoryManager *mm);

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;

  const XMLCh *getQName() const { return qname_; }
  const XMLCh *getURI() const { return uri_; }
  const XMLCh *getName() const { return name_; }
  unsigned int getNumArgs() const { return numArgs_; }

  ASTNode *getInstance() const { return instance_; }
  void setInstance(ASTNode *i) { instance_ = i; }

private:
  const XMLCh *qname_, *prefix_, *uri_, *name_;
  unsigned int numArgs_;
  ASTNode *instance_;

  friend class XQFunctionRefResult;
};

#endif
