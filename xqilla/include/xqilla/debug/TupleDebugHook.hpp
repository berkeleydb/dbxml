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
 * $Id: ForTuple.hpp 531 2008-04-10 23:23:07Z jpcs $
 */

#ifndef TUPLEDEBUGHOOK_HPP
#define TUPLEDEBUGHOOK_HPP

#include <xqilla/ast/TupleNode.hpp>

class XQILLA_API TupleDebugHook : public TupleNode
{
public:
  TupleDebugHook(TupleNode *parent, XPath2MemoryManager *mm);

  virtual TupleNode *staticResolution(StaticContext *context);
  virtual TupleNode *staticTypingImpl(StaticContext *context);
  virtual TupleNode *staticTypingTeardown(StaticContext *context, StaticAnalysis &usedSrc);

  virtual TupleResult::Ptr createResult(DynamicContext* context) const;
};

#endif
