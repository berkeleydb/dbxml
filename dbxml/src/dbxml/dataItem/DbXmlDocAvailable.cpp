//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include <dbxml/XmlTransaction.hpp>
#include "DbXmlDocAvailable.hpp"
#include "../QueryContext.hpp"
#include "../Manager.hpp"
#include "../query/QueryPlan.hpp"
#include "../Transaction.hpp"
#include "../ReferenceMinder.hpp"
#include "../OperationContext.hpp"
#include "../query/QueryExecutionContext.hpp"
#include "DbXmlUri.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlConfiguration.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/FunctionException.hpp>

#include <xercesc/util/XMLString.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh DbXmlDocAvailable::name[] = {
	chLatin_D,
	chLatin_b,
	chLatin_X,
	chLatin_m,
	chLatin_l,
	chLatin_D,
	chLatin_o,
	chLatin_c,
	chLatin_A,
	chLatin_v,
	chLatin_a,
	chLatin_i,
	chLatin_l,
	chLatin_a,
	chLatin_b,
	chLatin_l,
	chLatin_e,
	chNull
};

DbXmlDocAvailable::DbXmlDocAvailable(ReferenceMinder &minder, const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: XQFunction(name, 1, 1, "string?", args, memMgr)
{
	staticTypingImpl(0);
}

ASTNode* DbXmlDocAvailable::staticResolution(StaticContext* context)
{
	for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
		*i = (*i)->staticResolution(context);
	}
	return this;
}

ASTNode* DbXmlDocAvailable::staticTypingImpl(StaticContext* context)
{
	for(VectorOfASTNodes::iterator i = _args.begin(); i != _args.end(); ++i) {
		_src.add((*i)->getStaticAnalysis());
	}

	_src.getStaticType() = StaticType::BOOLEAN_TYPE;
	_src.availableDocumentsUsed(true);

	return this;
}

Sequence DbXmlDocAvailable::createSequence(DynamicContext* context, int flags) const
{
	const XMLCh* currentUri = getUriArg(context);
	DbXmlUri uri(context->getBaseURI(), currentUri, /*documentUri*/true);

	if(uri.isDbXmlScheme()) {
		if(uri.getDocumentName() != "") {
			try {
				DbXmlConfiguration *conf = GET_CONFIGURATION(context);
				OperationContext &oc = conf->getOperationContext();
	
				XmlContainer containerWrapper = uri.openContainer(conf->getManager(), oc.txn());

				// Perform a presence lookup on the built-in node-metadata-equality-string
				// index for the document name
				AutoDelete<NodeIterator> result(((Container*)containerWrapper)->
					createDocumentIterator(context, this, uri.getDocumentName().c_str(),
						uri.getDocumentName().length()));

				return Sequence(context->getItemFactory()->
					createBoolean(result->next(context), context), context->getMemoryManager());
			} catch(XmlException &) {}
		}
		return Sequence(context->getItemFactory()->createBoolean(false, context), context->getMemoryManager());
	}

	// Revert to the default behaviour
	try {
		Sequence seq = context->resolveDocument(currentUri, this);
		if(!seq.isEmpty()) {
			// Force the lazy DbXmlNodeImpl to parse the document,
			// to check that it actually exists and is valid.
			const Item *item = seq.first();
			const DbXmlNodeImpl *impl = (const DbXmlNodeImpl*)item->getInterface(DbXmlNodeImpl::gDbXml);
			DBXML_ASSERT(impl);
			impl->getNsDomNode();
			return Sequence(context->getItemFactory()->createBoolean(true, context), context->getMemoryManager());
		}
	} catch(...) {}

	return Sequence(context->getItemFactory()->createBoolean(false, context), context->getMemoryManager());
}

const XMLCh *DbXmlDocAvailable::getUriArg(DynamicContext *context) const
{
	const XMLCh *currentUri = 0;

	Item::Ptr arg = getParamNumber(1, context)->next(context);
	if(arg != NULLRCP) {
		// Get the uri argument
		currentUri = arg->asString(context);
	}

	try {
		context->getItemFactory()->
			createAnyURI(currentUri, context);
	} catch(XQException &) {
		XQThrow(FunctionException,
			X("QueryPlanFunction::getUriArg"),
			X("Invalid URI format [err:FODC0005]"));
	}

	return currentUri;
}

