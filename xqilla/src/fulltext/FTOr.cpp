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
 * $Id: FTOr.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTOr.hpp>
#include <xqilla/context/DynamicContext.hpp>

using namespace std;

FTOr::FTOr(XPath2MemoryManager *memMgr)
  : FTSelection(FTSelection::OR, memMgr),
    args_(memMgr)
{
}

FTOr::FTOr(FTSelection *left, FTSelection *right, XPath2MemoryManager *memMgr)
    : FTSelection(FTSelection::OR, memMgr),
      args_(memMgr)
{
  args_.push_back(left);
  args_.push_back(right);
}

FTSelection *FTOr::staticResolution(StaticContext *context)
{
  for(VectorOfFTSelections::iterator i = args_.begin();
      i != args_.end(); ++i) {
    *i = (*i)->staticResolution(context);
  }

  return this;
}

FTSelection *FTOr::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  for(VectorOfFTSelections::iterator i = args_.begin();
      i != args_.end(); ++i) {
    *i = (*i)->staticTyping(context, styper);
    src_.add((*i)->getStaticAnalysis());
  }

  return this;
}

FTSelection *FTOr::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTOr *ftor = new (mm) FTOr(mm);
  ftor->setLocationInfo(this);

  for(VectorOfFTSelections::const_iterator i = args_.begin();
      i != args_.end(); ++i) {
    FTSelection *arg = (*i)->optimize(ftcontext, execute);
    if(arg != 0)
      ftor->addArg(arg);
  }

  if(ftor->args_.empty()) {
    return 0;
  }
  if(ftor->args_.size() == 1) {
    return ftor->args_.back();
  }

  return ftor;
}

AllMatches::Ptr FTOr::execute(FTContext *ftcontext) const
{
  FTDisjunctionMatches *disjunction = new FTDisjunctionMatches(this);
  AllMatches::Ptr result(disjunction);

  for(VectorOfFTSelections::const_iterator i = args_.begin();
      i != args_.end(); ++i) {
    disjunction->addMatches((*i)->execute(ftcontext));
  }

  return result;
}

FTDisjunctionMatches::FTDisjunctionMatches(const LocationInfo *info)
  : AllMatches(info),
    toDo_(true)
{
}

Match::Ptr FTDisjunctionMatches::next(DynamicContext *context)
{
  // TBD AllMatches normalization

  if(toDo_) {
    toDo_ = false;
    it_ = args_.begin();
  }

  Match::Ptr result(0);
  while(it_ != args_.end()) {
    result = (*it_)->next(context);
    if(result.isNull()) {
      *it_ = 0;
      ++it_;
    }
    else {
      break;
    }
  }

  return result;
}
