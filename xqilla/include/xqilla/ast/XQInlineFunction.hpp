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
 * $Id: XQInlineFunction.hpp 680 2008-11-24 16:02:25Z jpcs $
 */

#ifndef XQINLINEFUNCTION_HPP
#define XQINLINEFUNCTION_HPP

#include <xqilla/ast/ASTNodeImpl.hpp>

class XQUserFunction;
class InlineFunctionResult;

class XQILLA_API XQInlineFunction : public ASTNodeImpl
{
public:
  XQInlineFunction(XQUserFunction *func, XPath2MemoryManager *mm);
  XQInlineFunction(XQUserFunction *func, unsigned int numArgs, ASTNode *instance, XPath2MemoryManager *mm);

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;

  unsigned int getNumArgs() const { return numArgs_; }

  XQUserFunction *getUserFunction() const { return func_; }
  void setUserFunction(XQUserFunction *func) { func_ = func; }

  ASTNode *getInstance() const { return instance_; }
  void setInstance(ASTNode *i) { instance_ = i; }

private:
  XQUserFunction *func_;
  unsigned int numArgs_;
  ASTNode *instance_;

  friend class InlineFunctionResult;
};

#endif
