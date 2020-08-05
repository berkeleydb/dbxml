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
 * $Id: FTContains.cpp 660 2008-10-07 14:29:16Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/fulltext/FTContains.hpp>
#include <xqilla/fulltext/FTSelection.hpp>
#include <xqilla/fulltext/DefaultTokenizer.hpp>
#include <xqilla/fulltext/DefaultTokenStore.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/exceptions/XPath2TypeMatchException.hpp>

FTContains::FTContains(ASTNode *argument, FTSelection *selection, ASTNode *ignore, XPath2MemoryManager* memMgr)
  : ASTNodeImpl(FTCONTAINS, memMgr),
    argument_(argument),
    selection_(selection),
    ignore_(ignore)
{
}

FTContains::~FTContains()
{
  //no-op
}

ASTNode* FTContains::staticResolution(StaticContext *context)
{
  argument_ = argument_->staticResolution(context);
  selection_ = selection_->staticResolution(context);

  if(ignore_ != NULL) {
    ignore_ = ignore_->staticResolution(context);
  }

  return this;
}

ASTNode *FTContains::staticTypingImpl(StaticContext *context)
{
  _src.clear();

  _src.getStaticType() = StaticType::BOOLEAN_TYPE;

  _src.add(argument_->getStaticAnalysis());

  _src.add(selection_->getStaticAnalysis());

  if(ignore_ != NULL) {
    _src.add(ignore_->getStaticAnalysis());
  }

  return this;
}

Result FTContains::createResult(DynamicContext* context, int flags) const
{
  return new FTContainsResult(this);
}

FTContains::FTContainsResult::FTContainsResult(const FTContains *parent)
  : SingleResult(parent),
    parent_(parent)
{
}

Item::Ptr FTContains::FTContainsResult::getSingleResult(DynamicContext *context) const
{
  // TBD deal with ignore nodes

  DefaultTokenizer tokenizer;

  Result argNodes = parent_->getArgument()->createResult(context);
  Item::Ptr item;
  while((item = argNodes->next(context)).notNull()) {
    if(!item->isNode())
      XQThrow(XPath2TypeMatchException, X("FTContains::FTContainsResult::getSingleResult"),
              X("The argument to ftcontains contains non nodes [err:XPTY0004]"));

    FTContext ftcontext(&tokenizer, new DefaultTokenStore((Node*)item.get(), &tokenizer, context), context);

    FTSelection *optimizedSelection = parent_->getSelection()->optimize(&ftcontext, /*execute*/true);

    AllMatches::Ptr matches = optimizedSelection->execute(&ftcontext);
    if(matches.notNull()) {
      StringMatches::const_iterator i;
      StringMatches::const_iterator end;

      Match::Ptr match(0);
      while((match = matches->next(context)).notNull()) {

        bool fail = false;
        i = match->getStringIncludes().begin();
        end = match->getStringIncludes().end();
        for(; i != end; ++i) {
          if(i->startToken && i->tokenInfo->getPosition() !=
             ftcontext.tokenStore->getStartTokenPosition()) {
            fail = true;
            break;
          }
          if(i->endToken && i->tokenInfo->getPosition() !=
             ftcontext.tokenStore->getEndTokenPosition()) {
            fail = true;
            break;
          }
        }

        if(fail) continue;

        i = match->getStringExcludes().begin();
        end = match->getStringExcludes().end();
        for(; i != end; ++i) {
          if(i->startToken && i->tokenInfo->getPosition() !=
             ftcontext.tokenStore->getStartTokenPosition()) {
            continue;
          }
          if(i->endToken && i->tokenInfo->getPosition() !=
             ftcontext.tokenStore->getEndTokenPosition()) {
            continue;
          }
          fail = true;
          break;
        }

        if(fail) continue;

        return context->getItemFactory()->createBoolean(true, context);
      }
    }
  }

  return context->getItemFactory()->createBoolean(false, context);
}

std::string FTContains::FTContainsResult::asString(DynamicContext *context, int indent) const
{
  return "<ftcontainsresult/>";
}
