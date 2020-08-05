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
 * $Id: FTScope.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTScope.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <set>

using namespace std;

FTSelection *FTScope::staticResolution(StaticContext *context)
{
  arg_ = arg_->staticResolution(context);
  return this;
}

FTSelection *FTScope::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  arg_ = arg_->staticTyping(context, styper);
  src_.add(arg_->getStaticAnalysis());

  return this;
}

FTSelection *FTScope::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  if(newarg->getType() == WORD) {
    return newarg;
  }

  newarg = new (mm) FTScope(newarg, type_, unit_, mm);
  newarg->setLocationInfo(this);
  return newarg;
}

AllMatches::Ptr FTScope::execute(FTContext *ftcontext) const
{
  switch(type_) {
  case SAME:
    return new FTScopeSameMatches(this, unit_, arg_->execute(ftcontext));
  case DIFFERENT:
    return new FTScopeDifferentMatches(this, unit_, arg_->execute(ftcontext));
  default:
    assert(0);
    break;
  }
  return 0;
}

Match::Ptr FTScopeSameMatches::next(DynamicContext *context)
{
  if(arg_.isNull()) return 0;

  unsigned int unitValue = 0;
  Match::Ptr match(0);
  while(match.isNull()) {
    match = arg_->next(context);
    if(match.isNull()) {
      arg_ = 0;
      return 0;
    }

    StringMatches::const_iterator i = match->getStringIncludes().begin();
    StringMatches::const_iterator end = match->getStringIncludes().end();
    if(i != end) {
      unitValue = FTOption::tokenUnit(i->tokenInfo, unit_);
      for(++i; i != end; ++i) {
        if(FTOption::tokenUnit(i->tokenInfo, unit_) != unitValue) {
          match = 0;
          break;
        }
      }
    }
    else {
      // If there are no StringIncludes, the match gets returned as-is.
      return match;
    }
  }

  Match::Ptr result = new Match();
  result->addStringIncludes(match->getStringIncludes());

  StringMatches::const_iterator end = match->getStringExcludes().end();
  for(StringMatches::const_iterator i = match->getStringExcludes().begin();
      i != end; ++i) {
    if(FTOption::tokenUnit(i->tokenInfo, unit_) == unitValue) {
      result->addStringExclude(*i);
    }
  }

  return result;
}

Match::Ptr FTScopeDifferentMatches::next(DynamicContext *context)
{
  if(arg_.isNull()) return 0;

  set<unsigned int> unitValuesSeen;
  Match::Ptr match(0);
  while(match.isNull()) {
    match = arg_->next(context);
    if(match.isNull()) {
      arg_ = 0;
      return 0;
    }

    unitValuesSeen.clear();
    StringMatches::const_iterator end = match->getStringIncludes().end();
    StringMatches::const_iterator i = match->getStringIncludes().begin();
    for(; i != end; ++i) {
      if(!unitValuesSeen.insert(FTOption::tokenUnit(i->tokenInfo, unit_)).second) {
        match = 0;
        break;
      }
    }
  }

  Match::Ptr result = new Match();
  result->addStringIncludes(match->getStringIncludes());

  StringMatches::const_iterator end = match->getStringExcludes().end();
  StringMatches::const_iterator i = match->getStringExcludes().begin();
  for(; i != end; ++i) {
    if(unitValuesSeen.find(FTOption::tokenUnit(i->tokenInfo, unit_)) != unitValuesSeen.end()) {
      result->addStringExclude(*i);
    }
  }

  return result;
}
