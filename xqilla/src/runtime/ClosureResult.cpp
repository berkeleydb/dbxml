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
 * $Id: ClosureResult.cpp 626 2008-08-26 20:26:44Z jpcs $
 */

#include "../config/xqilla_config.h"
#include <xqilla/runtime/ClosureResult.hpp>
#include <xqilla/ast/ASTNode.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ContextHelpers.hpp>

using namespace std;

Result ClosureResult::create(const ASTNode *ast, DynamicContext *context, const VariableStore *sourceScope)
{
  // TBD We probably need to store the regex groups here too - jpcs

  if(!ast->getStaticAnalysis().variablesUsed() &&
     !ast->getStaticAnalysis().areContextFlagsUsed()) {
    return ast->createResult(context);
  }

  return new ClosureResult(ast, context, sourceScope);
}

ClosureResult::ClosureResult(const ASTNode *ast, DynamicContext *context, const VariableStore *sourceScope)
  : ResultImpl(ast),
    contextItem_(context->getContextItem()),
    contextPosition_(context->getContextPosition()),
    contextSize_(context->getContextSize()),
    varStore_(context->getMemoryManager()),
    docCache_(const_cast<DocumentCache*>(context->getDocumentCache())),
    result_(0)
{
  if(sourceScope == 0) sourceScope = context->getVariableStore();

  // Copy the variables we need into our local storage
  varStore_.cacheVariableStore(ast->getStaticAnalysis(), sourceScope);

  try {
    AutoVariableStoreReset vsReset(context, &varStore_);
    result_ = ast->createResult(context);
  }
  catch(...) {
    varStore_.clear();
    throw;
  }
}

Item::Ptr ClosureResult::next(DynamicContext *context)
{
  AutoDocumentCacheReset dcReset(context);
  context->setDocumentCache(docCache_);
  AutoContextInfoReset ciReset(context, contextItem_, contextPosition_, contextSize_);
  AutoVariableStoreReset vsReset(context, &varStore_);

  return result_->next(context);
}

Item::Ptr ClosureResult::nextOrTail(Result &tail, DynamicContext *context)
{
  AutoDocumentCacheReset dcReset(context);
  context->setDocumentCache(docCache_);
  AutoContextInfoReset ciReset(context, contextItem_, contextPosition_, contextSize_);
  AutoVariableStoreReset vsReset(context, &varStore_);

  return result_->nextOrTail(tail, context);
}

ClosureEventGenerator::ClosureEventGenerator(const ASTNode *ast, DynamicContext *context, bool preserveNS, bool preserveType)
  : ASTNodeEventGenerator(ast, preserveNS, preserveType),
    contextItem_(context->getContextItem()),
    contextPosition_(context->getContextPosition()),
    contextSize_(context->getContextSize()),
    varStore_(context->getMemoryManager()),
    docCache_(const_cast<DocumentCache*>(context->getDocumentCache()))
{
  // Copy the variables we need into our local storage
  varStore_.cacheVariableStore(ast->getStaticAnalysis(), context->getVariableStore());
}

EventGenerator::Ptr ClosureEventGenerator::generateEvents(EventHandler *events, DynamicContext *context)
{
  AutoDocumentCacheReset dcReset(context);
  context->setDocumentCache(docCache_);
  AutoContextInfoReset ciReset(context, contextItem_, contextPosition_, contextSize_);
  AutoVariableStoreReset vsReset(context, &varStore_);

  return ast_->generateEvents(events, context, preserveNS_, preserveType_);
}
