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
 * $Id: FTMildnot.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTMildnot.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>

FTMildnot::FTMildnot(FTSelection *left, FTSelection *right, XPath2MemoryManager *memMgr)
    : FTSelection(FTSelection::MILD_NOT, memMgr),
      left_(left),
      right_(right)
{
}

FTSelection *FTMildnot::staticResolution(StaticContext *context)
{
  left_ = left_->staticResolution(context);
  right_ = right_->staticResolution(context);
  return this;
}

FTSelection *FTMildnot::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  left_ = left_->staticTyping(context, styper);
  src_.add(left_->getStaticAnalysis());

  right_ = right_->staticTyping(context, styper);
  src_.add(right_->getStaticAnalysis());

  return this;
}

FTSelection *FTMildnot::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newleft = left_->optimize(ftcontext, execute);
  if(newleft == 0) return 0;

  FTSelection *newright = right_->optimize(ftcontext, execute);
  if(newright == 0) return newleft;

  FTSelection *result = new (mm) FTMildnot(newleft, newright, mm);
  result->setLocationInfo(this);
  return result;
}

AllMatches::Ptr FTMildnot::execute(FTContext *ftcontext) const
{
  AllMatches::Ptr leftMatches = left_->execute(ftcontext);
  AllMatches::Ptr rightMatches = right_->execute(ftcontext);
  return new FTMildnotMatches(this, leftMatches, rightMatches);
}

Match::Ptr FTMildnotMatches::next(DynamicContext *context)
{
  if(left_.isNull()) return 0;

  if(right_.notNull()) {
    Match::Ptr match(0);
    while((match = right_->next(context)).notNull()) {
      if(!match->getStringExcludes().empty())
        XQThrow(XPath2ErrorException, X("FTMildnotMatches::next"),
                X("Invalid expression on the right-hand side of a not-in"));

      StringMatches::const_iterator end = match->getStringIncludes().end();
      for(StringMatches::const_iterator i = match->getStringIncludes().begin();
          i != end; ++i) {
        badTokens_.insert(i->tokenInfo->getPosition());
      }
    }
    right_ = 0;
  }

  Match::Ptr match(0);
  while(match.isNull()) {
    match = left_->next(context);
    if(match.isNull()) {
      left_ = 0;
      return 0;
    }

    StringMatches::const_iterator end = match->getStringIncludes().end();
    for(StringMatches::const_iterator i = match->getStringIncludes().begin();
        i != end; ++i) {
      if(badTokens_.find(i->tokenInfo->getPosition()) != badTokens_.end()) {
        match = 0;
        break;
      }
    }
  }

  return match;
}
