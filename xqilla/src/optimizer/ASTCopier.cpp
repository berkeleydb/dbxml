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

#include <xqilla/optimizer/ASTCopier.hpp>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <xqilla/ast/XQDocumentConstructor.hpp>
#include <xqilla/ast/XQElementConstructor.hpp>
#include <xqilla/ast/XQAttributeConstructor.hpp>
#include <xqilla/ast/XQTextConstructor.hpp>
#include <xqilla/ast/XQCommentConstructor.hpp>
#include <xqilla/ast/XQPIConstructor.hpp>
#include <xqilla/ast/XQNamespaceConstructor.hpp>

#include <xqilla/operators/And.hpp>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/Except.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/operators/IntegerDivide.hpp>
#include <xqilla/operators/Intersect.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/Minus.hpp>
#include <xqilla/operators/Mod.hpp>
#include <xqilla/operators/Multiply.hpp>
#include <xqilla/operators/NodeComparison.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/Or.hpp>
#include <xqilla/operators/OrderComparison.hpp>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/operators/Range.hpp>
#include <xqilla/operators/UnaryMinus.hpp>
#include <xqilla/operators/Union.hpp>

#include <xqilla/functions/FunctionCodepointEqual.hpp>
#include <xqilla/update/FunctionPut.hpp>

XERCES_CPP_NAMESPACE_USE;

ASTCopier::ASTCopier()
{
}

ASTNode *ASTCopier::copy(const ASTNode *item, DynamicContext *context)
{
  context_ = context;
  mm_ = context->getMemoryManager();
  return optimize(const_cast<ASTNode*>(item));
}

TupleNode *ASTCopier::copy(const TupleNode *item, DynamicContext *context)
{
  context_ = context;
  mm_ = context->getMemoryManager();
  return optimizeTupleNode(const_cast<TupleNode*>(item));
}

XQGlobalVariable *ASTCopier::optimizeGlobalVar(XQGlobalVariable *item)
{
  return ASTVisitor::optimizeGlobalVar(item);
}

class UpdateInstance : public ASTVisitor
{
public:

  void run(ASTNode *item, const XQUserFunction *oldFunc, XQUserFunction *newFunc)
  {
    oldFunc_ = oldFunc;
    newFunc_ = newFunc;
    optimize(item);
  }

protected:
  virtual XQUserFunction *optimizeFunctionDef(XQUserFunction *item)
  {
    if(item == oldFunc_) return newFunc_;
    return item;
  }

  virtual ASTNode *optimizeUserFunction(XQUserFunctionInstance *item)
  {
    if(item->getFunctionDefinition() == oldFunc_) item->setFunctionDefinition(newFunc_);
    return ASTVisitor::optimizeUserFunction(item);
  }

  const XQUserFunction *oldFunc_;
  XQUserFunction *newFunc_;
};

XQUserFunction *ASTCopier::optimizeFunctionDef(XQUserFunction *item)
{
  if(item == 0) return 0;

  XQUserFunction *result = new (mm_) XQUserFunction(item, mm_);
  ASTVisitor::optimizeFunctionDef(result);

  if(result->getTemplateInstance()) {
    // Update the pointers to the XQUserFunction in the instance
    UpdateInstance().run(result->getTemplateInstance(), item, result);
  }

  return result;
}

ASTNode *ASTCopier::optimizeUnknown(ASTNode *item)
{
  return ASTVisitor::optimizeUnknown(item);
}

TupleNode *ASTCopier::optimizeUnknownTupleNode(TupleNode *item)
{
  return ASTVisitor::optimizeUnknownTupleNode(item);
}

#define COPY_IMPL() { \
  result->setLocationInfo(item); \
  const_cast<StaticAnalysis&>(result->getStaticAnalysis()).copy(item->getStaticAnalysis()); \
  return result; \
}

#define COPY_FULL0(methodname, classname) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ0(name) COPY_FULL0(name, XQ ## name)
#define COPY0(name) COPY_FULL0(name, name)

#define COPY_FULL1(methodname, classname, arg1name) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(item->get ## arg1name (), mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ1(name, arg1name) COPY_FULL1(name, XQ ## name, arg1name)
#define COPY1(name, arg1name) COPY_FULL1(name, name, arg1name)

#define COPY_FULL2(methodname, classname, arg1name, arg2name) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(item->get ## arg1name (), item->get ## arg2name (), mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ2(name, arg1name, arg2name) COPY_FULL2(name, XQ ## name, arg1name, arg2name)
#define COPY2(name, arg1name, arg2name) COPY_FULL2(name, name, arg1name, arg2name)

#define COPY_FULL3(methodname, classname, arg1name, arg2name, arg3name) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(item->get ## arg1name (), item->get ## arg2name (), item->get ## arg3name (), mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ3(name, arg1name, arg2name, arg3name) COPY_FULL3(name, XQ ## name, arg1name, arg2name, arg3name)
#define COPY3(name, arg1name, arg2name, arg3name) COPY_FULL3(name, name, arg1name, arg2name, arg3name)

#define COPY_FULL4(methodname, classname, arg1name, arg2name, arg3name, arg4name) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(item->get ## arg1name (), item->get ## arg2name (), item->get ## arg3name (), item->get ## arg4name (), mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ4(name, arg1name, arg2name, arg3name, arg4name) COPY_FULL4(name, XQ ## name, arg1name, arg2name, arg3name, arg4name)
#define COPY4(name, arg1name, arg2name, arg3name, arg4name) COPY_FULL4(name, name, arg1name, arg2name, arg3name, arg4name)

#define COPY_FULL5(methodname, classname, arg1name, arg2name, arg3name, arg4name, arg5name) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(item->get ## arg1name (), item->get ## arg2name (), item->get ## arg3name (), item->get ## arg4name (), \
    item->get ## arg5name (), mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ5(name, arg1name, arg2name, arg3name, arg4name, arg5name) COPY_FULL5(name, XQ ## name, arg1name, arg2name, arg3name, arg4name, arg5name)
#define COPY5(name, arg1name, arg2name, arg3name, arg4name, arg5name) COPY_FULL5(name, name, arg1name, arg2name, arg3name, arg4name, arg5name)

#define COPY_FULL6(methodname, classname, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(item->get ## arg1name (), item->get ## arg2name (), item->get ## arg3name (), item->get ## arg4name (), \
    item->get ## arg5name (), item->get ## arg6name (), mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ6(name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name) COPY_FULL6(name, XQ ## name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name)
#define COPY6(name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name) COPY_FULL6(name, name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name)

#define COPY_FULL7(methodname, classname, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(item->get ## arg1name (), item->get ## arg2name (), item->get ## arg3name (), item->get ## arg4name (), \
    item->get ## arg5name (), item->get ## arg6name (), item->get ## arg7name (), mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ7(name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name) \
  COPY_FULL7(name, XQ ## name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name)
#define COPY7(name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name) \
  COPY_FULL7(name, name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name)

#define COPY_FULL8(methodname, classname, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name, arg8name) \
ASTNode *ASTCopier::optimize ## methodname (classname *item) \
{ \
  classname *result = new (mm_) classname(item->get ## arg1name (), item->get ## arg2name (), item->get ## arg3name (), item->get ## arg4name (), \
    item->get ## arg5name (), item->get ## arg6name (), item->get ## arg7name (), item->get ## arg8name (), mm_); \
  ASTVisitor::optimize ## methodname (result); \
  COPY_IMPL(); \
}

#define COPY_XQ8(name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name, arg8name) \
  COPY_FULL8(name, XQ ## name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name, arg8name)
#define COPY8(name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name, arg8name) \
  COPY_FULL8(name, name, arg1name, arg2name, arg3name, arg4name, arg5name, arg6name, arg7name, arg8name)

// TBD copy ItemConstructor objects - jpcs
// TBD copy SequenceType objects - jpcs
// TBD copy NodeTest objects - jpcs

COPY_XQ4(Literal, TypeURI, TypeName, Value, PrimitiveType)
COPY_XQ5(QNameLiteral, TypeURI, TypeName, URI, Prefix, Localname)
COPY_XQ4(NumericLiteral, TypeURI, TypeName, Value, PrimitiveType)
COPY_XQ1(Sequence, Children)
COPY_XQ2(Step, Axis, NodeTest)
COPY_XQ2(Variable, URI, Name)
COPY_XQ3(If, Test, WhenTrue, WhenFalse)
COPY_XQ2(InstanceOf, Expression, SequenceType) // SequenceType
COPY_XQ4(CastableAs, Expression, SequenceType, IsPrimitive, TypeIndex)
COPY_XQ4(CastAs, Expression, SequenceType, IsPrimitive, TypeIndex)
COPY_XQ8(TreatAs, Expression, SequenceType, ErrorCode, DoTypeCheck, DoCardinalityCheck, FuncConvert, TreatType, IsExact)
COPY_XQ0(ContextItem)
COPY_XQ2(Return, Parent, Expression)
COPY_XQ3(Quantified, QuantifierType, Parent, Expression)
COPY_XQ2(Validate, Expression, Mode)
COPY_XQ2(OrderingChange, OrderingValue, Expr)
COPY_XQ2(Atomize, Expression, DoPSVI)
COPY_XQ1(EffectiveBooleanValue, Expression)
COPY2(XPath1CompatConvertFunctionArg, Expression, SequenceType) // SequenceType
COPY_XQ5(PromoteUntyped, Expression, TypeURI, TypeName, IsPrimitive, TypeIndex)
COPY_XQ4(PromoteNumeric, Expression, TypeURI, TypeName, TypeIndex)
COPY_XQ3(PromoteAnyURI, Expression, TypeURI, TypeName)
COPY_XQ2(DocumentOrder, Expression, Unordered)
COPY_XQ3(Predicate, Expression, Predicate, Reverse)
COPY_XQ1(NameExpression, Expression)
COPY_XQ1(ContentSequence, Expression)
COPY_XQ2(DirectName, QName, IsAttr)
COPY_XQ2(NamespaceBinding, Namespaces, Expression)
COPY_XQ2(FunctionConversion, Expression, SequenceType)
COPY_XQ5(AnalyzeString, Expression, Regex, Flags, Match, NonMatch)
COPY_XQ2(CopyOf, Expression, CopyNamespaces)
COPY_XQ4(Copy, Expression, Children, CopyNamespaces, InheritNamespaces)
COPY1(ASTDebugHook, Expression)
COPY_XQ4(FunctionRef, URI, Name, NumArgs, Instance)
COPY1(UDelete, Expression)
COPY2(URename, Target, Name)
COPY2(UReplace, Target, Expression)
COPY2(UReplaceValueOf, Target, Expression)
COPY2(UInsertAsFirst, Source, Target)
COPY2(UInsertAsLast, Source, Target)
COPY2(UInsertInto, Source, Target)
COPY2(UInsertAfter, Source, Target)
COPY2(UInsertBefore, Source, Target)
COPY2(UApplyUpdates, Expression, RevalidationMode)
COPY3(FTContains, Argument, Selection, Ignore);
COPY_XQ2(Nav, Steps, SortAdded);
COPY_FULL3(UserFunction, XQUserFunctionInstance, FunctionDefinition, Arguments, AddReturnCheck);


ASTNode *ASTCopier::optimizeInlineFunction(XQInlineFunction *item)
{
  XQInlineFunction *result = new (mm_) XQInlineFunction(item->getUserFunction(), item->getNumArgs(), item->getInstance(), mm_);
  ASTVisitor::optimizeInlineFunction(result);

  if(result->getUserFunction()) {
    // Update the pointers to the XQUserFunction in the instance
    UpdateInstance().run(result->getInstance(), item->getUserFunction(), result->getUserFunction());
  }

  COPY_IMPL();
}

ASTNode *ASTCopier::optimizeFunction(XQFunction *item)
{
  const XMLCh *uri = item->getFunctionURI();
  const XMLCh *name = item->getFunctionName();

  XQFunction *result = 0;
  if(uri == XQFunction::XMLChFunctionURI) {
    if(name == FunctionPut::name) {
      result = new (mm_) FunctionPut(((FunctionPut*)item)->getBaseURI(), item->getArguments(), mm_);
    }
    else if(name == FunctionCodepointEqual::name) {
      result = new (mm_) FunctionCodepointEqual(((FunctionCodepointEqual*)item)->getCollation(), item->getArguments(), mm_);
    }
  }

  if(result == 0)
    result = (XQFunction*)context_->lookUpFunction(item->getFunctionURI(), item->getFunctionName(), item->getArguments());

  ASTVisitor::optimizeFunction(result);
  COPY_IMPL();
}

#define OP_CHECK(opName) \
  else if(item->getOperatorName() == opName::name) \
    result = new (mm_) opName(item->getArguments(), mm_)

ASTNode *ASTCopier::optimizeOperator(XQOperator *item)
{
  XQOperator *result = 0;

  if(item->getOperatorName() == GeneralComp::name) {
    GeneralComp *gen = (GeneralComp*)item;
    result = new (mm_) GeneralComp(gen->getOperation(), item->getArguments(), gen->getCollation(), gen->getXPath1CompatibilityMode(), mm_);
  }
  else if(item->getOperatorName() == OrderComparison::name) {
    OrderComparison *ord = (OrderComparison*)item;
    result = new (mm_) OrderComparison(item->getArguments(), ord->getTestBefore(), mm_);
  }
  else if(item->getOperatorName() == UnaryMinus::name) {
    UnaryMinus *un = (UnaryMinus*)item;
    result = new (mm_) UnaryMinus(un->getIsPositive(), item->getArguments(), mm_);
  }
  OP_CHECK(And);
  OP_CHECK(Divide);
  OP_CHECK(Equals);
  OP_CHECK(Except);
  OP_CHECK(GreaterThan);
  OP_CHECK(GreaterThanEqual);
  OP_CHECK(IntegerDivide);
  OP_CHECK(Intersect);
  OP_CHECK(LessThan);
  OP_CHECK(LessThanEqual);
  OP_CHECK(Minus);
  OP_CHECK(Mod);
  OP_CHECK(Multiply);
  OP_CHECK(NodeComparison);
  OP_CHECK(NotEquals);
  OP_CHECK(Or);
  OP_CHECK(Plus);
  OP_CHECK(Range);
  OP_CHECK(Union);

  ASTVisitor::optimizeOperator(result);
  COPY_IMPL();
}

static XQTypeswitch::Case *copyCase(const XQTypeswitch::Case *in, XPath2MemoryManager *mm)
{
  XQTypeswitch::Case *result = new (mm) XQTypeswitch::Case(in->getQName(), in->getURI(), in->getName(), in->getSequenceType(), in->getExpression());
  result->setLocationInfo(in);
  return result;
}

ASTNode *ASTCopier::optimizeTypeswitch(XQTypeswitch *item)
{
  XQTypeswitch::Cases *newCases = new (mm_) XQTypeswitch::Cases(XQillaAllocator<XQTypeswitch::Case*>(mm_));
  XQTypeswitch::Cases *clauses = const_cast<XQTypeswitch::Cases *>(item->getCases());
  for(XQTypeswitch::Cases::iterator i = clauses->begin(); i != clauses->end(); ++i) {
    newCases->push_back(copyCase(*i, mm_));
  }

  XQTypeswitch *result = new (mm_) XQTypeswitch(item->getExpression(), newCases, copyCase(item->getDefaultCase(), mm_), mm_);

  ASTVisitor::optimizeTypeswitch(result);
  COPY_IMPL();
}

ASTNode *ASTCopier::optimizeFunctionCall(XQFunctionCall *item)
{
  VectorOfASTNodes *newArgs = new (mm_) VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm_));
  *newArgs = *item->getArguments();

  XQFunctionCall *result = new (mm_) XQFunctionCall(item->getQName(), newArgs, mm_);

  ASTVisitor::optimizeFunctionCall(result);
  COPY_IMPL();
}

ASTNode *ASTCopier::optimizeDOMConstructor(XQDOMConstructor *item)
{
  VectorOfASTNodes *newChildren = 0;
  VectorOfASTNodes *children = const_cast<VectorOfASTNodes *>(item->getChildren());
  if(children) {
    newChildren = new (mm_) VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm_));
    *newChildren = *children;
  }
  
  VectorOfASTNodes *newAttrs = 0;
  VectorOfASTNodes *attrs = const_cast<VectorOfASTNodes *>(item->getAttributes());
  if(attrs) {
    newAttrs = new (mm_) VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm_));
    *newAttrs = *attrs;
  }

  XQDOMConstructor *result = 0;

  if(item->getNodeType() == Node::document_string) {
    result = new (mm_) XQDocumentConstructor(item->getValue(), mm_);
  }
  else if(item->getNodeType() == Node::element_string) {
    RefHashTableOf< XMLCh > *newNamespaces = 0;
    RefHashTableOf< XMLCh > *namespaces = ((XQElementConstructor*)item)->getNamespaces();
    if(namespaces != 0) {
      newNamespaces = new (mm_) RefHashTableOf< XMLCh >(5, false, mm_);
      RefHashTableOfEnumerator<XMLCh> nsEnumVal(namespaces, false, mm_);
      RefHashTableOfEnumerator<XMLCh> nsEnumKey(namespaces, false, mm_);
      while(nsEnumVal.hasMoreElements()) {
        newNamespaces->put(nsEnumKey.nextElementKey(), &nsEnumVal.nextElement());
      }
    }

    result = new (mm_) XQElementConstructor(item->getName(), newAttrs, newChildren, newNamespaces, mm_);
  }
  else if(item->getNodeType() == Node::attribute_string) {
    result = new (mm_) XQAttributeConstructor(item->getName(), newChildren, mm_);
  }
  else if(item->getNodeType() == Node::namespace_string) {
    result = new (mm_) XQNamespaceConstructor(item->getName(), newChildren, mm_);
  }
  else if(item->getNodeType() == Node::text_string) {
    result = new (mm_) XQTextConstructor(item->getValue(), mm_);
  }
  else if(item->getNodeType() == Node::comment_string) {
    result = new (mm_) XQCommentConstructor(item->getValue(), mm_, ((XQCommentConstructor*)item)->isXSLT());
  }
  else if(item->getNodeType() == Node::processing_instruction_string) {
    result = new (mm_) XQPIConstructor(item->getName(), item->getValue(), mm_, ((XQPIConstructor*)item)->isXSLT());
  }

  ASTVisitor::optimizeDOMConstructor(result);
  COPY_IMPL();
}

ASTNode *ASTCopier::optimizeSimpleContent(XQSimpleContent *item)
{
  VectorOfASTNodes *newChildren = 0;
  VectorOfASTNodes *children = const_cast<VectorOfASTNodes *>(item->getChildren());
  if(children) {
    newChildren = new (mm_) VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm_));
    *newChildren = *children;
  }

  XQSimpleContent *result = new (mm_) XQSimpleContent(newChildren, mm_);

  ASTVisitor::optimizeSimpleContent(result);
  COPY_IMPL();
}

static TemplateArguments *copyTemplateArgs(const TemplateArguments *in, XPath2MemoryManager *mm)
{
  if(in == 0) return 0;
  TemplateArguments *result = new (mm) TemplateArguments(XQillaAllocator<XQTemplateArgument*>(mm));
  for(TemplateArguments::const_iterator i = in->begin(); i != in->end(); ++i) {
    XQTemplateArgument *arg = new (mm) XQTemplateArgument((*i)->qname, (*i)->value, mm);
    arg->uri = (*i)->uri;
    arg->name = (*i)->name;
    arg->seqType = (*i)->seqType;
    arg->varSrc.copy((*i)->varSrc);
    result->push_back(arg);
  }
  return result;
}

ASTNode *ASTCopier::optimizeCallTemplate(XQCallTemplate *item)
{
  XQCallTemplate *result = new (mm_) XQCallTemplate(item->getQName(), item->getURI(), item->getName(), item->getASTName(),
                                                    copyTemplateArgs(item->getArguments(), mm_), item->getTemplates(), mm_);
  ASTVisitor::optimizeCallTemplate(result);
  COPY_IMPL();
}

ASTNode *ASTCopier::optimizeApplyTemplates(XQApplyTemplates *item)
{
  XQApplyTemplates *result = new (mm_) XQApplyTemplates(item->getExpression(), copyTemplateArgs(item->getArguments(), mm_),
                                                        item->getMode(), item->getTemplates(), mm_);
  ASTVisitor::optimizeApplyTemplates(result);
  COPY_IMPL();
}

ASTNode *ASTCopier::optimizeMap(XQMap *item)
{
  XQMap *result = new (mm_) XQMap(item->getArg1(), item->getArg2(), item->getURI(), item->getName(), mm_);
  ASTVisitor::optimizeMap(result);
  const_cast<StaticAnalysis&>(result->getVarSRC()).copy(item->getVarSRC());
  COPY_IMPL();
}

ASTNode *ASTCopier::optimizeFunctionDeref(XQFunctionDeref *item)
{
  VectorOfASTNodes *newArgs = 0;
  VectorOfASTNodes *args = const_cast<VectorOfASTNodes *>(item->getArguments());
  if(args) {
    newArgs = new (mm_) VectorOfASTNodes(XQillaAllocator<ASTNode*>(mm_));
    *newArgs = *args;
  }

  XQFunctionDeref *result = new (mm_) XQFunctionDeref(item->getExpression(), newArgs, mm_);

  ASTVisitor::optimizeFunctionDeref(item);
  COPY_IMPL();
}

ASTNode *ASTCopier::optimizeUTransform(UTransform *item)
{
  VectorOfCopyBinding *newBindings = new (mm_) VectorOfCopyBinding(XQillaAllocator<CopyBinding*>(mm_));
  VectorOfCopyBinding *bindings = const_cast<VectorOfCopyBinding*>(item->getBindings());
  for(VectorOfCopyBinding::iterator i = bindings->begin(); i != bindings->end(); ++i) {
    newBindings->push_back(new (mm_) CopyBinding(mm_, **i));
  }

  UTransform *result = new (mm_) UTransform(newBindings, item->getModifyExpr(), item->getReturnExpr(), item->getRevalidationMode(), mm_);

  ASTVisitor::optimizeUTransform(result);
  COPY_IMPL();
}

TupleNode *ASTCopier::optimizeContextTuple(ContextTuple *item)
{
  ContextTuple *result = new (mm_) ContextTuple(mm_);
  ASTVisitor::optimizeContextTuple(result);
  result->setLocationInfo(item);
  result->setMin(item->getMin());
  result->setMax(item->getMax());
  return result;
}

TupleNode *ASTCopier::optimizeForTuple(ForTuple *item)
{
  ForTuple *result = new (mm_) ForTuple(item->getParent(), item->getVarURI(), item->getVarName(),
                                        item->getPosURI(), item->getPosName(), item->getExpression(), mm_);
  ASTVisitor::optimizeForTuple(result);
  result->setLocationInfo(item);
  const_cast<StaticAnalysis&>(result->getVarSRC()).copy(item->getVarSRC());
  const_cast<StaticAnalysis&>(result->getPosSRC()).copy(item->getPosSRC());
  result->setMin(item->getMin());
  result->setMax(item->getMax());
  return result;
}

TupleNode *ASTCopier::optimizeLetTuple(LetTuple *item)
{
  LetTuple *result = new (mm_) LetTuple(item->getParent(), item->getVarURI(), item->getVarName(),
                                        item->getExpression(), mm_);
  ASTVisitor::optimizeLetTuple(result);
  result->setLocationInfo(item);
  const_cast<StaticAnalysis&>(result->getVarSRC()).copy(item->getVarSRC());
  result->setMin(item->getMin());
  result->setMax(item->getMax());
  return result;
}

TupleNode *ASTCopier::optimizeWhereTuple(WhereTuple *item)
{
  WhereTuple *result = new (mm_) WhereTuple(item->getParent(), item->getExpression(), mm_);
  ASTVisitor::optimizeWhereTuple(result);
  result->setLocationInfo(item);
  result->setMin(item->getMin());
  result->setMax(item->getMax());
  return result;
}

TupleNode *ASTCopier::optimizeOrderByTuple(OrderByTuple *item)
{
  OrderByTuple *result = new (mm_) OrderByTuple(item->getParent(), item->getExpression(), item->getModifiers(),
                                        item->getCollation(), mm_);
  ASTVisitor::optimizeOrderByTuple(result);
  result->setLocationInfo(item);
  const_cast<StaticAnalysis&>(result->getUsedSRC()).copy(item->getUsedSRC());
  result->setMin(item->getMin());
  result->setMax(item->getMax());
  return result;
}

TupleNode *ASTCopier::optimizeTupleDebugHook(TupleDebugHook *item)
{
  TupleDebugHook *result = new (mm_) TupleDebugHook(item->getParent(), mm_);
  ASTVisitor::optimizeTupleDebugHook(result);
  result->setLocationInfo(item);
  result->setMin(item->getMin());
  result->setMax(item->getMax());
  return result;
}
