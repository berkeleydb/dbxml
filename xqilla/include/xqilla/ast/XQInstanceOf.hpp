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
 * $Id: XQInstanceOf.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef _XQINSTANCEOF_HPP
#define _XQINSTANCEOF_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class SequenceType;

class XQILLA_API XQInstanceOf : public ASTNodeImpl
{
public:
  XQInstanceOf(ASTNode* expr, SequenceType* exprType, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  ASTNode *getExpression() const;
  SequenceType *getSequenceType() const;

  void setExpression(ASTNode *item);

protected:
  class InstanceOfResult : public SingleResult
  {
  public:
    InstanceOfResult(const XQInstanceOf *di, int flags);

    Item::Ptr getSingleResult(DynamicContext *context) const;

  private:
    int _flags;
    const XQInstanceOf *_di;
  };

  ASTNode* _expr;
  SequenceType* _exprType;
};

#endif
