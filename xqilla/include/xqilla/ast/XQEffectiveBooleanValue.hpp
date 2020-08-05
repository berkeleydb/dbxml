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
 * $Id: XQAtomize.hpp 587 2008-08-13 13:01:08Z jpcs $
 */

#ifndef _XQEFFECTIVEBOOLEANVALUE_HPP
#define _XQEFFECTIVEBOOLEANVALUE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>

class XQILLA_API XQEffectiveBooleanValue : public ASTNodeImpl
{
public:
  XQEffectiveBooleanValue(ASTNode* expr, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }

  /// Returns the effective boolean value of the sequence starting with the two items given
  static bool get(const Item::Ptr &first, const Item::Ptr &second, DynamicContext* context, const LocationInfo *info);

protected:
  ASTNode* expr_;
};

class XQILLA_API EffectiveBooleanValueResult : public ResultImpl
{
public:
  EffectiveBooleanValueResult(const LocationInfo *location, const Result &parent)
    : ResultImpl(location), parent_(parent) {}
  virtual Item::Ptr nextOrTail(Result &tail, DynamicContext *context);
private:
  Result parent_;
};

#endif
