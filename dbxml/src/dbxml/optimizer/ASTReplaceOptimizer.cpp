//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "ASTReplaceOptimizer.hpp"
#include "../dataItem/DbXmlDocAvailable.hpp"
#include "../dataItem/DbXmlNodeTest.hpp"

#include <xqilla/functions/FunctionDocAvailable.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/XMLParseException.hpp>

using namespace DbXml;
using namespace std;

ASTNode *ASTReplaceOptimizer::optimizeFunction(XQFunction *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();

	const XMLCh *uri = item->getFunctionURI();
	const XMLCh *name = item->getFunctionName();

	NodeVisitingOptimizer::optimizeFunction(item);

	// Replace fn:doc() or fn:collection() with our fn:QueryPlanFunction()
	if(uri == XQFunction::XMLChFunctionURI) {
		if(name == FunctionDocAvailable::name) {
			DbXmlDocAvailable *result = new (mm)
				DbXmlDocAvailable(minder_, item->getArguments(), mm);
			result->setLocationInfo(item);
			return result;
		}
	}

	return item;
}

ASTNode *ASTReplaceOptimizer::optimizeStep(XQStep *item)
{
	XPath2MemoryManager *mm = xpc_->getMemoryManager();
	item->setNodeTest(new (mm) DbXmlNodeTest(item->getNodeTest(), mm));
	return item;
}

ASTNode *ASTReplaceOptimizer::optimizeValidate(XQValidate *item)
{
	// We don't support "validate" expressions, so we need to check
	// for them and raise an error if we find them.
	// NB We throw an XQilla exception to get the file and line info
	XQThrow3(XMLParseException,X("ASTReplaceOptimizer::optimizeValidate"),
		X("The validation feature is not supported. You cannot use the \"validate\" keyword. [err:XQST0075]"),
		item);
}
