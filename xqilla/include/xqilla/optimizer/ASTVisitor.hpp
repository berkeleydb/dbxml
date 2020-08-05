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
 * $Id: ASTVisitor.hpp 652 2008-10-01 00:08:53Z jpcs $
 */

#ifndef _ASTVISITOR_HPP
#define _ASTVISITOR_HPP

#include <xqilla/optimizer/Optimizer.hpp>

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/XQStep.hpp>
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/ast/XQIf.hpp>
#include <xqilla/ast/XQInstanceOf.hpp>
#include <xqilla/ast/XQCastableAs.hpp>
#include <xqilla/ast/XQCastAs.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQOperator.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/ast/XQDOMConstructor.hpp>
#include <xqilla/ast/XQReturn.hpp>
#include <xqilla/ast/XQQuantified.hpp>
#include <xqilla/ast/XQFunctionCall.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQTypeswitch.hpp>
#include <xqilla/ast/XQValidate.hpp>
#include <xqilla/ast/XQOrderingChange.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XQEffectiveBooleanValue.hpp>
#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/ast/XQNamespaceBinding.hpp>
#include <xqilla/ast/XQFunctionConversion.hpp>
#include <xqilla/ast/XQAnalyzeString.hpp>
#include <xqilla/ast/XQCopyOf.hpp>
#include <xqilla/ast/XQCopy.hpp>
#include <xqilla/debug/ASTDebugHook.hpp>
#include <xqilla/ast/XQCallTemplate.hpp>
#include <xqilla/ast/XQApplyTemplates.hpp>
#include <xqilla/ast/XQInlineFunction.hpp>
#include <xqilla/ast/XQFunctionRef.hpp>
#include <xqilla/ast/XQFunctionDeref.hpp>
#include <xqilla/ast/XQMap.hpp>

#include <xqilla/ast/ContextTuple.hpp>
#include <xqilla/ast/ForTuple.hpp>
#include <xqilla/ast/LetTuple.hpp>
#include <xqilla/ast/WhereTuple.hpp>
#include <xqilla/ast/OrderByTuple.hpp>
#include <xqilla/debug/TupleDebugHook.hpp>

#include <xqilla/functions/XQUserFunction.hpp>

#include <xqilla/update/UDelete.hpp>
#include <xqilla/update/URename.hpp>
#include <xqilla/update/UReplace.hpp>
#include <xqilla/update/UReplaceValueOf.hpp>
#include <xqilla/update/UInsertAsFirst.hpp>
#include <xqilla/update/UInsertAsLast.hpp>
#include <xqilla/update/UInsertInto.hpp>
#include <xqilla/update/UInsertAfter.hpp>
#include <xqilla/update/UInsertBefore.hpp>
#include <xqilla/update/UTransform.hpp>
#include <xqilla/update/UApplyUpdates.hpp>

#include <xqilla/fulltext/FTContains.hpp>

#define ALL_ASTVISITOR_METHODS() \
  virtual XQGlobalVariable *optimizeGlobalVar(XQGlobalVariable *item); \
  virtual XQUserFunction *optimizeFunctionDef(XQUserFunction *item); \
\
  virtual ASTNode *optimizeUnknown(ASTNode *item); \
  virtual ASTNode *optimizeFunction(XQFunction *item); \
  virtual ASTNode *optimizeLiteral(XQLiteral *item); \
  virtual ASTNode *optimizeQNameLiteral(XQQNameLiteral *item); \
  virtual ASTNode *optimizeNumericLiteral(XQNumericLiteral *item); \
  virtual ASTNode *optimizeNav(XQNav *item); \
  virtual ASTNode *optimizeSequence(XQSequence *item); \
  virtual ASTNode *optimizeStep(XQStep *item); \
  virtual ASTNode *optimizeVariable(XQVariable *item); \
  virtual ASTNode *optimizeIf(XQIf *item); \
  virtual ASTNode *optimizeInstanceOf(XQInstanceOf *item); \
  virtual ASTNode *optimizeCastableAs(XQCastableAs *item); \
  virtual ASTNode *optimizeCastAs(XQCastAs *item); \
  virtual ASTNode *optimizeTreatAs(XQTreatAs *item); \
  virtual ASTNode *optimizeOperator(XQOperator *item); \
  virtual ASTNode *optimizeContextItem(XQContextItem *item); \
  virtual ASTNode *optimizeReturn(XQReturn *item); \
  virtual ASTNode *optimizeQuantified(XQQuantified *item); \
  virtual ASTNode *optimizeTypeswitch(XQTypeswitch *item); \
  virtual ASTNode *optimizeValidate(XQValidate *item); \
  virtual ASTNode *optimizeFunctionCall(XQFunctionCall *item); \
  virtual ASTNode *optimizeDOMConstructor(XQDOMConstructor *item); \
  virtual ASTNode *optimizeSimpleContent(XQSimpleContent *item); \
  virtual ASTNode *optimizeOrderingChange(XQOrderingChange *item); \
  virtual ASTNode *optimizeAtomize(XQAtomize *item); \
  virtual ASTNode *optimizeEffectiveBooleanValue(XQEffectiveBooleanValue *item); \
  virtual ASTNode *optimizeXPath1CompatConvertFunctionArg(XPath1CompatConvertFunctionArg *item); \
  virtual ASTNode *optimizePromoteUntyped(XQPromoteUntyped *item); \
  virtual ASTNode *optimizePromoteNumeric(XQPromoteNumeric *item); \
  virtual ASTNode *optimizePromoteAnyURI(XQPromoteAnyURI *item); \
  virtual ASTNode *optimizeDocumentOrder(XQDocumentOrder *item); \
  virtual ASTNode *optimizePredicate(XQPredicate *item); \
  virtual ASTNode *optimizeUserFunction(XQUserFunctionInstance *item); \
  virtual ASTNode *optimizeNameExpression(XQNameExpression *item); \
  virtual ASTNode *optimizeContentSequence(XQContentSequence *item); \
  virtual ASTNode *optimizeDirectName(XQDirectName *item); \
  virtual ASTNode *optimizeNamespaceBinding(XQNamespaceBinding *item); \
  virtual ASTNode *optimizeFunctionConversion(XQFunctionConversion *item); \
  virtual ASTNode *optimizeAnalyzeString(XQAnalyzeString *item); \
  virtual ASTNode *optimizeCopyOf(XQCopyOf *item); \
  virtual ASTNode *optimizeCopy(XQCopy *item); \
  virtual ASTNode *optimizeASTDebugHook(ASTDebugHook *item); \
  virtual ASTNode *optimizeCallTemplate(XQCallTemplate *item); \
  virtual ASTNode *optimizeApplyTemplates(XQApplyTemplates *item); \
  virtual ASTNode *optimizeInlineFunction(XQInlineFunction *item); \
  virtual ASTNode *optimizeFunctionRef(XQFunctionRef *item); \
  virtual ASTNode *optimizeFunctionDeref(XQFunctionDeref *item); \
  virtual ASTNode *optimizeMap(XQMap *item); \
\
  virtual ASTNode *optimizeUDelete(UDelete *item); \
  virtual ASTNode *optimizeURename(URename *item); \
  virtual ASTNode *optimizeUReplace(UReplace *item); \
  virtual ASTNode *optimizeUReplaceValueOf(UReplaceValueOf *item); \
  virtual ASTNode *optimizeUInsertAsFirst(UInsertAsFirst *item); \
  virtual ASTNode *optimizeUInsertAsLast(UInsertAsLast *item); \
  virtual ASTNode *optimizeUInsertInto(UInsertInto *item); \
  virtual ASTNode *optimizeUInsertAfter(UInsertAfter *item); \
  virtual ASTNode *optimizeUInsertBefore(UInsertBefore *item); \
  virtual ASTNode *optimizeUTransform(UTransform *item); \
  virtual ASTNode *optimizeUApplyUpdates(UApplyUpdates *item); \
\
  virtual ASTNode *optimizeFTContains(FTContains *item); \
\
  virtual TupleNode *optimizeUnknownTupleNode(TupleNode *item); \
  virtual TupleNode *optimizeContextTuple(ContextTuple *item); \
  virtual TupleNode *optimizeForTuple(ForTuple *item); \
  virtual TupleNode *optimizeLetTuple(LetTuple *item); \
  virtual TupleNode *optimizeWhereTuple(WhereTuple *item); \
  virtual TupleNode *optimizeOrderByTuple(OrderByTuple *item); \
  virtual TupleNode *optimizeTupleDebugHook(TupleDebugHook *item);


class XQILLA_API ASTVisitor : public Optimizer
{
public:
  ASTVisitor(Optimizer *parent = 0)
    : Optimizer(parent) {}

protected:
  virtual void optimize(XQQuery *query);
  virtual ASTNode *optimize(ASTNode *item);
  virtual TupleNode *optimizeTupleNode(TupleNode *item);

  ALL_ASTVISITOR_METHODS()
};

#endif
