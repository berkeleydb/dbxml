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
 * $Id: FTOption.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _FTOPTION_HPP
#define _FTOPTION_HPP

#include <xqilla/fulltext/FTSelection.hpp>
#include <xqilla/fulltext/TokenInfo.hpp>

class XQILLA_API FTOption : public FTSelection
{
public:
  enum FTUnit {
    WORDS,
    SENTENCES,
    PARAGRAPHS
  };

  virtual void setArgument(FTSelection *arg) = 0;

  static unsigned int tokenUnit(const TokenInfo::Ptr t, FTUnit unit)
  {
    switch(unit) {
    case WORDS:
      return t->getPosition();
    case SENTENCES:
      return t->getSentence();
    case PARAGRAPHS:
      return t->getParagraph();
    }
    return 0;
  }

  static unsigned int tokenDistance(const TokenInfo::Ptr a, const TokenInfo::Ptr b, FTUnit unit)
  {
    switch(unit) {
    case WORDS:
      return (unsigned int)labs((long)a->getPosition() - (long)b->getPosition()) - 1;
    case SENTENCES:
      return (unsigned int)labs((long)a->getSentence() - (long)b->getSentence());
    case PARAGRAPHS:
      return (unsigned int)labs((long)a->getParagraph() - (long)b->getParagraph());
    }
    return 0;
  }

protected:
  FTOption(Type type, XPath2MemoryManager *memMgr)
    : FTSelection(type, memMgr) {}
};

typedef std::vector<FTOption*,XQillaAllocator<FTOption*> > VectorOfFTOptions;

#endif
