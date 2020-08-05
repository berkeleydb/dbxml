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
 * $Id: FTWindow.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTWindow.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/utils/UTF8Str.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQTreatAs.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#include <stdlib.h>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

FTSelection *FTWindow::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  SequenceType *seqType = new (mm) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                SchemaSymbols::fgDT_INTEGER,
                                                SequenceType::EXACTLY_ONE, mm);
  seqType->setLocationInfo(this);

  arg_ = arg_->staticResolution(context);

  expr_ = new (mm) XQAtomize(expr_, mm);
  expr_->setLocationInfo(this);
  expr_ = new (mm) XQTreatAs(expr_, seqType, mm);
  expr_->setLocationInfo(this);
  expr_ = expr_->staticResolution(context);

  return this;
}

FTSelection *FTWindow::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  arg_ = arg_->staticTyping(context, styper);
  src_.add(arg_->getStaticAnalysis());

  expr_ = expr_->staticTyping(context, styper);
  src_.add(expr_->getStaticAnalysis());

  return this;
}

FTSelection *FTWindow::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  if(execute || expr_->isConstant()) {
    Result rangeResult = expr_->createResult(ftcontext->context);
    Numeric::Ptr num = (Numeric::Ptr)rangeResult->next(ftcontext->context);

    long distance = ::atol(UTF8(num->asString(ftcontext->context)));

    FTSelection *result = new (mm) FTWindowLiteral(arg_, (unsigned int)distance, unit_, mm);
    result->setLocationInfo(this);
    return result->optimize(ftcontext, execute);
  }

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  if(newarg->getType() == WORD) {
    return newarg;
  }

  newarg = new (mm) FTWindow(newarg, expr_, unit_, mm);
  newarg->setLocationInfo(this);
  return newarg;
}

AllMatches::Ptr FTWindow::execute(FTContext *ftcontext) const
{
  assert(0);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTSelection *FTWindowLiteral::staticResolution(StaticContext *context)
{
  arg_ = arg_->staticResolution(context);
  return this;
}

FTSelection *FTWindowLiteral::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  arg_ = arg_->staticTyping(context, styper);
  src_.add(arg_->getStaticAnalysis());

  return this;
}

FTSelection *FTWindowLiteral::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  if(newarg->getType() == WORD) {
    return newarg;
  }

  newarg = new (mm) FTWindowLiteral(newarg, distance_, unit_, mm);
  newarg->setLocationInfo(this);
  return newarg;
}

AllMatches::Ptr FTWindowLiteral::execute(FTContext *ftcontext) const
{
  return new FTWindowMatches(this, distance_, unit_, arg_->execute(ftcontext));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Match::Ptr FTWindowMatches::next(DynamicContext *context)
{
  if(arg_.isNull()) return 0;

  unsigned int unitVal;
  while(match_.isNull()) {
    match_ = arg_->next(context);
    if(match_.isNull()) {
      arg_ = 0;
      return 0;
    }

    StringMatches::const_iterator end = match_->getStringIncludes().end();
    StringMatches::const_iterator i = match_->getStringIncludes().begin();
    if(i != end) {
      unsigned int includeMin = FTOption::tokenUnit(i->tokenInfo, unit_);
      unsigned int includeMax = includeMin;
      for(++i; i != end; ++i) {
        unitVal = FTOption::tokenUnit(i->tokenInfo, unit_);
        if(unitVal < includeMin) {
          includeMin = unitVal;
        }
        else if(unitVal > includeMax) {
          includeMax = unitVal;
        }
      }

      if((includeMax - includeMin + 1) <= distance_) {
        unsigned int tokenStart = 0;
        if(includeMax > distance_) {
          tokenStart = includeMax - distance_ + 1;
        }
        unsigned int tokenEnd = includeMin + distance_ - 1;

        excludeValues_.clear();
        end = match_->getStringIncludes().end();
        i = match_->getStringIncludes().begin();
        for(; i != end; ++i) {
          unitVal = FTOption::tokenUnit(i->tokenInfo, unit_);
          if(tokenStart <= unitVal && unitVal <= tokenEnd) {
            excludeValues_.insert(unitVal);
          }
        }
        if(excludeValues_.empty()) {
          // Add a dummy value, so we return a match
          excludeValues_.insert(includeMin);
        }
        excludeIt_ = excludeValues_.begin();
        break;
      }
    }

    match_ = 0;
  }

  Match::Ptr result = new Match();
  result->addStringIncludes(match_->getStringIncludes());

  for(StringMatches::const_iterator i = match_->getStringExcludes().begin();
      i != match_->getStringExcludes().end(); ++i) {
    unitVal = FTOption::tokenUnit(i->tokenInfo, unit_);
    if(*excludeIt_ <= unitVal && unitVal <= (*excludeIt_ + distance_ - 1)) {
      result->addStringExclude(*i);
    }
  }

  ++excludeIt_;
  if(excludeIt_ == excludeValues_.end()) {
    match_ = 0;
  }

  return result;
}
