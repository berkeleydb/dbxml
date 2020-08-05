//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include <sstream>

#include "NodeHandleFunction.hpp"
#include "DbXmlNodeImpl.hpp"
#include "DbXmlUri.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlConfiguration.hpp"
#include "../QueryContext.hpp"
#include "../UTF8.hpp"
#include "../OperationContext.hpp"
#include "../ReferenceMinder.hpp"
#include "../Container.hpp"

#include <xqilla/exceptions/FunctionException.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/context/DynamicContext.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

const XMLCh NodeToHandleFunction::name[] = {
	chLatin_n,
	chLatin_o,
	chLatin_d,
	chLatin_e,
	chDash,
	chLatin_t,
	chLatin_o,
	chDash,
	chLatin_h,
	chLatin_a,
	chLatin_n,
	chLatin_d,
	chLatin_l,
	chLatin_e,
	chNull
};

const unsigned int NodeToHandleFunction::minArgs = 0;
const unsigned int NodeToHandleFunction::maxArgs = 1;

NodeToHandleFunction::NodeToHandleFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "node()", args, memMgr)
{
	_fURI = XMLChFunctionURI;
}

ASTNode* NodeToHandleFunction::staticResolution(StaticContext* context)
{
	return resolveArguments(context);
}

ASTNode* NodeToHandleFunction::staticTypingImpl(StaticContext* context)
{
	if(_args.size() == 0) {
		_src.contextItemUsed(true);
	}
	_src.getStaticType() = StaticType::STRING_TYPE;
	return calculateSRCForArguments(context);
}

Result NodeToHandleFunction::createResult(DynamicContext* context, int flags) const
{
	return new NodeToHandleResult(this);
}

NodeToHandleFunction::NodeToHandleResult::NodeToHandleResult(const NodeToHandleFunction *func)
	: SingleResult(func),
	  func_(func)
{
}

Item::Ptr NodeToHandleFunction::NodeToHandleResult::getSingleResult(DynamicContext *context) const
{
	// Get the node argument
	Item::Ptr node = 0;
	if(func_->getNumArgs() == 0) {
		// node argument is the context item
		node = context->getContextItem();
		if(node == NULLRCP || !node->isNode()) {
			XQThrow(FunctionException,X("NodeToHandleFunction::NodeToHandleResult::getSingleResult"),
				 X("The context item is not a node in function dbxml:node-handle [err:FODC0001]"));
		}
	} else {
		// node argument is given
		node = func_->getParamNumber(1, context)->next(context);
	}

	const DbXmlNodeImpl *nodeImpl = (const DbXmlNodeImpl*)node->getInterface(DbXmlNodeImpl::gDbXml);
	DBXML_ASSERT(nodeImpl != 0);
	
	IndexEntry ie;
	// this method will only work on nodes from a real container
	if (nodeImpl->getContainerID() == 0) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Node handles are only available for nodes from a container");
	}
	nodeImpl->getNodeHandle(ie);
	string handle = ie.getNodeHandle();

	return context->getItemFactory()->createString(UTF8ToXMLCh(handle).str(), context);
}

const XMLCh HandleToNodeFunction::name[] = {
	chLatin_h,
	chLatin_a,
	chLatin_n,
	chLatin_d,
	chLatin_l,
	chLatin_e,
	chDash,
	chLatin_t,
	chLatin_o,
	chDash,
	chLatin_n,
	chLatin_o,
	chLatin_d,
	chLatin_e,
	chNull
};

const unsigned int HandleToNodeFunction::minArgs = 2;
const unsigned int HandleToNodeFunction::maxArgs = 2;

HandleToNodeFunction::HandleToNodeFunction(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr)
	: DbXmlFunction(name, minArgs, maxArgs, "string, string", args, memMgr),
	  container_(0)
{
	_fURI = XMLChFunctionURI;
}

ASTNode* HandleToNodeFunction::staticResolution(StaticContext* context)
{
	return resolveArguments(context);
}

ASTNode* HandleToNodeFunction::staticTypingImpl(StaticContext* context)
{
	_src.availableCollectionsUsed(true);
	_src.getStaticType() = StaticType::NODE_TYPE;
	_src.setProperties(StaticAnalysis::DOCORDER | StaticAnalysis::GROUPED |
		StaticAnalysis::SUBTREE | StaticAnalysis::ONENODE);

	calculateSRCForArguments(context);

	if(context) {
		AutoDelete<DynamicContext> dContext(context->createDynamicContext());
		dContext->setMemoryManager(context->getMemoryManager());

		container_ = getContainerArg(dContext, /*lookup*/false);
	}

	return this;
}

Container *HandleToNodeFunction::getContainerArg(DynamicContext *context, bool lookup) const
{
	if(container_ != 0) return container_;

	if(!_args[0]->isConstant() && !lookup) return 0;

	DbXmlConfiguration *conf = GET_CONFIGURATION(context);
	
	Item::Ptr containerName = getParamNumber(1, context)->next(context);

	try {
		XmlContainer container = DbXmlUri::openContainer(XMLChToUTF8(containerName->asString(context)).str(),
			conf->getManager(), conf->getTransaction());

		Container *tcont = (Container*)((Container*)container);
		conf->getMinder()->addContainer(tcont);
		return tcont;
	}
	catch(XmlException &e) {
		e.setLocationInfo(this);
		throw;
	}

	// Never reached
	return 0;
}

Result HandleToNodeFunction::createResult(DynamicContext* context, int flags) const
{
	return new HandleToNodeResult(this);
}

HandleToNodeFunction::HandleToNodeResult::HandleToNodeResult(const HandleToNodeFunction *func)
	: SingleResult(func),
	  func_(func)
{
}

Item::Ptr HandleToNodeFunction::HandleToNodeResult::getSingleResult(DynamicContext *context) const
{
	try {
		Container *container = func_->getContainerArg(context, /*lookup*/true);

		Item::Ptr handleItem = func_->getParamNumber(2, context)->next(context);
		XMLChToUTF8 handle(handleItem->asString(context));

		Buffer rawBuf(0, handle.len() >> 1);
		IndexEntry::Ptr ie(new IndexEntry);
		ie->setFromNodeHandle(rawBuf, handle.str());
		// donate Buffer memory to the IndexEntry
		ie->setMemory(rawBuf.donateBuffer());
		// if wholedoc container be sure that the document and
		// node are materialized in a temporary DB for results
		// processing
		try {
			return ((DbXmlFactoryImpl*)context->getItemFactory())->
				createNode(ie, container, context,
					   container->isWholedocContainer());
		} 
		catch(XmlException &ex) {
			// surrounding catch will set location info
			throw XmlException(XmlException::INVALID_VALUE,
					   "Invalid node handle -- unable to resolve to a document or node");
		}
	}
	catch(XmlException &ex1) {
		ex1.setLocationInfo(this);
		throw;
	}
}
