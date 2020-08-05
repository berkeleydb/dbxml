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
 * $Id: FTSelection.hpp 660 2008-10-07 14:29:16Z jpcs $
 */

#ifndef _FTSELECTION_HPP
#define _FTSELECTION_HPP


#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/ast/LocationInfo.hpp>
#include <xqilla/fulltext/AllMatches.hpp>
#include <xqilla/fulltext/TokenStore.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>

#include <vector>

class DynamicContext;
class StaticContext;
class StaticTyper;

class XQILLA_API FTContext
{
public:
  FTContext(const Tokenizer *tok, const TokenStore::Ptr &tStore, DynamicContext *cont)
    : queryPos(0), tokenizer(tok), tokenStore(tStore), context(cont) {}

  unsigned int queryPos;
  const Tokenizer *tokenizer;
  TokenStore::Ptr tokenStore;
  DynamicContext *context;
};

class XQILLA_API FTSelection : public LocationInfo
{
public:
  /// The type of the FTSelection node
  enum Type {
    OR,
    AND,
    MILD_NOT,
    UNARY_NOT,
    WORDS,
    WORD,
    ORDER,
    DISTANCE,
    DISTANCE_LITERAL,
    SCOPE,
    CONTENT,
    WINDOW,
    WINDOW_LITERAL
  };

  virtual ~FTSelection() {}

  Type getType() const { return type_; }

  virtual const StaticAnalysis &getStaticAnalysis() const { return src_; }

  virtual FTSelection *staticResolution(StaticContext *context) = 0;
  virtual FTSelection *staticTyping(StaticContext *context, StaticTyper *styper) = 0;
  virtual FTSelection *optimize(FTContext *context, bool execute) const = 0;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const = 0;

protected:
  FTSelection(Type type, XPath2MemoryManager *memMgr)
    : src_(memMgr), type_(type), memMgr_(memMgr) {}

  XPath2MemoryManager *getMemoryManager() const { return memMgr_; }

  StaticAnalysis src_;

private:
  Type type_;
  XPath2MemoryManager *memMgr_;
};

typedef std::vector<FTSelection*,XQillaAllocator<FTSelection*> > VectorOfFTSelections;

#endif
