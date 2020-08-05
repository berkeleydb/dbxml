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
 * $Id: ASTVisitor.cpp 531 2008-04-10 23:23:07Z jpcs $
 */

#include <xqilla/optimizer/ASTReleaser.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>

ASTReleaser::ASTReleaser()
{
}

void ASTReleaser::release(ASTNode *item)
{
  optimize(item);
}

void ASTReleaser::release(TupleNode *item)
{
  optimizeTupleNode(item);
}

void ASTReleaser::release(XQUserFunction *item)
{
  optimizeFunctionDef(item);
}

XQGlobalVariable *ASTReleaser::optimizeGlobalVar(XQGlobalVariable *item)
{
  return ASTVisitor::optimizeGlobalVar(item);
}

XQUserFunction *ASTReleaser::optimizeFunctionDef(XQUserFunction *item)
{
  if(item) {
    ASTVisitor::optimizeFunctionDef(item);
    item->releaseImpl();
  }
  return 0;
}

ASTNode *ASTReleaser::optimizeUnknown(ASTNode *item)
{
  return ASTVisitor::optimizeUnknown(item);
}

TupleNode *ASTReleaser::optimizeUnknownTupleNode(TupleNode *item)
{
  return ASTVisitor::optimizeUnknownTupleNode(item);
}

ASTNode *ASTReleaser::optimize(ASTNode *item)
{
  if(item) {
    return ASTVisitor::optimize(item);
  }
  return 0;
}

TupleNode *ASTReleaser::optimizeTupleNode(TupleNode *item)
{
  if(item) {
    return ASTVisitor::optimizeTupleNode(item);
  }
  return 0;
}

#define RELEASE_IMPL() { \
  const_cast<StaticAnalysis&>(item->getStaticAnalysis()).clear(); \
  item->getMemoryManager()->deallocate(item); \
  return 0; \
}

#define RELEASE_FULL(methodname, classname) \
ASTNode *ASTReleaser::optimize ## methodname (classname *item) \
{ \
  ASTVisitor::optimize ## methodname (item); \
  RELEASE_IMPL(); \
}

#define RELEASE_XQ(name) RELEASE_FULL(name, XQ ## name)
#define RELEASE(name) RELEASE_FULL(name, name)

// TBD release ItemConstructor objects - jpcs
// TBD release SequenceType objects - jpcs
// TBD release FTSelection objects - jpcs

RELEASE_XQ(Literal)
RELEASE_XQ(QNameLiteral)
RELEASE_XQ(NumericLiteral)
RELEASE_XQ(Step)
RELEASE_XQ(Variable)
RELEASE_XQ(If)
RELEASE_XQ(InstanceOf)
RELEASE_XQ(CastableAs)
RELEASE_XQ(CastAs)
RELEASE_XQ(TreatAs)
RELEASE_XQ(ContextItem)
RELEASE_XQ(Return)
RELEASE_XQ(Quantified)
RELEASE_XQ(Validate)
RELEASE_XQ(OrderingChange)
RELEASE_XQ(Atomize)
RELEASE_XQ(EffectiveBooleanValue)
RELEASE(XPath1CompatConvertFunctionArg)
RELEASE_XQ(PromoteUntyped)
RELEASE_XQ(PromoteNumeric)
RELEASE_XQ(PromoteAnyURI)
RELEASE_XQ(DocumentOrder)
RELEASE_XQ(Predicate)
RELEASE_XQ(Map)
RELEASE_XQ(NameExpression)
RELEASE_XQ(ContentSequence)
RELEASE_XQ(DirectName)
RELEASE_XQ(NamespaceBinding)
RELEASE_XQ(FunctionConversion)
RELEASE_XQ(AnalyzeString)
RELEASE_XQ(CopyOf)
RELEASE(ASTDebugHook)
RELEASE_XQ(FunctionRef)
RELEASE_XQ(InlineFunction)
RELEASE(UDelete)
RELEASE(URename)
RELEASE(UReplace)
RELEASE(UReplaceValueOf)
RELEASE(UInsertAsFirst)
RELEASE(UInsertAsLast)
RELEASE(UInsertInto)
RELEASE(UInsertAfter)
RELEASE(UInsertBefore)
RELEASE(UApplyUpdates)
RELEASE(FTContains)

ASTNode *ASTReleaser::optimizeFunction(XQFunction *item)
{
  ASTVisitor::optimizeFunction(item);
  // Release the argument vector
  const_cast<VectorOfASTNodes&>(item->getArguments()).~VectorOfASTNodes();
  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeNav(XQNav *item)
{
  ASTVisitor::optimizeNav(item);
  // Release the step vector
#if defined(_MSC_VER) || defined(__xlC__) 
  typedef XQNav::Steps StepVector;
  const_cast<StepVector&>(item->getSteps()).~StepVector();
#else
  const_cast<std::vector<XQNav::StepInfo,XQillaAllocator<XQNav::StepInfo> >&>(item->getSteps()).~vector<XQNav::StepInfo,XQillaAllocator<XQNav::StepInfo> >();
#endif
  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeSequence(XQSequence *item)
{
  ASTVisitor::optimizeSequence(item);
  // Release the argument vector
  const_cast<VectorOfASTNodes&>(item->getChildren()).~VectorOfASTNodes();
  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeOperator(XQOperator *item)
{
  ASTVisitor::optimizeOperator(item);
  // Release the argument vector
  const_cast<VectorOfASTNodes &>(item->getArguments()).~VectorOfASTNodes();
  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeTypeswitch(XQTypeswitch *item)
{
  ASTVisitor::optimizeTypeswitch(item);

  // Release the clauses and vector
#if defined(_MSC_VER) || defined(__xlC__)
  typedef XQTypeswitch::Cases ClauseVector;
  ClauseVector *clauses = const_cast<ClauseVector*>(item->getCases());
#else
  std::vector<XQTypeswitch::Case*,XQillaAllocator<XQTypeswitch::Case*> >* clauses =
	  const_cast<std::vector<XQTypeswitch::Case*,XQillaAllocator<XQTypeswitch::Case*> >*>(item->getCases());
#endif
  for(XQTypeswitch::Cases::iterator i = clauses->begin(); i != clauses->end(); ++i) {
    item->getMemoryManager()->deallocate(*i);
  }
#if defined(_MSC_VER) || defined(__xlC__) 
  clauses->~ClauseVector();
#else
  clauses->~vector<XQTypeswitch::Case*, XQillaAllocator<XQTypeswitch::Case*> >();
#endif
  item->getMemoryManager()->deallocate(clauses);

  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeFunctionCall(XQFunctionCall *item)
{
  ASTVisitor::optimizeFunctionCall(item);

  // Release the argument vector
  VectorOfASTNodes *args = const_cast<VectorOfASTNodes*>(item->getArguments());
  args->~VectorOfASTNodes();
  item->getMemoryManager()->deallocate(args);

  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeUserFunction(XQUserFunctionInstance *item)
{
  ASTVisitor::optimizeUserFunction(item);
  // Release the argument vector
  const_cast<VectorOfASTNodes &>(item->getArguments()).~VectorOfASTNodes();
  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeDOMConstructor(XQDOMConstructor *item)
{
  ASTVisitor::optimizeDOMConstructor(item);

  // Release the attrs vector
  VectorOfASTNodes *attrs = const_cast<VectorOfASTNodes *>(item->getAttributes());
  if(attrs) {
    attrs->~VectorOfASTNodes();
    item->getMemoryManager()->deallocate(attrs);
  }

  // Release the children vector
  VectorOfASTNodes *children = const_cast<VectorOfASTNodes *>(item->getChildren());
  if(children) {
    children->~VectorOfASTNodes();
    item->getMemoryManager()->deallocate(children);
  }

  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeSimpleContent(XQSimpleContent *item)
{
  ASTVisitor::optimizeSimpleContent(item);

  // Release the children vector
  VectorOfASTNodes *children = const_cast<VectorOfASTNodes *>(item->getChildren());
  if(children) {
    children->~VectorOfASTNodes();
    item->getMemoryManager()->deallocate(children);
  }

  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeCopy(XQCopy *item)
{
  ASTVisitor::optimizeCopy(item);
  // Release the children vector
  const_cast<VectorOfASTNodes &>(item->getChildren()).~VectorOfASTNodes();
  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeCallTemplate(XQCallTemplate *item)
{
  ASTVisitor::optimizeCallTemplate(item);

  // Release the template arguments and vector
  TemplateArguments *args = item->getArguments();
  if(args != 0) {
    for(TemplateArguments::iterator i = args->begin(); i != args->end(); ++i) {
      (*i)->varSrc.clear();
      item->getMemoryManager()->deallocate(*i);
    }
    args->~TemplateArguments();
    item->getMemoryManager()->deallocate(args);
  }

  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeApplyTemplates(XQApplyTemplates *item)
{
  ASTVisitor::optimizeApplyTemplates(item);

  // Release the template arguments and vector
  TemplateArguments *args = item->getArguments();
  if(args != 0) {
    for(TemplateArguments::iterator i = args->begin(); i != args->end(); ++i) {
      (*i)->varSrc.clear();
      item->getMemoryManager()->deallocate(*i);
    }
    args->~TemplateArguments();
    item->getMemoryManager()->deallocate(args);
  }

  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeFunctionDeref(XQFunctionDeref *item)
{
  ASTVisitor::optimizeFunctionDeref(item);

  // Release the argument vector
  VectorOfASTNodes *args = const_cast<VectorOfASTNodes*>(item->getArguments());
  args->~VectorOfASTNodes();
  item->getMemoryManager()->deallocate(args);

  RELEASE_IMPL();
}

ASTNode *ASTReleaser::optimizeUTransform(UTransform *item)
{
  ASTVisitor::optimizeUTransform(item);

  // Release the copy bindings and vector
  VectorOfCopyBinding *bindings = const_cast<VectorOfCopyBinding*>(item->getBindings());
  if(bindings != 0) {
    for(VectorOfCopyBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
      (*i)->src_.clear();
      item->getMemoryManager()->deallocate(*i);
    }
    bindings->~VectorOfCopyBinding();
    item->getMemoryManager()->deallocate(bindings);
  }

  RELEASE_IMPL();
}

#define RELEASE_TUPLE(name) \
TupleNode *ASTReleaser::optimize ## name (name *item) \
{ \
  ASTVisitor::optimize ## name (item); \
  item->getMemoryManager()->deallocate(item); \
  return 0; \
}

RELEASE_TUPLE(ContextTuple)
RELEASE_TUPLE(ForTuple)
RELEASE_TUPLE(LetTuple)
RELEASE_TUPLE(WhereTuple)
RELEASE_TUPLE(OrderByTuple)
RELEASE_TUPLE(TupleDebugHook)

