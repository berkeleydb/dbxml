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
 * $Id: FTUnaryNot.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTUnaryNot.hpp>
#include <xqilla/context/DynamicContext.hpp>

FTUnaryNot::FTUnaryNot(FTSelection *arg, XPath2MemoryManager *memMgr)
    : FTSelection(FTSelection::UNARY_NOT, memMgr),
      arg_(arg)
{
}

FTSelection *FTUnaryNot::staticResolution(StaticContext *context)
{
  arg_ = arg_->staticResolution(context);
  return this;
}

FTSelection *FTUnaryNot::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  arg_ = arg_->staticTyping(context, styper);
  src_.add(arg_->getStaticAnalysis());

  return this;
}

FTSelection *FTUnaryNot::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  newarg = new (mm) FTUnaryNot(newarg, mm);
  newarg->setLocationInfo(this);
  return newarg;
}

AllMatches::Ptr FTUnaryNot::execute(FTContext *ftcontext) const
{
  return new FTUnaryNotMatches(this, arg_->execute(ftcontext));
}

Match::Ptr FTUnaryNotMatches::next(DynamicContext *context)
{
  // TBD need to check for StringInclude / StringExclude contradictions

  if(toDo_) {
    toDo_ = false;

    Match::Ptr result = new Match();

    if(arg_.notNull()) {
      Match::Ptr match(0);
      while((match = arg_->next(context)).notNull()) {
        result->addStringExcludes(match->getStringIncludes());
        result->addStringIncludes(match->getStringExcludes());
      }
    }

    arg_ = 0;
    return result;
  }
  return 0;
}
