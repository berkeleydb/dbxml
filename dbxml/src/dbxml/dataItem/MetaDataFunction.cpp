//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"

#include "MetaDataFunction.hpp"
#include "DbXmlNodeImpl.hpp"
#include "../QueryContext.hpp"

#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/exceptions/XPath2ErrorException.hpp>
#include <xqilla/exceptions/IllegalArgumentException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;

const XMLCh MetaDataFunction::name[] = {
	chLatin_m,
	chLatin_e,
	chLatin_t,
	chLatin_a,
	chLatin_d,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chNull
};

const unsigned int MetaDataFunction::minArgs = 1;
const unsigned int MetaDataFunction::maxArgs = 2;

MetaDataFunction::MetaDataFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "string, node()", args, memMgr)
{
	_fURI = XMLChFunctionURI;
}

ASTNode* MetaDataFunction::staticResolution(StaticContext* context)
{
	return resolveArguments(context);
}

ASTNode* MetaDataFunction::staticTypingImpl(StaticContext* context)
{
	if(_args.size() == 1) {
		_src.contextItemUsed(true);
	}
	_src.getStaticType() = StaticType(StaticType::ANY_ATOMIC_TYPE, 0, 1);
	return calculateSRCForArguments(context);
}

Result MetaDataFunction::createResult(DynamicContext* context, int flags) const
{
	return new MetaDataResult(this);
}

MetaDataFunction::MetaDataResult::MetaDataResult(const MetaDataFunction *func)
	: SingleResult(func),
	  func_(func)
{
}

Item::Ptr MetaDataFunction::MetaDataResult::getSingleResult(DynamicContext *context) const
{
	// Get the node argument
	Item::Ptr node = 0;
	if(func_->getNumArgs() == 1) {
		// node argument is the context item
		node = context->getContextItem();
		if(node == NULLRCP || !node->isNode()) {
			XQThrow(FunctionException,X("MetaDataFunction::MetaDataResult::getSingleResult"),
				 X("The context item is not a node in function dbxml:metadata [err:FODC0001]"));
		}
	} else {
		// node argument is given
		node = func_->getParamNumber(2, context)->next(context);
	}

	// Resolve the string argument as a QName
	const XMLCh *uri, *name;
	func_->getQNameArg(1, uri, name, context);

	// Lookup the metadata
	const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)node->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(nodeImpl != 0);

	return nodeImpl->getMetaData(uri, name, context);
}

