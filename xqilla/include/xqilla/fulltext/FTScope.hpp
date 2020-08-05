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
 * $Id: FTScope.hpp 660 2008-10-07 14:29:16Z jpcs $
 */

#ifndef _FTSCOPE_HPP
#define _FTSCOPE_HPP

#include <xqilla/fulltext/FTOption.hpp>

class XQILLA_API FTScope : public FTOption
{
public:
  enum Type {
    SAME,
    DIFFERENT
  };

  FTScope(Type type, FTUnit unit, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::SCOPE, memMgr), type_(type), unit_(unit), arg_(0) {}
  FTScope(FTSelection *arg, Type type, FTUnit unit, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::SCOPE, memMgr), type_(type), unit_(unit), arg_(arg) {}

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context, StaticTyper *styper);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  virtual void setArgument(FTSelection *arg) { arg_ = arg; }
  const FTSelection *getArgument() const { return arg_; }
  Type getType() const { return type_; }
  FTUnit getUnit() const { return unit_; }

private:
  Type type_;
  FTUnit unit_;
  FTSelection *arg_;
};

class FTScopeSameMatches : public AllMatches
{
public:
  FTScopeSameMatches(const LocationInfo *info, FTOption::FTUnit unit, const AllMatches::Ptr &arg)
    : AllMatches(info), unit_(unit), arg_(arg) {}
  Match::Ptr next(DynamicContext *context);

private:
  FTOption::FTUnit unit_;
  AllMatches::Ptr arg_;
};

class FTScopeDifferentMatches : public AllMatches
{
public:
  FTScopeDifferentMatches(const LocationInfo *info, FTOption::FTUnit unit, const AllMatches::Ptr &arg)
    : AllMatches(info), unit_(unit), arg_(arg) {}
  Match::Ptr next(DynamicContext *context);

private:
  FTOption::FTUnit unit_;
  AllMatches::Ptr arg_;
};

#endif
