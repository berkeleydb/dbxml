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
 * $Id: EventGenerator.hpp 639 2008-09-01 13:22:54Z jpcs $
 */

#ifndef _EVENTGENERATOR_HPP
#define _EVENTGENERATOR_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>
#include <xqilla/runtime/ResultImpl.hpp>

class DynamicContext;
class EventHandler;
class ASTNode;
class QueryPathNode;

class XQILLA_API EventGenerator : public ReferenceCounted
{
public:
  typedef RefCountPointer<EventGenerator> Ptr;

  virtual ~EventGenerator() {}

  /** Returns the result of this expression via the EventHandler provided.
      An EventGenerator may be returned to be called as a tail call optimization */
  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context) = 0;

  static void generateAndTailCall(const EventGenerator::Ptr &generator, EventHandler *events,
                                  DynamicContext *context)
  {
    EventGenerator::Ptr gen = generator;
    while(gen.notNull()) {
      gen = gen->generateEvents(events, context);
    }
  }

};

class XQILLA_API ASTNodeEventGenerator : public EventGenerator
{
public:
  ASTNodeEventGenerator(const ASTNode *ast, bool preserveNS, bool preserveType);

  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context);

protected:
  const ASTNode *ast_;
  bool preserveNS_, preserveType_;
};

class XQILLA_API GenerateEventsResult : public ResultImpl
{
public:
  GenerateEventsResult(const ASTNode *ast, const QueryPathNode *projection);

  virtual Item::Ptr nextOrTail(Result &tail, DynamicContext *context);

private:
  const ASTNode *ast_;
  const QueryPathNode *projection_;
};

#endif
