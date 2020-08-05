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
 * $Id: FTAnd.hpp 660 2008-10-07 14:29:16Z jpcs $
 */

#ifndef _FTAND_HPP
#define _FTAND_HPP

#include <xqilla/fulltext/FTSelection.hpp>

#include <list>

class XQILLA_API FTAnd : public FTSelection
{
public:
  FTAnd(XPath2MemoryManager *memMgr);
  FTAnd(FTSelection *left, FTSelection *right, XPath2MemoryManager *memMgr);

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context, StaticTyper *styper);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  const VectorOfFTSelections &getArguments() const { return args_; }
  void addArg(FTSelection *sel) { args_.push_back(sel); }

private:
  VectorOfFTSelections args_;
};

class BufferedMatches : public AllMatches
{
public:
  typedef RefCountPointer<BufferedMatches> Ptr;

  BufferedMatches(const LocationInfo *info, const AllMatches::Ptr matches);

  Match::Ptr current();
  Match::Ptr next(DynamicContext *context);
  void reset();

private:
  AllMatches::Ptr matches_;
  std::list<Match::Ptr> buffer_;
  std::list<Match::Ptr>::iterator it_;
};

class FTConjunctionMatches : public AllMatches
{
public:
  FTConjunctionMatches(const LocationInfo *info);

  void addMatches(const AllMatches::Ptr &m);

  Match::Ptr next(DynamicContext *context);

private:
  bool toDo_;
  std::vector<BufferedMatches::Ptr> args_;
};

#endif
