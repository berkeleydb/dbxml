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
 * $Id: FTMildnot.hpp 660 2008-10-07 14:29:16Z jpcs $
 */

#ifndef _FTMILDNOT_HPP
#define _FTMILDNOT_HPP

#include <xqilla/fulltext/FTSelection.hpp>

#include <set>

class XQILLA_API FTMildnot : public FTSelection
{
public:
  FTMildnot(FTSelection *left, FTSelection *right, XPath2MemoryManager *memMgr);

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context, StaticTyper *styper);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  const FTSelection *getLeft() const { return left_; }
  const FTSelection *getRight() const { return right_; }

private:
  FTSelection *left_, *right_;
};

class FTMildnotMatches : public AllMatches
{
public:
  FTMildnotMatches(const LocationInfo *info, const AllMatches::Ptr &left, const AllMatches::Ptr &right)
    : AllMatches(info), left_(left), right_(right) {}
  Match::Ptr next(DynamicContext *context);

private:
  AllMatches::Ptr left_;
  AllMatches::Ptr right_;
  std::set<unsigned int> badTokens_;
};

#endif
