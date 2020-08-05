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
 * $Id: XQCopyOf.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef XQCOPYOF_HPP
#define XQCOPYOF_HPP

#include <xqilla/ast/ASTNodeImpl.hpp>

class XQILLA_API XQCopyOf : public ASTNodeImpl
{
public:
  XQCopyOf(XPath2MemoryManager* memMgr);
  XQCopyOf(ASTNode *expr, bool copyNamespaces, XPath2MemoryManager* memMgr);

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context,
                                        bool preserveNS, bool preserveType) const;

  ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *item) { expr_ = item; }

  bool getCopyNamespaces() const { return copyNamespaces_; }
  void setCopyNamespaces(bool value) { copyNamespaces_ = value; }

protected:
  ASTNode *expr_;
  bool copyNamespaces_;
};

#endif
