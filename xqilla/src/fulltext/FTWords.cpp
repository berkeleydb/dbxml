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
 * $Id: FTWords.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTWords.hpp>
#include <xqilla/fulltext/FTOr.hpp>
#include <xqilla/fulltext/FTAnd.hpp>
#include <xqilla/fulltext/FTOrder.hpp>
#include <xqilla/fulltext/Tokenizer.hpp>
#include <xqilla/fulltext/FTRange.hpp>
#include <xqilla/fulltext/FTDistance.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQTreatAs.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

FTWords::FTWords(ASTNode *expr, FTAnyallOption option, XPath2MemoryManager *memMgr)
    : FTSelection(FTSelection::WORDS, memMgr),
      expr_(expr),
      option_(option)
{
}

FTSelection *FTWords::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  SequenceType *seqType = new (mm) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                SchemaSymbols::fgDT_STRING,
                                                SequenceType::STAR, mm);
  seqType->setLocationInfo(this);

  expr_ = new (mm) XQAtomize(expr_, mm);
  expr_->setLocationInfo(this);
  expr_ = new (mm) XQTreatAs(expr_, seqType, mm);
  expr_->setLocationInfo(this);
  expr_ = expr_->staticResolution(context);

  return this;
}

FTSelection *FTWords::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  expr_ = expr_->staticTyping(context, styper);
  src_.add(expr_->getStaticAnalysis());

  return this;
}

FTSelection *FTWords::optimize(FTContext *ftcontext, bool execute) const
{
  if(execute || expr_->isConstant()) {
    Result strings = expr_->createResult(ftcontext->context);

    switch(option_) {
    case ANY_WORD: {
      return optimizeAnyWord(strings, ftcontext)->
        optimize(ftcontext, execute);
    }
    case ALL_WORDS: {
      return optimizeAllWords(strings, ftcontext)->
        optimize(ftcontext, execute);
    }
    case PHRASE: {
      return optimizePhrase(strings, ftcontext)->
        optimize(ftcontext, execute);
    }
    case ANY: {
      return optimizeAny(strings, ftcontext)->
        optimize(ftcontext, execute);
    }
    case ALL: {
      return optimizeAll(strings, ftcontext)->
        optimize(ftcontext, execute);
    }
    default:
      assert(0);
      break;
    }
  }

  return const_cast<FTWords*>(this);
}

FTSelection *FTWords::optimizeAnyWord(Result strings, FTContext *ftcontext) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTOr *ftor = new (mm) FTOr(mm);
  ftor->setLocationInfo(this);

  Item::Ptr item;
  while((item = strings->next(ftcontext->context)).notNull()) {
    TokenStream::Ptr stream = ftcontext->tokenizer->
      tokenize(item->asString(ftcontext->context), ftcontext->context->getMemoryManager());
    TokenInfo::Ptr token;
    while((token = stream->next()).notNull()) {
      FTSelection *word = new (mm) FTWord(token->getWord(), mm);
      word->setLocationInfo(this);
      ftor->addArg(word);
    }
  }

  return ftor;
}

FTSelection *FTWords::optimizeAllWords(Result strings, FTContext *ftcontext) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTAnd *ftand = new (mm) FTAnd(mm);
  ftand->setLocationInfo(this);

  Item::Ptr item;
  while((item = strings->next(ftcontext->context)).notNull()) {
    TokenStream::Ptr stream = ftcontext->tokenizer->
      tokenize(item->asString(ftcontext->context), ftcontext->context->getMemoryManager());
    TokenInfo::Ptr token;
    while((token = stream->next()).notNull()) {
      FTSelection *word = new (mm) FTWord(token->getWord(), mm);
      word->setLocationInfo(this);
      ftand->addArg(word);
    }
  }

  return ftand;
}

FTSelection *FTWords::optimizePhrase(Result strings, FTContext *ftcontext) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *result = new (mm) FTOrder(optimizeAllWords(strings, ftcontext), mm);
  result->setLocationInfo(this);

  result = new (mm) FTDistanceLiteral(result, FTRange::EXACTLY, 0, 0, FTOption::WORDS, mm);
  result->setLocationInfo(this);

  return result;
}

FTSelection *FTWords::optimizeAny(Result strings, FTContext *ftcontext) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTOr *ftor = new (mm) FTOr(mm);
  ftor->setLocationInfo(this);

  Item::Ptr item;
  while((item = strings->next(ftcontext->context)).notNull()) {
    ftor->addArg(optimizePhrase(Sequence(item), ftcontext));
  }

  return ftor;
}

FTSelection *FTWords::optimizeAll(Result strings, FTContext *ftcontext) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTAnd *ftand = new (mm) FTAnd(mm);
  ftand->setLocationInfo(this);

  Item::Ptr item;
  while((item = strings->next(ftcontext->context)).notNull()) {
    ftand->addArg(optimizePhrase(Sequence(item), ftcontext));
  }

  return ftand;
}

AllMatches::Ptr FTWords::execute(FTContext *ftcontext) const
{
  assert(0);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTWord::FTWord(const XMLCh *queryString, XPath2MemoryManager *memMgr)
  : FTSelection(WORD, memMgr),
    queryString_(queryString)
{
}

FTSelection *FTWord::staticResolution(StaticContext *context)
{
  return this;
}

FTSelection *FTWord::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();
  return this;
}

FTSelection *FTWord::optimize(FTContext *context, bool execute) const
{
  return const_cast<FTWord*>(this);
}

AllMatches::Ptr FTWord::execute(FTContext *ftcontext) const
{
  return new FTStringSearchMatches(this, queryString_, ftcontext);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTStringSearchMatches::FTStringSearchMatches(const LocationInfo *info, const XMLCh *queryString, FTContext *ftcontext)
  : AllMatches(info),
    queryString_(queryString),
    queryPos_(ftcontext->queryPos++),
    tokenStream_(ftcontext->tokenStore->findTokens(queryString))
{
}

Match::Ptr FTStringSearchMatches::next(DynamicContext *context)
{
  if(tokenStream_.isNull()) return 0;

  TokenInfo::Ptr token = tokenStream_->next();
  if(token.isNull()) {
    tokenStream_ = 0;
    return 0;
  }

  // TBD query position
  Match::Ptr match = new Match();
  match->addStringInclude(queryString_, queryPos_, token);
  return match;
}

AllMatches::Ptr FTStringSearchMatches::optimize()
{
  return this;
}
