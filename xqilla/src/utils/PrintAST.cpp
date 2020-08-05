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
 * $Id: PrintAST.cpp 767 2009-06-05 13:51:32Z jpcs $
 */

// #define SHOW_QUERY_PATH_TREES

#include "../config/xqilla_config.h"
#include <iostream>
#include <sstream>

#include <xqilla/utils/PrintAST.hpp>
#include <xqilla/utils/UTF8Str.hpp>

#include "../dom-api/impl/XQillaExpressionImpl.hpp"
#include <xqilla/ast/ASTNode.hpp>
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
#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <xqilla/utils/XStr.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/ast/XQDOMConstructor.hpp>
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
#include <xqilla/ast/XQReturn.hpp>
#include <xqilla/ast/XQNamespaceBinding.hpp>
#include <xqilla/ast/XQFunctionConversion.hpp>
#include <xqilla/ast/XQAnalyzeString.hpp>
#include <xqilla/ast/XQCopyOf.hpp>
#include <xqilla/ast/XQCopy.hpp>
#include <xqilla/ast/XQCallTemplate.hpp>
#include <xqilla/ast/XQApplyTemplates.hpp>
#include <xqilla/ast/XQInlineFunction.hpp>
#include <xqilla/ast/XQFunctionDeref.hpp>
#include <xqilla/ast/XQFunctionRef.hpp>
#include <xqilla/ast/XQMap.hpp>
#include <xqilla/debug/ASTDebugHook.hpp>
#include <xqilla/functions/XQUserFunction.hpp>

#include <xqilla/fulltext/FTContains.hpp>
#include <xqilla/fulltext/FTSelection.hpp>
#include <xqilla/fulltext/FTWords.hpp>
#include <xqilla/fulltext/FTOr.hpp>
#include <xqilla/fulltext/FTAnd.hpp>
#include <xqilla/fulltext/FTMildnot.hpp>
#include <xqilla/fulltext/FTUnaryNot.hpp>
#include <xqilla/fulltext/FTOrder.hpp>
#include <xqilla/fulltext/FTDistance.hpp>
#include <xqilla/fulltext/FTRange.hpp>
#include <xqilla/fulltext/FTScope.hpp>
#include <xqilla/fulltext/FTContent.hpp>
#include <xqilla/fulltext/FTWindow.hpp>

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

#include <xqilla/ast/ContextTuple.hpp>
#include <xqilla/ast/ForTuple.hpp>
#include <xqilla/ast/LetTuple.hpp>
#include <xqilla/ast/WhereTuple.hpp>
#include <xqilla/ast/OrderByTuple.hpp>
#include <xqilla/debug/TupleDebugHook.hpp>

#include <xqilla/functions/FunctionDoc.hpp>
#include <xqilla/functions/FunctionDocument.hpp>
#include <xqilla/functions/FunctionCollection.hpp>
#include <xqilla/functions/FunctionParseXML.hpp>
#include <xqilla/functions/FunctionParseJSON.hpp>
#include <xqilla/optimizer/QueryPathNode.hpp>

XERCES_CPP_NAMESPACE_USE;

using namespace std;

static const int INDENT = 1;

string PrintAST::print(const XQillaExpression *expr, const DynamicContext *context, int indent)
{
  return print(((const XQillaExpressionImpl *)expr)->getCompiledExpression(), context, indent);
}

string PrintAST::print(const XQQuery *query, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  if(query->getIsLibraryModule()) {
    s << in << "<Module";
  }
  else {
    s << in << "<XQuery";
  }
  if(query->getModuleTargetNamespace()) {
    s << " targetNamespace=\"" << UTF8(query->getModuleTargetNamespace()) << "\"";
  }
  s << ">" << endl;

  const ImportedModules &modules = query->getImportedModules();
  for(ImportedModules::const_iterator it1 = modules.begin();
      it1 != modules.end(); ++it1) {
    s << print(*it1, context, indent + INDENT);
  }

  PrintAST p;

  for(std::vector<XQUserFunction*, XQillaAllocator<XQUserFunction*> >::const_iterator i = query->getFunctions().begin();
      i != query->getFunctions().end(); ++i) {
    s << p.printXQUserFunction(*i, context, indent + INDENT);
  }
  for(vector<XQGlobalVariable*, XQillaAllocator<XQGlobalVariable*> >::const_iterator it = query->getVariables().begin();
      it != query->getVariables().end(); ++it) {
    s << p.printGlobal(*it, context, indent + INDENT);
  }

  if(query->getQueryBody() != 0)
    s << p.printASTNode(query->getQueryBody(), context, indent + INDENT);

  if(query->getIsLibraryModule()) {
    s << in << "</Module>" << endl;
  }
  else {
    s << in << "</XQuery>";
  }

  return s.str();
}

string PrintAST::printXQUserFunction(const XQUserFunction *f, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  string name;
  if(f->getQName() != 0) {
    name += UTF8(f->getQName());
  }
  else if(f->getName() != 0) {
    name += "{";
    name += UTF8(f->getURI());
    name += "}";
    name += UTF8(f->getName());
  }

  if(f->isTemplate()) {
    s << in << "<TemplateDefinition";
  }
  else {
    s << in << "<FunctionDefinition";
  }
  if(name != "") {
    s << " name=\"" << name << "\"";
  }
  if(f->getArgumentSpecs() == NULL && f->getPattern() == NULL && f->getFunctionBody() == NULL && f->getReturnType() == NULL) {
    s << "/>" << endl;
  } else {
    s <<">" << endl;

    if(f->getPattern() != NULL) {
      s << in << "  <Pattern>" << endl;
      VectorOfASTNodes::iterator patIt = f->getPattern()->begin();
      for(; patIt != f->getPattern()->end(); ++patIt) {
        s << printASTNode(*patIt, context, indent + INDENT + INDENT);
      }
      s << in << "  </Pattern>" << endl;
    }

    if(f->getArgumentSpecs() != NULL) {
      XQUserFunction::ArgumentSpecs::const_iterator argIt;
      for(argIt = f->getArgumentSpecs()->begin(); argIt != f->getArgumentSpecs()->end(); ++argIt) {
        if((*argIt)->isUsed()) {
          s << in << "  <Parameter name=\"";
          if((*argIt)->getQName() != 0) {
            s << UTF8((*argIt)->getQName());
          }
          else {
            s << "{" << UTF8((*argIt)->getURI()) << "}" << UTF8((*argIt)->getName());
          }
          if((*argIt)->getType() == 0) {
            s << "\"/>" << endl;
          }
          else {
            s << "\">" << endl;
            s << printSequenceType((*argIt)->getType(), context, indent + INDENT + INDENT);
            s << in << "  </Parameter>" << endl;
          }
        }
      }
    }

    if(f->getFunctionBody() != NULL) {
      s << printASTNode(f->getFunctionBody(), context, indent + INDENT);
    }

//     if(f->getReturnType() != NULL) {
//       s << printSequenceType(f->getReturnType(), context, indent + INDENT);
//     }

    if(f->isTemplate()) {
      s << in << "</TemplateDefinition>" << endl;
    }
    else {
      s << in << "</FunctionDefinition>" << endl;
    }
  }

  return s.str();
}

string PrintAST::print(const ASTNode *item, const DynamicContext *context, int indent)
{
  PrintAST p;
  return p.printASTNode(item, context, indent);
}

string PrintAST::printASTNode(const ASTNode *item, const DynamicContext *context, int indent)
{
  switch(item->getType()) {
  case ASTNode::LITERAL: {
    return printLiteral((XQLiteral *)item, context, indent);
    break;
  }
  case ASTNode::QNAME_LITERAL: {
    return printQNameLiteral((XQQNameLiteral *)item, context, indent);
    break;
  }
  case ASTNode::NUMERIC_LITERAL: {
    return printNumericLiteral((XQNumericLiteral *)item, context, indent);
    break;
  }
  case ASTNode::SEQUENCE: {
    return printSequence((XQSequence *)item, context, indent);
    break;
  }
  case ASTNode::FUNCTION: {
    return printFunction((XQFunction *)item, context, indent);
    break;
  }
  case ASTNode::NAVIGATION: {
    return printNav((XQNav *)item, context, indent);
    break;
  }
  case ASTNode::VARIABLE: {
    return printVariable((XQVariable *)item, context, indent);
    break;
  }
  case ASTNode::STEP: {
    return printStep((XQStep *)item, context, indent);
    break;
  }
  case ASTNode::IF: {
    return printIf((XQIf *)item, context, indent);
    break;
  }
  case ASTNode::INSTANCE_OF: {
    return printInstanceOf((XQInstanceOf *)item, context, indent);
    break;
  }
  case ASTNode::CASTABLE_AS: {
    return printCastableAs((XQCastableAs *)item, context, indent);
    break;
  }
  case ASTNode::CAST_AS: {
    return printCastAs((XQCastAs *)item, context, indent);
    break;
  }
  case ASTNode::TREAT_AS: {
    return printTreatAs((XQTreatAs *)item, context, indent);
    break;
  }
  case ASTNode::OPERATOR: {
    return printOperator((XQOperator *)item, context, indent);
    break;
  }
  case ASTNode::CONTEXT_ITEM: {
    return printContextItem((XQContextItem *)item, context, indent);
    break;
  }
  case ASTNode::QUANTIFIED: {
    return printQuantified((XQQuantified *)item, context, indent);
    break;
  }
  case ASTNode::TYPESWITCH: {
    return printTypeswitch((XQTypeswitch *)item, context, indent);
    break;
  }
  case ASTNode::VALIDATE: {
    return printValidate((XQValidate *)item, context, indent);
    break;
  }
  case ASTNode::FUNCTION_CALL: {
    return printFunctionCall((XQFunctionCall *)item, context, indent);
    break;
  }
  case ASTNode::DOM_CONSTRUCTOR: {
    return printDOMConstructor((XQDOMConstructor *)item, context, indent);
    break;
  }
  case ASTNode::SIMPLE_CONTENT: {
    return printSimpleContent((XQSimpleContent *)item, context, indent);
    break;
  }
  case ASTNode::ORDERING_CHANGE: {
    return printOrderingChange((XQOrderingChange *)item, context, indent);
    break;
  }
  case ASTNode::ATOMIZE: {
    return printAtomize((XQAtomize *)item, context, indent);
    break;
  }
  case ASTNode::EBV: {
    return printEffectiveBooleanValue((XQEffectiveBooleanValue *)item, context, indent);
    break;
  }
  case ASTNode::MAP: {
    return printMap((XQMap *)item, context, indent);
    break;
  }
  case ASTNode::DOCUMENT_ORDER: {
    return printDocumentOrder((XQDocumentOrder *)item, context, indent);
    break;
  }
  case ASTNode::XPATH1_CONVERT: {
    return printXPath1CompatConvertFunctionArg((XPath1CompatConvertFunctionArg *)item, context, indent);
    break;
  }
  case ASTNode::PROMOTE_UNTYPED: {
    return printPromoteUntyped((XQPromoteUntyped *)item, context, indent);
    break;
  }
  case ASTNode::PROMOTE_NUMERIC: {
    return printPromoteNumeric((XQPromoteNumeric *)item, context, indent);
    break;
  }
  case ASTNode::PROMOTE_ANY_URI: {
    return printPromoteAnyURI((XQPromoteAnyURI *)item, context, indent);
    break;
  }
  case ASTNode::PREDICATE: {
    return printPredicate((XQPredicate *)item, context, indent);
    break;
  }
  case ASTNode::NAME_EXPRESSION: {
    return printNameExpression((XQNameExpression *)item, context, indent);
    break;
  }
  case ASTNode::CONTENT_SEQUENCE: {
    return printContentSequence((XQContentSequence *)item, context, indent);
    break;
  }
  case ASTNode::DIRECT_NAME: {
    return printDirectName((XQDirectName *)item, context, indent);
    break;
  }
  case ASTNode::USER_FUNCTION: {
    return printUserFunction((XQUserFunctionInstance *)item, context, indent);
    break;
  }
  case ASTNode::FTCONTAINS: {
    return printFTContains((FTContains *)item, context, indent);
    break;
  }
  case ASTNode::UDELETE: {
    return printUDelete((UDelete *)item, context, indent);
    break;
  }
  case ASTNode::URENAME: {
    return printURename((URename *)item, context, indent);
    break;
  }
  case ASTNode::UREPLACE: {
    return printUReplace((UReplace *)item, context, indent);
    break;
  }
  case ASTNode::UREPLACE_VALUE_OF: {
    return printUReplaceValueOf((UReplaceValueOf *)item, context, indent);
    break;
  }
  case ASTNode::UINSERT_AS_FIRST: {
    return printUInsertAsFirst((UInsertAsFirst *)item, context, indent);
    break;
  }
  case ASTNode::UINSERT_AS_LAST: {
    return printUInsertAsLast((UInsertAsLast *)item, context, indent);
    break;
  }
  case ASTNode::UINSERT_INTO: {
    return printUInsertInto((UInsertInto *)item, context, indent);
    break;
  }
  case ASTNode::UINSERT_AFTER: {
    return printUInsertAfter((UInsertAfter *)item, context, indent);
    break;
  }
  case ASTNode::UINSERT_BEFORE: {
    return printUInsertBefore((UInsertBefore *)item, context, indent);
    break;
  }
  case ASTNode::UTRANSFORM: {
    return printUTransform((UTransform *)item, context, indent);
    break;
  }
  case ASTNode::UAPPLY_UPDATES: {
    return printUApplyUpdates((UApplyUpdates *)item, context, indent);
    break;
  }
  case ASTNode::RETURN: {
    return printReturn((XQReturn*)item, context, indent);
    break;
  }
  case ASTNode::NAMESPACE_BINDING: {
    return printNamespaceBinding((XQNamespaceBinding*)item, context, indent);
    break;
  }
  case ASTNode::FUNCTION_CONVERSION: {
    return printFunctionConversion((XQFunctionConversion*)item, context, indent);
    break;
  }
  case ASTNode::ANALYZE_STRING: {
    return printAnalyzeString((XQAnalyzeString*)item, context, indent);
    break;
  }
  case ASTNode::COPY_OF: {
    return printCopyOf((XQCopyOf*)item, context, indent);
    break;
  }
  case ASTNode::COPY: {
    return printCopy((XQCopy*)item, context, indent);
    break;
  }
  case ASTNode::DEBUG_HOOK: {
    return printASTDebugHook((ASTDebugHook*)item, context, indent);
    break;
  }
  case ASTNode::CALL_TEMPLATE: {
    return printCallTemplate((XQCallTemplate*)item, context, indent);
    break;
  }
  case ASTNode::APPLY_TEMPLATES: {
    return printApplyTemplates((XQApplyTemplates*)item, context, indent);
    break;
  }
  case ASTNode::INLINE_FUNCTION: {
    return printInlineFunction((XQInlineFunction*)item, context, indent);
    break;
  }
  case ASTNode::FUNCTION_DEREF: {
    return printFunctionDeref((XQFunctionDeref*)item, context, indent);
    break;
  }
  case ASTNode::FUNCTION_REF: {
    return printFunctionRef((XQFunctionRef*)item, context, indent);
    break;
  }
  }
  return printUnknown(item, context, indent);
}

string PrintAST::printUnknown(const ASTNode *item, const DynamicContext *context, int indent)
{
  return getIndent(indent) + "<Unknown/>\n";
}

string PrintAST::printFunction(const XQFunction *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XMLCh *funUri = item->getFunctionURI();
  const XMLCh *funName = item->getFunctionName();

  QueryPathNode *queryPathTree = 0;
#ifdef SHOW_QUERY_PATH_TREES
  if(funUri == XQFunction::XMLChFunctionURI) {
    if(funName == FunctionDoc::name) {
      queryPathTree = ((FunctionDoc*)item)->getQueryPathTree();
    }
    else if(funName == FunctionDocument::name) {
      queryPathTree = ((FunctionDocument*)item)->getQueryPathTree();
    }
    else if(funName == FunctionCollection::name) {
      queryPathTree = ((FunctionCollection*)item)->getQueryPathTree();
    }
  }
  else if(funUri == XQillaFunction::XMLChFunctionURI) {
    if(funName == FunctionParseXML::name) {
      queryPathTree = ((FunctionParseXML*)item)->getQueryPathTree();
    }
    else if(funName == FunctionParseJSON::name) {
      queryPathTree = ((FunctionParseJSON*)item)->getQueryPathTree();
    }
  }
#endif

  string name("{");
  name += UTF8(funUri);
  name += "}";
  name += UTF8(funName);

  const VectorOfASTNodes &args = item->getArguments();
  if(args.empty() && queryPathTree == 0) {
    s << in << "<Function name=\"" << name << "\"/>" << endl;
  }
  else {
    s << in << "<Function name=\"" << name << "\">" << endl;
    for(VectorOfASTNodes::const_iterator i = args.begin(); i != args.end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT);
    }
    if(queryPathTree)
      s << queryPathTree->toString(indent + INDENT);
    s << in << "</Function>" << endl;
  }

  return s.str();
}

string PrintAST::printOperator(const XQOperator *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string name(UTF8(item->getOperatorName()));
  if(name == "comp") {
    name = getComparisonOperationName(((const GeneralComp *)item)->getOperation());
  }

  const VectorOfASTNodes &args = item->getArguments();
  if(args.empty()) {
    s << in << "<Operator name=\"" << name << "\"/>" << endl;
  }
  else {
    s << in << "<Operator name=\"" << name << "\">" << endl;
    for(VectorOfASTNodes::const_iterator i = args.begin(); i != args.end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT);
    }
    s << in << "</Operator>" << endl;
  }

  return s.str();
}

string PrintAST::printContextItem(const XQContextItem *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<ContextItem/>" << endl;

  return s.str();
}

string PrintAST::printLiteral(const XQLiteral *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Literal";
  s << " value=\"" << UTF8(item->getValue());
  s << "\" typeuri=\"" << UTF8(item->getTypeURI());
  s << "\" typename=\"" << UTF8(item->getTypeName());
  s << "\"/>" << endl;

  return s.str();
}

string PrintAST::printQNameLiteral(const XQQNameLiteral *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<QNameLiteral";
  s << " uri=\"" << UTF8(item->getURI());
  s << "\" prefix=\"" << UTF8(item->getPrefix());
  s << "\" localname=\"" << UTF8(item->getLocalname());
  s << "\" typeuri=\"" << UTF8(item->getTypeURI());
  s << "\" typename=\"" << UTF8(item->getTypeName());
  s << "\"/>" << endl;

  return s.str();
}

string PrintAST::printNumericLiteral(const XQNumericLiteral *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<NumericLiteral";
  char obuf[1024];
  m_apm_to_string_mt(obuf, item->getRawValue().m_apm_datalength, const_cast<M_APM>(&item->getRawValue()));
  s << " value=\"" << obuf;
  s << "\" typeuri=\"" << UTF8(item->getTypeURI());
  s << "\" typename=\"" << UTF8(item->getTypeName());
  s << "\"/>" << endl;

  return s.str();
}

string PrintAST::printNav(const XQNav *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XQNav::Steps &steps = item->getSteps();

  s << in << "<Navigation>" << endl;
  for(XQNav::Steps::const_iterator i = steps.begin(); i != steps.end(); ++i) {
    s << printASTNode(i->step, context, indent + INDENT);
  }
  s << in << "</Navigation>" << endl;

  return s.str();
}

string PrintAST::printStep(const XQStep *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  const NodeTest *step = item->getNodeTest();

  s << in << "<Step";
  s << " axis=\"" << getAxisName(item->getAxis()) << "\"";
  
  SequenceType::ItemType *type = step->getItemType();
  if(type == 0) {
    if(step->getNamespaceWildcard()) {
      s << " uri=\"*\"";
    }
    else {
      if(step->getNodePrefix() != 0) {
        s << " prefix=\"" << UTF8(step->getNodePrefix()) << "\"";
      }
      if(step->getNodeUri() != 0) {
        s << " uri=\"" << UTF8(step->getNodeUri()) << "\"";
      }
    }
    if(step->getNameWildcard()) {
      s << " name=\"*\"";
    }
    else if(step->getNodeName() != 0) {
      s << " name=\"" << UTF8(step->getNodeName()) << "\"";
    }
    
    if(step->getTypeWildcard()) {
      s << " nodeType=\"*\"";
    }
    else if(step->isNodeTypeSet()) {
      s << " nodeType=\"" << UTF8(step->getNodeType()) << "\"";
    }
  }

  if(type != 0) {
    s << ">" << endl;
    if(type != 0) {
      s << in << "  <ItemType";
      XMLBuffer buf;
      type->toBuffer(buf);
      s << " type=\"" << UTF8(buf.getRawBuffer()) << "\"";
      s << "/>" << endl;
    }
    s << in << "</Step>" << endl;
  }
  else {
    s << "/>" << endl;
  }

  return s.str();
}

string PrintAST::printSequence(const XQSequence *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  const VectorOfASTNodes &children = item->getChildren();

  if(children.empty()) {
    s << in << "<Sequence/>" << endl;
  }
  else {
    s << in << "<Sequence>" << endl;
    for(VectorOfASTNodes::const_iterator i = children.begin(); i != children.end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT);
    }
    s << in << "</Sequence>" << endl;
  }

  return s.str();
}

string PrintAST::printVariable(const XQVariable *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string prefix(UTF8(item->getPrefix()));
  string name(UTF8(item->getName()));
  if(prefix != "") {
    name = prefix + ":" + name;
  }

  s << in << "<Variable name=\"" << name << "\"/>" << endl;

  return s.str();
}

string PrintAST::printIf(const XQIf *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<If>" << endl;
  s << in << "  <Test>" << endl;
  s << printASTNode(item->getTest(), context, indent + INDENT + INDENT);
  s << in << "  </Test>" << endl;
  s << in << "  <Then>" << endl;
  s << printASTNode(item->getWhenTrue(), context, indent + INDENT + INDENT);
  s << in << "  </Then>" << endl;
  s << in << "  <Else>" << endl;
  s << printASTNode(item->getWhenFalse(), context, indent + INDENT + INDENT);
  s << in << "  </Else>" << endl;
  s << in << "</If>" << endl;

  return s.str();
}

string PrintAST::printInstanceOf(const XQInstanceOf *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<InstanceOf>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</InstanceOf>" << endl;

  return s.str();
}

string PrintAST::printCastableAs(const XQCastableAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<CastableAs>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</CastableAs>" << endl;

  return s.str();
}

string PrintAST::printCastAs(const XQCastAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<CastAs>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</CastAs>" << endl;

  return s.str();
}

string PrintAST::printTreatAs(const XQTreatAs *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<TreatAs>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</TreatAs>" << endl;

  return s.str();
}

string PrintAST::printUserFunction(const XQUserFunctionInstance *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XMLCh *funUri = item->getFunctionURI();
  const XMLCh *funName = item->getFunctionName();

  string name("{");
  name += UTF8(funUri);
  name += "}";
  name += UTF8(funName);

  const VectorOfASTNodes &args = item->getArguments();

  if(item->getFunctionDefinition()->isTemplate())
    s << in << "<Template name=\"" << name << "\"";
  else s << in << "<UserFunction name=\"" << name << "\"";

  if(item->getFunctionDefinition()->getArgumentSpecs() && !item->getFunctionDefinition()->getArgumentSpecs()->empty()) {
    s << ">" << endl;
    XQUserFunction::ArgumentSpecs::const_iterator binding = item->getFunctionDefinition()->getArgumentSpecs()->begin();
    for(VectorOfASTNodes::const_iterator arg = args.begin(); arg != args.end() && binding != item->getFunctionDefinition()->getArgumentSpecs()->end(); ++arg, ++binding) {
      if((*binding)->isUsed()) {
        s << in << "  <Binding name=\"";
        if((*binding)->getQName() != 0) {
          s << UTF8((*binding)->getQName());
        }
        else {
          s << "{" << UTF8((*binding)->getURI()) << "}" << UTF8((*binding)->getName());
        }
        s << "\">" << endl;
        s << printASTNode(*arg, context, indent + INDENT + INDENT);
        s << in << "  </Binding>" << endl;
      }
    }

    // We don't output the body, as it may result in an infinite loop
    // for recursive user functions - jpcs
    if(item->getFunctionDefinition()->isTemplate())
      s << in << "</Template>" << endl;
    else s << in << "</UserFunction>" << endl;
  } else {
    s << "/>" << endl;
  }

  return s.str();
}

string PrintAST::printTypeswitch(const XQTypeswitch *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const XQTypeswitch::Cases *cases = item->getCases();

  s << in << "<TypeSwitch>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  for(XQTypeswitch::Cases::const_iterator i = cases->begin(); i != cases->end(); ++i) {
    s << printCase(*i, context, indent + INDENT);
  }
  s << printCase(item->getDefaultCase(), context, indent + INDENT);
  s << in << "</TypeSwitch>" << endl;

  return s.str();
}

string PrintAST::printCase(const XQTypeswitch::Case *cse, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  if(cse->getSequenceType()) {
    s << in << "<Case";
    if(cse->isVariableUsed()) {
      s << " name=\"" << UTF8(cse->getQName()) << "\"";
    }
    s << ">" << endl;
    s << printSequenceType(cse->getSequenceType(), context, indent + INDENT);
    s << printASTNode(cse->getExpression(), context, indent + INDENT);
    s << in << "</Case>" << endl;
  }
  else {
    s << in << "<Default";
    if(cse->isVariableUsed()) {
      s << " name=\"" << UTF8(cse->getQName()) << "\"";
    }
    s << ">" << endl;
    s << printASTNode(cse->getExpression(), context, indent + INDENT);
    s << in << "</Default>" << endl;
  }

  return s.str();
}

string PrintAST::printValidate(const XQValidate *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Validate mode=\"";
  switch(item->getMode()) {
  case DocumentCache::VALIDATION_STRICT: {
    s << "strict";
    break;
  }
  case DocumentCache::VALIDATION_LAX: {
    s << "lax";
    break;
  }
  default: break;
  }
  s << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</Validate>" << endl;

  return s.str();
}

string PrintAST::printGlobal(const XQGlobalVariable *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  if(item->isParam()) {
    s << in << "<GlobalParam name=\"";
  }
  else {
    s << in << "<GlobalVar name=\"";
  }
  if(item->getVariableName() != 0) {
    s << UTF8(item->getVariableName());
  }
  else {
    s << "{" << UTF8(item->getVariableURI()) << "}" << UTF8(item->getVariableLocalName());
  }
  s << "\"";
  if(item->isExternal()) {
    s << " external=\"true\"";
  }
  if(item->getSequenceType() || item->getVariableExpr()) {
    s << ">" << endl;
    if(item->getSequenceType()) {
      s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
    }
    if(item->getVariableExpr()) {
      s << printASTNode(item->getVariableExpr(), context, indent + INDENT);
    }
    if(item->isParam()) {
      s << in << "</GlobalParam>" << endl;
    }
    else {
      s << in << "</GlobalVar>" << endl;
    }
  }
  else {
    s << "/>" << endl;
  }

  return s.str();
}

string PrintAST::printFunctionCall(const XQFunctionCall *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const VectorOfASTNodes *args = item->getArguments();
  if(args->empty()) {
    s << in << "<FunctionCall name=\"" << UTF8(item->getQName()) << "\"/>" << endl;
  }
  else {
    s << in << "<FunctionCall name=\"" << UTF8(item->getQName()) << "\">" << endl;
    for(VectorOfASTNodes::const_iterator i = args->begin(); i != args->end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT);
    }
    s << in << "</FunctionCall>" << endl;
  }

  return s.str();
}

string PrintAST::printDOMConstructor(const XQDOMConstructor *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<DOMConstructor type=\"" << UTF8(item->getNodeType());
  if(item->getName() ||
     (item->getAttributes() != 0 && !item->getAttributes()->empty()) ||
     (item->getChildren() != 0 && !item->getChildren()->empty()) ||
     item->getValue() != 0
#ifdef SHOW_QUERY_PATH_TREES
     || item->getQueryPathTree() != 0
#endif
     ) {
    s << "\">" << endl;
    if(item->getName()) {
      s << in << "  <Name>" << endl;
      s << printASTNode(item->getName(), context, indent + INDENT + INDENT);
      s << in << "  </Name>" << endl;
    }
    if(item->getAttributes() != 0 && !item->getAttributes()->empty()) {
      s << in << "  <Attributes>" << endl;
      for(VectorOfASTNodes::const_iterator i = item->getAttributes()->begin();
          i != item->getAttributes()->end(); ++i) {
        s << printASTNode(*i, context, indent + INDENT + INDENT);
      }
      s << in << "  </Attributes>" << endl;
    }
    if(item->getChildren() != 0 && !item->getChildren()->empty()) {
      s << in << "  <Children>" << endl;
      for(VectorOfASTNodes::const_iterator i = item->getChildren()->begin();
          i != item->getChildren()->end(); ++i) {
        s << printASTNode(*i, context, indent + INDENT + INDENT);
      }
      s << in << "  </Children>" << endl;
    }
    if(item->getValue() != 0) {
      s << in << "  <Children>" << endl;
      s << printASTNode(item->getValue(), context, indent + INDENT + INDENT);
      s << in << "  </Children>" << endl;
    }
#ifdef SHOW_QUERY_PATH_TREES
    if(item->getQueryPathTree())
      s << item->getQueryPathTree()->toString(indent + INDENT);
#endif
    s << in << "</DOMConstructor>" << endl;
  }
  else {
    s << "\"/>" << endl;
  }

  return s.str();
}

string PrintAST::printSimpleContent(const XQSimpleContent *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<SimpleContent";
  if(item->getChildren() != 0 && !item->getChildren()->empty()) {
    s << ">" << endl;
    for(VectorOfASTNodes::const_iterator i = item->getChildren()->begin();
        i != item->getChildren()->end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT + INDENT);
    }
    s << in << "</SimpleContent>" << endl;
  }
  else {
    s << "\"/>" << endl;
  }

  return s.str();
}

string PrintAST::printOrderingChange(const XQOrderingChange *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<OrderingChange value=\"";
  if(item->getOrderingValue() == StaticContext::ORDERING_ORDERED)
    s << "ordered";
  else s << "unordered";
  s << "\">" << endl;
  s << printASTNode(item->getExpr(), context, indent + INDENT);
  s << in << "</OrderingChange>" << endl;

  return s.str();
}

string PrintAST::printFTContains(const FTContains *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTContains>" << endl;
  s << printASTNode(item->getArgument(), context, indent + INDENT);
  s << printFTSelection(item->getSelection(), context, indent + INDENT);
  if(item->getIgnore() != NULL) {
    s << in << "  <ignore>" << endl;
    s << printASTNode(item->getIgnore(), context, indent + INDENT + INDENT);
    s << in << "  </ignore>" << endl;
  }
  s << in << "</FTContains>" << endl;

  return s.str();
}

string PrintAST::printFTSelection(const FTSelection *selection, const DynamicContext *context, int indent)
{
  switch(selection->getType()) {
  case FTSelection::OR: {
    return printFTOr((FTOr*)selection, context, indent);
    break;
  }
  case FTSelection::AND: {
    return printFTAnd((FTAnd*)selection, context, indent);
    break;
  }
  case FTSelection::MILD_NOT: {
    return printFTMildnot((FTMildnot*)selection, context, indent);
    break;
  }
  case FTSelection::UNARY_NOT: {
    return printFTUnaryNot((FTUnaryNot*)selection, context, indent);
    break;
  }
  case FTSelection::WORDS: {
    return printFTWords((FTWords*)selection, context, indent);
    break;
  }
  case FTSelection::WORD: {
    return printFTWord((FTWord*)selection, context, indent);
    break;
  }
  case FTSelection::ORDER: {
    return printFTOrder((FTOrder*)selection, context, indent);
    break;
  }
  case FTSelection::DISTANCE: {
    return printFTDistance((FTDistance*)selection, context, indent);
    break;
  }
  case FTSelection::DISTANCE_LITERAL: {
    return printFTDistanceLiteral((FTDistanceLiteral*)selection, context, indent);
    break;
  }
  case FTSelection::SCOPE: {
    return printFTScope((FTScope*)selection, context, indent);
    break;
  }
  case FTSelection::CONTENT: {
    return printFTContent((FTContent*)selection, context, indent);
    break;
  }
  case FTSelection::WINDOW: {
    return printFTWindow((FTWindow*)selection, context, indent);
    break;
  }
  case FTSelection::WINDOW_LITERAL: {
    return printFTWindowLiteral((FTWindowLiteral*)selection, context, indent);
    break;
  }
  default:
    break;
  }
  return getIndent(indent) + "<Unknown/>\n";
}

string PrintAST::printFTWords(const FTWords *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTWords option=\"";
  switch(selection->getOption()) {
  case FTWords::ANY: s << "any"; break;
  case FTWords::ANY_WORD: s << "any word"; break;
  case FTWords::ALL: s << "all"; break;
  case FTWords::ALL_WORDS: s << "all words"; break;
  case FTWords::PHRASE: s << "phrase"; break;
  }
  s << "\">" << endl;
  s << printASTNode(selection->getExpr(), context, indent + INDENT);
  s << in << "</FTWords>" << endl;

  return s.str();
}

string PrintAST::printFTWord(const FTWord *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTWord>" << UTF8(selection->getQueryString()) << "</FTWord>" << endl;

  return s.str();
}

string PrintAST::printFTOr(const FTOr *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTOr>" << endl;
  for(VectorOfFTSelections::const_iterator i = selection->getArguments().begin();
      i != selection->getArguments().end(); ++i) {
    s << printFTSelection(*i, context, indent + INDENT);
  }
  s << in << "</FTOr>" << endl;

  return s.str();
}

string PrintAST::printFTAnd(const FTAnd *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTAnd>" << endl;
  for(VectorOfFTSelections::const_iterator i = selection->getArguments().begin();
      i != selection->getArguments().end(); ++i) {
    s << printFTSelection(*i, context, indent + INDENT);
  }
  s << in << "</FTAnd>" << endl;

  return s.str();
}

string PrintAST::printFTMildnot(const FTMildnot *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTMildnot>" << endl;
  s << printFTSelection(selection->getLeft(), context, indent + INDENT);
  s << printFTSelection(selection->getRight(), context, indent + INDENT);
  s << in << "</FTMildnot>" << endl;

  return s.str();
}

string PrintAST::printFTUnaryNot(const FTUnaryNot *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTUnaryNot>" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTUnaryNot>" << endl;

  return s.str();
}

string PrintAST::printFTOrder(const FTOrder *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTOrder>" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTOrder>" << endl;

  return s.str();
}

string PrintAST::printFTDistance(const FTDistance *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTDistance units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << printFTRange(selection->getRange(), context, indent + INDENT);
  s << in << "</FTDistance>" << endl;

  return s.str();
}

string PrintAST::printFTDistanceLiteral(const FTDistanceLiteral *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTDistanceLiteral type=\"" << getFTRangeTypeName(selection->getType())
    << "\" distance=\"" << selection->getDistance();
  if(selection->getType() == FTRange::FROM_TO) {
    s << "\" distance2=\"" << selection->getDistance2();
  }
  s << "\" units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTDistanceLiteral>" << endl;

  return s.str();
}

string PrintAST::printFTScope(const FTScope *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTScope type=\"";
  switch(selection->getType()) {
  case FTScope::SAME: s << "same"; break;
  case FTScope::DIFFERENT: s << "different"; break;
  }
  s << "\" units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTScope>" << endl;

  return s.str();
}

string PrintAST::printFTContent(const FTContent *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTContent type=\"";
  switch(selection->getType()) {
  case FTContent::AT_START: s << "at start"; break;
  case FTContent::AT_END: s << "at end"; break;
  case FTContent::ENTIRE_CONTENT: s << "entire content"; break;
  }
  s << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTContent>" << endl;

  return s.str();
}

string PrintAST::printFTWindow(const FTWindow *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTWindow units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << printASTNode(selection->getExpr(), context, indent + INDENT);
  s << in << "</FTWindow>" << endl;

  return s.str();
}

string PrintAST::printFTWindowLiteral(const FTWindowLiteral *selection, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTWindowLiteral distance=\"" << selection->getDistance();
  s << "\" units=\"" << getFTUnitName(selection->getUnit()) << "\">" << endl;
  s << printFTSelection(selection->getArgument(), context, indent + INDENT);
  s << in << "</FTWindowLiteral>" << endl;

  return s.str();
}

string PrintAST::printFTRange(const FTRange &range, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FTRange type=\"" << getFTRangeTypeName(range.type) << "\">" << endl;
  s << printASTNode(range.arg1, context, indent + INDENT);
  if(range.arg2 != 0)
    s << printASTNode(range.arg2, context, indent + INDENT);
  s << in << "</FTRange>" << endl;

  return s.str();
}

string PrintAST::getFTRangeTypeName(FTRange::Type type)
{
  switch(type) {
  case FTRange::EXACTLY: return "exactly";
  case FTRange::AT_LEAST: return "at least";
  case FTRange::AT_MOST: return "at most";
  case FTRange::FROM_TO: return "from to";
  }
  return "unknown";
}

string PrintAST::getFTUnitName(FTOption::FTUnit unit)
{
  switch(unit) {
  case FTOption::WORDS: return "words";
  case FTOption::SENTENCES: return "sentences";
  case FTOption::PARAGRAPHS: return "paragraphs";
  }
  return "unknown";
}

string PrintAST::printAtomize(const XQAtomize *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Atomize>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</Atomize>" << endl;

  return s.str();
}

string PrintAST::printEffectiveBooleanValue(const XQEffectiveBooleanValue *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<EffectiveBooleanValue>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</EffectiveBooleanValue>" << endl;

  return s.str();
}

string PrintAST::printMap(const XQMap *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Map";
  if(item->getName() != 0) {
    s << " uri=\"" << UTF8(item->getURI()) << "\"";
    s << " name=\"" << UTF8(item->getName()) << "\"";
  }
  s << ">" << endl;
  s << printASTNode(item->getArg1(), context, indent + INDENT);
  s << printASTNode(item->getArg2(), context, indent + INDENT);
  s << in << "</Map>" << endl;

  return s.str();
}

string PrintAST::printDocumentOrder(const XQDocumentOrder *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string name(item->getUnordered() ? "UniqueNodes" : "DocumentOrder");

  s << in << "<" << name << ">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</" << name << ">" << endl;

  return s.str();
}

string PrintAST::printXPath1CompatConvertFunctionArg(const XPath1CompatConvertFunctionArg *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<XPath1CompatConvertFunctionArg>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</XPath1CompatConvertFunctionArg>" << endl;

  return s.str();
}

string PrintAST::printPromoteUntyped(const XQPromoteUntyped *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<PromoteUntyped uri=\"" << UTF8(item->getTypeURI())
    << "\" name=\"" << UTF8(item->getTypeName())
    << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</PromoteUntyped>" << endl;

  return s.str();
}

string PrintAST::printPromoteNumeric(const XQPromoteNumeric *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<PromoteNumeric uri=\"" << UTF8(item->getTypeURI())
    << "\" name=\"" << UTF8(item->getTypeName())
    << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</PromoteNumeric>" << endl;

  return s.str();
}

string PrintAST::printPromoteAnyURI(const XQPromoteAnyURI *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<PromoteAnyURI uri=\"" << UTF8(item->getTypeURI())
    << "\" name=\"" << UTF8(item->getTypeName())
    << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</PromoteAnyURI>" << endl;

  return s.str();
}

string PrintAST::printPredicate(const XQPredicate *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Predicate>" << endl; 
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printASTNode(item->getPredicate(), context, indent + INDENT);
  s << in << "</Predicate>" << endl;

  return s.str();
}

string PrintAST::printNameExpression(const XQNameExpression *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<NameExpression>" << endl; 
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</NameExpression>" << endl;

  return s.str();
}

string PrintAST::printContentSequence(const XQContentSequence *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<ContentSequence>" << endl; 
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</ContentSequence>" << endl;

  return s.str();
}

string PrintAST::printDirectName(const XQDirectName *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<DirectName qname=\"" << UTF8(item->getQName()) << "/>" << endl; 

  return s.str();
}

string PrintAST::printUDelete(const UDelete *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UDelete>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</UDelete>" << endl;

  return s.str();
}

string PrintAST::printURename(const URename *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<URename>" << endl;
  s << printASTNode(item->getTarget(), context, indent + INDENT);
  s << printASTNode(item->getName(), context, indent + INDENT);
  s << in << "</URename>" << endl;

  return s.str();
}

string PrintAST::printUReplace(const UReplace *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UReplace>" << endl;
  s << printASTNode(item->getTarget(), context, indent + INDENT);
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</UReplace>" << endl;

  return s.str();
}

string PrintAST::printUReplaceValueOf(const UReplaceValueOf *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UReplaceValueOf>" << endl;
  s << printASTNode(item->getTarget(), context, indent + INDENT);
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</UReplaceValueOf>" << endl;

  return s.str();
}

string PrintAST::printUInsertAsFirst(const UInsertAsFirst *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UInsertAsFirst>" << endl;
  s << printASTNode(item->getSource(), context, indent + INDENT);
  s << printASTNode(item->getTarget(), context, indent + INDENT);
  s << in << "</UInsertAsFirst>" << endl;

  return s.str();
}

string PrintAST::printUInsertAsLast(const UInsertAsLast *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UInsertAsLast>" << endl;
  s << printASTNode(item->getSource(), context, indent + INDENT);
  s << printASTNode(item->getTarget(), context, indent + INDENT);
  s << in << "</UInsertAsLast>" << endl;

  return s.str();
}

string PrintAST::printUInsertInto(const UInsertInto *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UInsertInto>" << endl;
  s << printASTNode(item->getSource(), context, indent + INDENT);
  s << printASTNode(item->getTarget(), context, indent + INDENT);
  s << in << "</UInsertInto>" << endl;

  return s.str();
}

string PrintAST::printUInsertAfter(const UInsertAfter *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UInsertAfter>" << endl;
  s << printASTNode(item->getSource(), context, indent + INDENT);
  s << printASTNode(item->getTarget(), context, indent + INDENT);
  s << in << "</UInsertAfter>" << endl;

  return s.str();
}

string PrintAST::printUInsertBefore(const UInsertBefore *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UInsertBefore>" << endl;
  s << printASTNode(item->getSource(), context, indent + INDENT);
  s << printASTNode(item->getTarget(), context, indent + INDENT);
  s << in << "</UInsertBefore>" << endl;

  return s.str();
}

string PrintAST::printUTransform(const UTransform *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  const VectorOfCopyBinding *bindings = item->getBindings();

  s << in << "<UTransform>" << endl;
  for(VectorOfCopyBinding::const_iterator i = bindings->begin(); i != bindings->end(); ++i) {
    s << in << "  <Copy name=\"" << UTF8((*i)->qname_);
    s << "\">" << endl;
    s << printASTNode((*i)->expr_, context, indent + INDENT + INDENT);
    s << in << "  </Copy>" << endl;
  }
  s << printASTNode(item->getModifyExpr(), context, indent + INDENT);
  s << printASTNode(item->getReturnExpr(), context, indent + INDENT);
  s << in << "</UTransform>" << endl;

  return s.str();
}

string PrintAST::printUApplyUpdates(const UApplyUpdates *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<UApplyUpdates>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</UApplyUpdates>" << endl;

  return s.str();
}

string PrintAST::printQuantified(const XQQuantified *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));
  string name(item->getQuantifierType() == XQQuantified::SOME ? "Some" : "Every");

  s << in << "<" << name << ">" << endl;
  s << printTupleNode(item->getParent(), context, indent + INDENT);
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</" << name << ">" << endl;

  return s.str();
}

string PrintAST::printReturn(const XQReturn *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Return>" << endl;
  s << printTupleNode(item->getParent(), context, indent + INDENT);
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</Return>" << endl;

  return s.str();
}

string PrintAST::printNamespaceBinding(const XQNamespaceBinding *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<NamespaceBinding>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</NamespaceBinding>" << endl;

  return s.str();
}

string PrintAST::printFunctionConversion(const XQFunctionConversion *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FunctionConversion>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printSequenceType(item->getSequenceType(), context, indent + INDENT);
  s << in << "</FunctionConversion>" << endl;

  return s.str();
}

string PrintAST::printAnalyzeString(const XQAnalyzeString *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<AnalyzeString>" << endl;
  s << in << "  <Select>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT + INDENT);
  s << in << "  </Select>" << endl;
  s << in << "  <Regex>" << endl;
  s << printASTNode(item->getRegex(), context, indent + INDENT + INDENT);
  s << in << "  </Regex>" << endl;
  if(item->getFlags() != 0) {
    s << in << "  <Flags>" << endl;
    s << printASTNode(item->getFlags(), context, indent + INDENT + INDENT);
    s << in << "  </Flags>" << endl;
  }
  s << in << "  <Match>" << endl;
  s << printASTNode(item->getMatch(), context, indent + INDENT + INDENT);
  s << in << "  </Match>" << endl;
  s << in << "  <NonMatch>" << endl;
  s << printASTNode(item->getNonMatch(), context, indent + INDENT + INDENT);
  s << in << "  </NonMatch>" << endl;
  s << in << "</AnalyzeString>" << endl;

  return s.str();
}

string PrintAST::printCopyOf(const XQCopyOf *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<CopyOf copy-namespaces=\"" << (item->getCopyNamespaces() ? "yes" : "no") << "\">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</CopyOf>" << endl;

  return s.str();
}

string PrintAST::printCopy(const XQCopy *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<Copy>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  for(VectorOfASTNodes::const_iterator i = item->getChildren().begin();
      i != item->getChildren().end(); ++i) {
    s << printASTNode(*i, context, indent + INDENT + INDENT);
  }
  s << in << "</Copy>" << endl;

  return s.str();
}

string PrintAST::printASTDebugHook(const ASTDebugHook *item, const DynamicContext *context, int indent)
{
  return printASTNode(item->getExpression(), context, indent);

//   ostringstream s;

//   string in(getIndent(indent));

//   s << in << "<ASTDebugHook";
//   s << " location=\"" << UTF8(item->getFile()) << ":" << item->getLine() << ":" << item->getColumn() << "\"";
//   s <<">" << endl;
//   s << printASTNode(item->getExpression(), context, indent + INDENT);
//   s << in << "</ASTDebugHook>" << endl;

//   return s.str();
}

string PrintAST::printCallTemplate(const XQCallTemplate *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  string name;
  if(item->getQName() != 0) {
    name += UTF8(item->getQName());
  }
  else if(item->getName() != 0) {
    name += "{";
    name += UTF8(item->getURI());
    name += "}";
    name += UTF8(item->getName());
  }

  s << in << "<CallTemplate";

  if(item->getQName() != 0) {
    s << " name=\"" << UTF8(item->getQName()) << "\"";
  }
  else if(item->getName() != 0) {
    s << " name=\"{" << UTF8(item->getURI()) << "}" << UTF8(item->getName()) << "\"";
  }

  s << ">" << endl;

  if(item->getASTName() != 0) {
    s << printASTNode(item->getASTName(), context, indent + INDENT);
  }

  if(item->getArguments() != NULL) {
    TemplateArguments::iterator argIt;
    for(argIt = item->getArguments()->begin(); argIt != item->getArguments()->end(); ++argIt) {
      s << in << "  <Argument name=\"";
      if((*argIt)->qname != 0) {
        s << UTF8((*argIt)->qname);
      }
      else {
        s << "{" << UTF8((*argIt)->uri) << "}" << UTF8((*argIt)->name);
      }
      s << "\">" << endl;
      s << printASTNode((*argIt)->value, context, indent + INDENT + INDENT);
      s << in << "  </Argument>" << endl;
    }
  }
  s << in << "</CallTemplate>" << endl;

  return s.str();
}

string PrintAST::printApplyTemplates(const XQApplyTemplates *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<ApplyTemplates>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  
  if(item->getArguments() != NULL) {
    TemplateArguments::iterator argIt;
    for(argIt = item->getArguments()->begin(); argIt != item->getArguments()->end(); ++argIt) {
      s << in << "  <Argument name=\"";
      if((*argIt)->qname != 0) {
        s << UTF8((*argIt)->qname);
      }
      else {
        s << "{" << UTF8((*argIt)->uri) << "}" << UTF8((*argIt)->name);
      }
      s << "\">" << endl;
      s << printASTNode((*argIt)->value, context, indent + INDENT + INDENT);
      s << in << "  </Argument>" << endl;
    }
  }
  s << in << "</ApplyTemplates>" << endl;

  return s.str();
}

string PrintAST::printInlineFunction(const XQInlineFunction *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<InlineFunction>" << endl;
  if(item->getUserFunction()) {
    s << printXQUserFunction(item->getUserFunction(), context, indent + INDENT);
  }
  else {
    s << printASTNode(item->getInstance(), context, indent + INDENT);
  }
  s << in << "</InlineFunction>" << endl;

  return s.str();
}

string PrintAST::printFunctionDeref(const XQFunctionDeref *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<FunctionDeref>" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);

  const VectorOfASTNodes *args = item->getArguments();
  if(args != 0 && !args->empty()) {
    s << in << "  <Arguments>" << endl;
    for(VectorOfASTNodes::const_iterator i = args->begin(); i != args->end(); ++i) {
      s << printASTNode(*i, context, indent + INDENT + INDENT);
    }
    s << in << "  </Arguments>" << endl;
  }

  s << in << "</FunctionDeref>" << endl;

  return s.str();
}

string PrintAST::printFunctionRef(const XQFunctionRef *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  string name;
  if(item->getName() != 0) {
    name += "{";
    name += UTF8(item->getURI());
    name += "}";
    name += UTF8(item->getName());
  }
  else if(item->getQName() != 0) {
    name += UTF8(item->getQName());
  }

  s << in << "<FunctionRef name=\"" << name
    << "\" numArgs=\"" << item->getNumArgs() << "\"/>" << endl;

//   s << in << "<FunctionRef name=\"" << UTF8(item->getQName())
//     << "\" numArgs=\"" << item->getNumArgs() << "\">" << endl;

//   if(item->getInstance())
// 	  s << printASTNode(item->getInstance(), context, indent + INDENT);

//   s << in << "</FunctionRef>" << endl;

  return s.str();
}

string PrintAST::print(const TupleNode *item, const DynamicContext *context, int indent)
{
  PrintAST p;
  return p.printTupleNode(item, context, indent);
}

string PrintAST::printTupleNode(const TupleNode *item, const DynamicContext *context, int indent)
{
  switch(item->getType()) {
  case TupleNode::CONTEXT_TUPLE:
    return printContextTuple((ContextTuple*)item, context, indent);
  case TupleNode::FOR:
    return printForTuple((ForTuple*)item, context, indent);
  case TupleNode::LET:
    return printLetTuple((LetTuple*)item, context, indent);
  case TupleNode::WHERE:
    return printWhereTuple((WhereTuple*)item, context, indent);
  case TupleNode::ORDER_BY:
    return printOrderByTuple((OrderByTuple*)item, context, indent);
  case TupleNode::DEBUG_HOOK:
    return printTupleDebugHook((TupleDebugHook*)item, context, indent);
  default:
    break;
  }
  return printUnknownTupleNode(item, context, indent);
}

string PrintAST::printUnknownTupleNode(const TupleNode *item, const DynamicContext *context, int indent)
{
  return getIndent(indent) + "<Unknown/>\n";
}

string PrintAST::printForTuple(const ForTuple *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<ForTuple";
  if(item->getVarName()) {
    s << " uri=\"" << UTF8(item->getVarURI())
      << "\" name=\"" << UTF8(item->getVarName())
      << "\"";
  }
  if(item->getPosName()) {
    s << " posURI=\"" << UTF8(item->getPosURI())
      << "\" posName=\"" << UTF8(item->getPosName())
      << "\"";
  }
  s << ">" << endl;
  s << printTupleNode(item->getParent(), context, indent + INDENT);
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</ForTuple>" << endl;

  return s.str();
}

string PrintAST::printLetTuple(const LetTuple *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<LetTuple";
  if(item->getVarName()) {
    s << " uri=\"" << UTF8(item->getVarURI())
      << "\" name=\"" << UTF8(item->getVarName())
      << "\"";
  }
  s << ">" << endl;
  s << printTupleNode(item->getParent(), context, indent + INDENT);
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</LetTuple>" << endl;

  return s.str();
}

string PrintAST::printWhereTuple(const WhereTuple *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<WhereTuple>" << endl;
  s << printTupleNode(item->getParent(), context, indent + INDENT);
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</WhereTuple>" << endl;

  return s.str();
}

string PrintAST::printOrderByTuple(const OrderByTuple *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<OrderByTuple";

  if(item->getModifiers() & OrderByTuple::DESCENDING)
    s << " direction=\"descending\"";
  else s << " direction=\"ascending\"";

  if(item->getModifiers() & OrderByTuple::EMPTY_LEAST)
    s << " empty=\"least\"";
  else s << " empty=\"greatest\"";

  if(item->getModifiers() & OrderByTuple::UNSTABLE)
    s << " stable=\"false\"";
  else s << " stable=\"true\"";

  s << " collation=\"" << UTF8(item->getCollation()->getCollationName()) << "\">" << endl;
  s << printTupleNode(item->getParent(), context, indent + INDENT);
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << in << "</OrderByTuple>" << endl;

  return s.str();
}

string PrintAST::printTupleDebugHook(const TupleDebugHook *item, const DynamicContext *context, int indent)
{
  return printTupleNode(item->getParent(), context, indent);

//   ostringstream s;

//   string in(getIndent(indent));

//   s << in << "<TupleDebugHook";
//   s << " location=\"" << UTF8(item->getFile()) << ":" << item->getLine() << ":" << item->getColumn() << "\"";
//   s << ">" << endl;
//   s << printTupleNode(item->getParent(), context, indent + INDENT);
//   s << in << "</TupleDebugHook>" << endl;

//   return s.str();
}

string PrintAST::printContextTuple(const ContextTuple *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<ContextTuple/>" << endl;

  return s.str();
}

string PrintAST::printSequenceType(const SequenceType *type, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<SequenceType";
  XMLBuffer buf;
  type->toBuffer(buf);
  s << " type=\"" << UTF8(buf.getRawBuffer()) << "\"";
  s << "/>" << endl;

  return s.str();
}

string PrintAST::getIndent(int indent)
{
  ostringstream s;

  for(int i = indent; i != 0; --i) {
    s << "  ";
  }

  return s.str();
}

string PrintAST::getAxisName(XQStep::Axis axis)
{
  switch(axis) {
  case XQStep::ANCESTOR: {
    return "ancestor";
  }
  case XQStep::ANCESTOR_OR_SELF: {
    return "ancestor-or-self";
  }
  case XQStep::ATTRIBUTE: {
    return "attribute";
  }
  case XQStep::CHILD: {
    return "child";
  }
  case XQStep::DESCENDANT: {
    return "descendant";
  }
  case XQStep::DESCENDANT_OR_SELF: {
    return "descendant-or-self";
  }
  case XQStep::FOLLOWING: {
    return "following";
  }
  case XQStep::FOLLOWING_SIBLING: {
    return "following-sibling";
  }
  case XQStep::NAMESPACE: {
    return "namespace";
  }
  case XQStep::PARENT: {
    return "parent";
  }
  case XQStep::PRECEDING: {
    return "preceding";
  }
  case XQStep::PRECEDING_SIBLING: {
    return "preceding-sibling";
  }
  case XQStep::SELF: {
    return "self";
  }
  default: {
    return "unknown";
  }
  }
}

string PrintAST::getOccurrenceIndicatorName(SequenceType::OccurrenceIndicator oi)
{
  switch(oi) {
  case SequenceType::EXACTLY_ONE: {
    return "exactly_one";
  }
  case SequenceType::STAR: {
    return "zero_to_many";
  }
  case SequenceType::PLUS: {
    return "one_to_many";
  }
  case SequenceType::QUESTION_MARK: {
    return "zero_or_one";
  }
  default: {
    return "unknown";
  }
  }
}

string PrintAST::getComparisonOperationName(GeneralComp::ComparisonOperation co)
{
  switch(co) {
  case GeneralComp::EQUAL: {
    return "equal";
  }
  case GeneralComp::NOT_EQUAL: {
    return "not_equal";
  }
  case GeneralComp::LESS_THAN: {
    return "less_than";
  }
  case GeneralComp::LESS_THAN_EQUAL: {
    return "less_than_equal";
  }
  case GeneralComp::GREATER_THAN: {
    return "greater_than";
  }
  case GeneralComp::GREATER_THAN_EQUAL: {
    return "greater_than_equal";
  }
  default: {
    return "unknown";
  }
  }
}
