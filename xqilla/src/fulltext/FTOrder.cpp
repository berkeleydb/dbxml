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
 * $Id: FTOrder.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTOrder.hpp>
#include <xqilla/context/DynamicContext.hpp>

FTSelection *FTOrder::staticResolution(StaticContext *context)
{
  arg_ = arg_->staticResolution(context);
  return this;
}

FTSelection *FTOrder::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  arg_ = arg_->staticTyping(context, styper);
  src_.add(arg_->getStaticAnalysis());

  return this;
}

FTSelection *FTOrder::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  if(newarg->getType() == WORD) {
    return newarg;
  }

  newarg = new (mm) FTOrder(newarg, mm);
  newarg->setLocationInfo(this);
  return newarg;
}

AllMatches::Ptr FTOrder::execute(FTContext *ftcontext) const
{
  return new FTOrderMatches(this, arg_->execute(ftcontext));
}

Match::Ptr FTOrderMatches::next(DynamicContext *context)
{
  if(arg_.isNull()) return 0;

  Match::Ptr match(0);
  while(match.isNull()) {
    match = arg_->next(context);
    if(match.isNull()) {
      arg_ = 0;
      return 0;
    }

    StringMatches::const_iterator begin = match->getStringIncludes().begin();
    StringMatches::const_iterator end = match->getStringIncludes().end();
    StringMatches::const_iterator i, j;
    for(i = begin; i != end && match.notNull(); ++i) {
      for(j = i, ++j; j != end; ++j) {
        if((i->queryPos > j->queryPos && i->tokenInfo->getPosition() < j->tokenInfo->getPosition()) ||
           (i->queryPos < j->queryPos && i->tokenInfo->getPosition() > j->tokenInfo->getPosition())) {
          match = 0;
          break;
        }
      }
    }
  }

  Match::Ptr result = new Match();
  result->addStringIncludes(match->getStringIncludes());

  StringMatches::const_iterator e_end = match->getStringExcludes().end();
  StringMatches::const_iterator e = match->getStringExcludes().begin();
  StringMatches::const_iterator i_begin = match->getStringIncludes().begin();
  StringMatches::const_iterator i_end = match->getStringIncludes().end();
  StringMatches::const_iterator i;
  for(; e != e_end; ++e) {
    for(i = i_begin; i != i_end; ++i) {
      if((i->queryPos <= e->queryPos && i->tokenInfo->getPosition() <= e->tokenInfo->getPosition()) ||
         (i->queryPos >= e->queryPos && i->tokenInfo->getPosition() >= e->tokenInfo->getPosition())) {
        result->addStringExclude(*e);
      }
    }
  }

  return result;
}
