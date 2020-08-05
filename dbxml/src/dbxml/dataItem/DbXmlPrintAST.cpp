//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include <iostream>
#include <sstream>

#include "DbXmlPrintAST.hpp"
#include "UTF8.hpp"
#include "dataItem/DbXmlDocAvailable.hpp"
#include "query/QueryPlan.hpp"
#include "query/QueryPlanToAST.hpp"
#include "Container.hpp"
#include "dataItem/DbXmlNodeTest.hpp"
#include "dataItem/DbXmlPredicate.hpp"

#define DEBUG_COST 0

#if DEBUG_COST
#include "query/QueryExecutionContext.hpp"
#include "dataItem/DbXmlConfiguration.hpp"
#endif

#include <xqilla/utils/UTF8Str.hpp>
#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/Collation.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/XPath1Compat.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/functions/XQUserFunction.hpp>

#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/context/DynamicContext.hpp>

using namespace DbXml;
using namespace std;

static const int INDENT = 1;

string DbXmlPrintAST::print(const XQillaExpression *expr, const DynamicContext *context, int indent)
{
	return "Cannot print XQillaExpression in BDB XML";
}

string DbXmlPrintAST::print(const XQQuery *query, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	if(query->getIsLibraryModule()) {
		s << in << "<Module";
	} else {
		s << in << "<XQuery";
	}

	if(query->getModuleTargetNamespace()) {
		s << " targetNamespace=\"" << XMLChToUTF8(query->getModuleTargetNamespace()).str() << "\"";
	}
	s << ">" << endl;

	const ImportedModules &modules = query->getImportedModules();
	for(ImportedModules::const_iterator it = modules.begin();
	    it != modules.end(); ++it) {
		s << print(*it, context, indent + INDENT);
	}

	DbXmlPrintAST p;

	for(UserFunctions::const_iterator i = query->getFunctions().begin();
	    i != query->getFunctions().end(); ++i) {
		XQUserFunction *f = *i;

		const XMLCh *funUri = f->getURI();
		const XMLCh *funName = f->getName();

		string name("{");
		name += XMLChToUTF8(funUri).str();
		name += "}:";
		name += XMLChToUTF8(funName).str();

		s << in << "  <FunctionDefinition name=\"" << name << "\">" << endl;
		s << p.printASTNode(f->getFunctionBody(), context, indent + INDENT + INDENT);
		s << in << "  </FunctionDefinition>" << endl;
	  }
	for(GlobalVariables::const_iterator it1 = query->getVariables().begin();
	    it1 != query->getVariables().end(); ++it1) {
		s << p.printGlobal(*it1, context, indent + INDENT);
	}

	if(query->getQueryBody() != 0)
		s << in << p.printASTNode(query->getQueryBody(), context, indent + INDENT);

	if(query->getIsLibraryModule()) {
		s << in << "</Module>" << endl;
	} else {
		s << in << "</XQuery>" << endl;
	}

	return s.str();
}

string DbXmlPrintAST::print(const ASTNode *item, const DynamicContext *context, int indent)
{
	DbXmlPrintAST p;
	return p.printASTNode(item, context, indent);
}

string DbXmlPrintAST::print(const TupleNode *item, const DynamicContext *context, int indent)
{
	DbXmlPrintAST p;
	return p.printTupleNode(item, context, indent);
}

string DbXmlPrintAST::printUnknown(const ASTNode *item, const DynamicContext *context, int indent)
{
	switch((DbXmlASTNode::whichType)item->getType()) {
	case (DbXmlASTNode::QP_TO_AST): {
		return printQueryPlanToAST((QueryPlanToAST*)item, context, indent);
		break;
	}
	case (DbXmlASTNode::NODE_CHECK): {
		return printDbXmlNodeCheck((DbXmlNodeCheck*)item, context, indent);
		break;
	}
	case (DbXmlASTNode::LAST_STEP_CHECK): {
		return printDbXmlLastStepCheck((DbXmlLastStepCheck*)item, context, indent);
		break;
	}
	case (DbXmlASTNode::DBXML_PREDICATE): {
		return printDbXmlPredicate((DbXmlPredicate*)item, context, indent);
		break;
	}
	default: break;
	}
	return getIndent(indent) + "<Unknown/>\n";
}

string DbXmlPrintAST::printFunction(const XQFunction *item, const DynamicContext *context, int indent)
{
	const XMLCh *funUri = item->getFunctionURI();
	const XMLCh *funName = item->getFunctionName();

	if(funUri == XQFunction::XMLChFunctionURI) {
		if(funName == DbXmlDocAvailable::name) {
			return printDbXmlDocAvailable((DbXmlDocAvailable*)item, context, indent);
		}
	}

	ostringstream s;

	string in(getIndent(indent));

	string name("{");
	name += XMLChToUTF8(funUri).str();
	name += "}:";
	name += XMLChToUTF8(funName).str();

	const VectorOfASTNodes &args = item->getArguments();
	if(args.empty()) {
		s << in << "<Function name=\"" << name << "\"/>" << endl;
	} else {
		s << in << "<Function name=\"" << name << "\">" << endl;
		for(VectorOfASTNodes::const_iterator i = args.begin();
		    i != args.end(); ++i) {
			s << printASTNode(*i, context, indent + INDENT);
		}
		s << in << "</Function>" << endl;
	}

	return s.str();
}

string DbXmlPrintAST::printDbXmlDocAvailable(
	const DbXmlDocAvailable *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	s << in << "<DbXmlDocAvailable>" << endl;

	const VectorOfASTNodes &args = item->getArguments();
	for(VectorOfASTNodes::const_iterator i = args.begin();
	    i != args.end(); ++i) {
		s << printASTNode(*i, context, indent + INDENT);
	}

	s << in << "</DbXmlDocAvailable>" << endl;

	return s.str();
}

string DbXmlPrintAST::printNodeTestAttrs(const NodeTest *step)
{
	ostringstream s;

	SequenceType::ItemType *type = step->getItemType();
	if(type == 0) {
		if(step->getNamespaceWildcard()) {
			s << " uri=\"*\"";
		}
		else {
			if(step->getNodePrefix() != 0) {
				s << " prefix=\"" << XMLChToUTF8(step->getNodePrefix()).str() << "\"";
			}
			if(step->getNodeUri() != 0) {
				s << " uri=\"" << XMLChToUTF8(step->getNodeUri()).str() << "\"";
			}
		}
		if(step->getNameWildcard()) {
			s << " name=\"*\"";
		}
		else if(step->getNodeName() != 0) {
			s << " name=\"" << XMLChToUTF8(step->getNodeName()).str() << "\"";
		}
		
		if(step->getTypeWildcard()) {
			s << " nodeType=\"*\"";
		}
		else if(step->isNodeTypeSet()) {
			s << " nodeType=\"" << XMLChToUTF8(step->getNodeType()).str() << "\"";
		}
	}

	return s.str();
}

string DbXmlPrintAST::getJoinTypeName(Join::Type join)
{
  switch(join) {
  case Join::ANCESTOR: {
    return "ancestor";
  }
  case Join::ANCESTOR_OR_SELF: {
    return "ancestor-or-self";
  }
  case Join::ATTRIBUTE: {
    return "attribute";
  }
  case Join::CHILD: {
    return "child";
  }
  case Join::DESCENDANT: {
    return "descendant";
  }
  case Join::DESCENDANT_OR_SELF: {
    return "descendant-or-self";
  }
  case Join::FOLLOWING: {
    return "following";
  }
  case Join::FOLLOWING_SIBLING: {
    return "following-sibling";
  }
  case Join::NAMESPACE: {
    return "namespace";
  }
  case Join::PARENT: {
    return "parent";
  }
  case Join::PRECEDING: {
    return "preceding";
  }
  case Join::PRECEDING_SIBLING: {
    return "preceding-sibling";
  }
  case Join::SELF: {
    return "self";
  }
  case Join::PARENT_A: {
	  return "parent-of-attribute";
  }
  case Join::PARENT_C: {
	  return "parent-of-child";
  }
  case Join::ATTRIBUTE_OR_CHILD: {
	  return "attribute-or-child";
  }
  default: {
    return "unknown";
  }
  }
}

string DbXmlPrintAST::printQueryPlanToAST(const QueryPlanToAST *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	s << in << "<QueryPlanToAST>" << endl;

#if DEBUG_COST
	QueryExecutionContext *qec = GET_CONFIGURATION(context)->getQueryExecutionContext();
	qec->setDynamicContext(const_cast<DynamicContext*>(context));
	OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();

	item->getQueryPlan()->cost(oc, *qec);
#endif

	s << item->getQueryPlan()->printQueryPlan(context, indent + INDENT);
	s << in << "</QueryPlanToAST>" << endl;

	return s.str();
}

string DbXmlPrintAST::printDbXmlNodeCheck(const DbXmlNodeCheck *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	s << in << "<DbXmlNodeCheck>" << endl;
	s << printASTNode(item->getArg(), context, indent + INDENT);
	s << in << "</DbXmlNodeCheck>" << endl;

	return s.str();
}

string DbXmlPrintAST::printDbXmlLastStepCheck(const DbXmlLastStepCheck *item, const DynamicContext *context, int indent)
{
	ostringstream s;

	string in(getIndent(indent));

	s << in << "<DbXmlLastStepCheck>" << endl;
	s << printASTNode(item->getArg(), context, indent + INDENT);
	s << in << "</DbXmlLastStepCheck>" << endl;

	return s.str();
}

string DbXmlPrintAST::printDbXmlPredicate(const DbXmlPredicate *item, const DynamicContext *context, int indent)
{
  ostringstream s;

  string in(getIndent(indent));

  s << in << "<DbXmlPredicate"; 
  if(item->getName() != 0) {
	  s << " uri=\"" << XMLChToUTF8(item->getURI()).str() << "\"";
	  s << " name=\"" << XMLChToUTF8(item->getName()).str() << "\"";
  }
  s << ">" << endl;
  s << printASTNode(item->getExpression(), context, indent + INDENT);
  s << printASTNode(item->getPredicate(), context, indent + INDENT);
  s << in << "</DbXmlPredicate>" << endl;

  return s.str();
}

