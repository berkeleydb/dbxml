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
 * $Id: Optimizer.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _STATICTYPER_HPP
#define _STATICTYPER_HPP

#include <xqilla/optimizer/ASTVisitor.hpp>

class XQILLA_API StaticTyper : public ASTVisitor
{
public:
  StaticTyper()
    : ASTVisitor(), context_(0), tupleSetup_(false) {}
  StaticTyper(StaticContext *context, Optimizer *parent = 0)
    : ASTVisitor(parent), context_(context), tupleSetup_(false) {}

  ASTNode *run(ASTNode *item, StaticContext *context);

protected:
  virtual void optimize(XQQuery *query);
  virtual ASTNode *optimize(ASTNode *item);
  virtual ASTNode *optimizePredicate(XQPredicate *item);
  virtual ASTNode *optimizeAnalyzeString(XQAnalyzeString *item);
  virtual ASTNode *optimizeNav(XQNav *item);
  virtual ASTNode *optimizeQuantified(XQQuantified *item);
  virtual ASTNode *optimizeInstanceOf(XQInstanceOf *item);
  virtual ASTNode *optimizeMap(XQMap *item);
  virtual ASTNode *optimizeReturn(XQReturn *item);
  virtual ASTNode *optimizeTypeswitch(XQTypeswitch *item);
  virtual ASTNode *optimizeTreatAs(XQTreatAs *item);
  virtual ASTNode *optimizeNamespaceBinding(XQNamespaceBinding *item);
  virtual ASTNode *optimizeFunctionRef(XQFunctionRef *item);
  virtual ASTNode *optimizeInlineFunction(XQInlineFunction *item);
  virtual ASTNode *optimizeEffectiveBooleanValue(XQEffectiveBooleanValue *item);
  virtual ASTNode *optimizeUTransform(UTransform *item);
  virtual ASTNode *optimizeFTContains(FTContains *item);

  virtual TupleNode *optimizeTupleNode(TupleNode *item);
  virtual TupleNode *optimizeForTuple(ForTuple *item);
  virtual TupleNode *optimizeLetTuple(LetTuple *item);
  virtual TupleNode *optimizeWhereTuple(WhereTuple *item);
  virtual TupleNode *optimizeOrderByTuple(OrderByTuple *item);

  void optimizeCase(const StaticAnalysis &var_src, XQTypeswitch::Case *item);

  StaticContext *context_;
  bool tupleSetup_;
};

#endif
