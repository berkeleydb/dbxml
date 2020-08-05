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
 * $Id: FTDistance.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/fulltext/FTDistance.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/utils/UTF8Str.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQTreatAs.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#include <algorithm>
#include <stdlib.h>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

FTSelection *FTDistance::staticResolution(StaticContext *context)
{
  XPath2MemoryManager *mm = context->getMemoryManager();

  SequenceType *seqType = new (mm) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                SchemaSymbols::fgDT_INTEGER,
                                                SequenceType::EXACTLY_ONE, mm);
  seqType->setLocationInfo(this);

  arg_ = arg_->staticResolution(context);

  range_.arg1 = new (mm) XQAtomize(range_.arg1, mm);
  range_.arg1->setLocationInfo(this);
  range_.arg1 = new (mm) XQTreatAs(range_.arg1, seqType, mm);
  range_.arg1->setLocationInfo(this);
  range_.arg1 = range_.arg1->staticResolution(context);

  if(range_.arg2 != NULL) {
    range_.arg2 = new (mm) XQAtomize(range_.arg2, mm);
    range_.arg2->setLocationInfo(this);
    range_.arg2 = new (mm) XQTreatAs(range_.arg2, seqType, mm);
    range_.arg2->setLocationInfo(this);
    range_.arg2 = range_.arg2->staticResolution(context);
  }

  return this;
}

FTSelection *FTDistance::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  arg_ = arg_->staticTyping(context, styper);
  src_.add(arg_->getStaticAnalysis());

  range_.arg1 = range_.arg1->staticTyping(context, styper);
  src_.add(range_.arg1->getStaticAnalysis());

  if(range_.arg2 != NULL) {
    range_.arg2 = range_.arg2->staticTyping(context, styper);
    src_.add(range_.arg2->getStaticAnalysis());
  }

  return this;
}

FTSelection *FTDistance::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  if(execute || range_.arg1->isConstant()) {
    Result rangeResult = range_.arg1->createResult(ftcontext->context);
    Numeric::Ptr num = (Numeric::Ptr)rangeResult->next(ftcontext->context);
    long distance = ::atol(UTF8(num->asString(ftcontext->context)));

    switch(range_.type) {
    case FTRange::EXACTLY: {
      FTSelection *result = new (mm) FTDistanceLiteral(arg_, FTRange::EXACTLY, distance, 0, unit_, mm);
      result->setLocationInfo(this);
      return result->optimize(ftcontext, execute);
    }
    case FTRange::AT_LEAST: {
      FTSelection *result = new (mm) FTDistanceLiteral(arg_, FTRange::AT_LEAST, distance, 0, unit_, mm);
      result->setLocationInfo(this);
      return result->optimize(ftcontext, execute);
    }
    case FTRange::AT_MOST: {
      FTSelection *result = new (mm) FTDistanceLiteral(arg_, FTRange::AT_MOST, distance, 0, unit_, mm);
      result->setLocationInfo(this);
      return result->optimize(ftcontext, execute);
    }
    case FTRange::FROM_TO: {
      Result rangeResult2 = range_.arg2->createResult(ftcontext->context);
      Numeric::Ptr num2 = (Numeric::Ptr)rangeResult2->next(ftcontext->context);
      long distance2 = ::atol(UTF8(num->asString(ftcontext->context)));

      FTSelection *result = new (mm) FTDistanceLiteral(arg_, FTRange::FROM_TO, distance, distance2, unit_, mm);
      result->setLocationInfo(this);
      return result->optimize(ftcontext, execute);
    }
    }
  }

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  if(newarg->getType() == WORD) {
    return newarg;
  }

  newarg = new (mm) FTDistance(range_, unit_, newarg, mm);
  newarg->setLocationInfo(this);
  return newarg;
}

AllMatches::Ptr FTDistance::execute(FTContext *ftcontext) const
{
  assert(0);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTSelection *FTDistanceLiteral::staticResolution(StaticContext *context)
{
  arg_ = arg_->staticResolution(context);
  return this;
}

FTSelection *FTDistanceLiteral::staticTyping(StaticContext *context, StaticTyper *styper)
{
  src_.clear();

  arg_ = arg_->staticTyping(context, styper);
  src_.add(arg_->getStaticAnalysis());

  return this;
}

FTSelection *FTDistanceLiteral::optimize(FTContext *ftcontext, bool execute) const
{
  XPath2MemoryManager *mm = ftcontext->context->getMemoryManager();

  FTSelection *newarg = arg_->optimize(ftcontext, execute);
  if(newarg == 0) return 0;

  if(newarg->getType() == WORD) {
    return newarg;
  }
  
  newarg = new (mm) FTDistanceLiteral(newarg, type_, distance_, distance2_, unit_, mm);
  newarg->setLocationInfo(this);
  return newarg;
}

AllMatches::Ptr FTDistanceLiteral::execute(FTContext *ftcontext) const
{
    switch(type_) {
    case FTRange::EXACTLY: {
      return new FTDistanceExactlyMatches(this, distance_, unit_, arg_->execute(ftcontext));
    }
    case FTRange::AT_LEAST: {
      return new FTDistanceAtLeastMatches(this, distance_, unit_, arg_->execute(ftcontext));
    }
    case FTRange::AT_MOST: {
      return new FTDistanceAtMostMatches(this, distance_, unit_, arg_->execute(ftcontext));
    }
    case FTRange::FROM_TO: {
      return new FTDistanceFromToMatches(this, distance_, distance2_, unit_, arg_->execute(ftcontext));
    }
    default:
      assert(0);
      break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static bool lessThanCompareFn(const StringMatch &first, const StringMatch &second)
{
	return first.tokenInfo->getPosition() < second.tokenInfo->getPosition();
}

Match::Ptr FTDistanceMatches::next(DynamicContext *context)
{
  if(arg_.isNull()) return 0;

  Match::Ptr match(0);
  while(match.isNull()) {
    match = arg_->next(context);
    if(match.isNull()) {
      arg_ = 0;
      return 0;
    }

    if(match->getStringIncludes().size() > 1) {
      StringMatches sMatches = match->getStringIncludes();
      std::sort(sMatches.begin(), sMatches.end(), lessThanCompareFn);

      StringMatches::iterator end = sMatches.end();
      StringMatches::iterator a = sMatches.begin();
      StringMatches::iterator b = a; ++b;
      for(; b != end; ++a, ++b) {
        unsigned int actual = FTOption::tokenDistance(a->tokenInfo, b->tokenInfo, unit_);
        if(!distanceMatches(actual)) {
          match = 0;
          break;
        }
      }
    }
  }

  Match::Ptr result = new Match();
  result->addStringIncludes(match->getStringIncludes());

  for(StringMatches::const_iterator i = match->getStringExcludes().begin();
      i != match->getStringExcludes().end(); ++i) {
    for(StringMatches::const_iterator j = match->getStringIncludes().begin();
        j != match->getStringIncludes().end(); ++j) {
      unsigned int actual = FTOption::tokenDistance(i->tokenInfo, j->tokenInfo, unit_);
      if(distanceMatches(actual)) {
        result->addStringExclude(*i);
        break;
      }
    }
  }

  return result;
}

bool FTDistanceExactlyMatches::distanceMatches(unsigned int actual) const
{
  return actual == distance_;
}

bool FTDistanceAtLeastMatches::distanceMatches(unsigned int actual) const
{
  return actual >= distance_;
}

bool FTDistanceAtMostMatches::distanceMatches(unsigned int actual) const
{
  return actual <= distance_;
}

bool FTDistanceFromToMatches::distanceMatches(unsigned int actual) const
{
  return distance_ <= actual && actual <= distance2_;
}
