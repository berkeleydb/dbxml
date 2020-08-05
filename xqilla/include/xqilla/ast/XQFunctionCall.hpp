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
 * $Id: XQFunctionCall.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef XQFUNCTIONCALL_HPP
#define XQFUNCTIONCALL_HPP

#include <xqilla/ast/ASTNodeImpl.hpp>

class XQILLA_API XQFunctionCall : public ASTNodeImpl
{
public:
  XQFunctionCall(const XMLCh *qname, VectorOfASTNodes *args, XPath2MemoryManager *mm);

  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  virtual Result createResult(DynamicContext* context, int flags=0) const;

  const XMLCh *getQName() const { return qname_; }
  const VectorOfASTNodes *getArguments() const { return args_; }

private:
  const XMLCh *qname_;
  VectorOfASTNodes *args_;
};

#endif
