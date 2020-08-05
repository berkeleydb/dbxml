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
 * $Id: XQIf.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef _XQIF_HPP
#define _XQIF_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>

/** If function. */
class XQILLA_API XQIf : public ASTNodeImpl
{
public:
  XQIf(ASTNode* test, ASTNode* whenTrue, ASTNode* whenFalse, XPath2MemoryManager* memMgr);
  
  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context,
                                               bool preserveNS, bool preserveType) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  virtual PendingUpdateList createUpdateList(DynamicContext *context) const;

  ASTNode *getTest() const;
  ASTNode *getWhenTrue() const;
  ASTNode *getWhenFalse() const;

  void setTest(ASTNode *item);
  void setWhenTrue(ASTNode *item);
  void setWhenFalse(ASTNode *item);

protected:
  class IfResult : public ResultImpl
  {
  public:
    IfResult(const XQIf *di);

    Item::Ptr nextOrTail(Result &tail, DynamicContext *context);

  private:
    const XQIf *_di;
  };

  ASTNode* _test,* _whenTrue,* _whenFalse;
};

#endif


