//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlNodeImpl.hpp"

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

DbXmlFactoryImpl::DbXmlFactoryImpl(const DocumentCache* dc, MemoryManager* memMgr)
	: ItemFactoryImpl(dc, memMgr)
{
}

DbXmlFactoryImpl::DbXmlFactoryImpl(DatatypeLookup *dl)
	: ItemFactoryImpl(dl)
{
}

DbXmlFactoryImpl::~DbXmlFactoryImpl()
{}

Node::Ptr DbXmlFactoryImpl::createNode(Document *doc, const DynamicContext *context) const
{

	return new DbXmlNsDomNode(doc, context);
}

Node::Ptr DbXmlFactoryImpl::createNode(const NsDomNode *node,
				       Document *doc,
				       const DynamicContext *context) const
{
	return new DbXmlNsDomNode(node, doc, context);
}

Node::Ptr DbXmlFactoryImpl::createNode(const IndexEntry::Ptr &ie,
				       const ContainerBase *container,
				       const DynamicContext *context,
				       bool materialise) const
{
	DbXmlNsDomNode *node = new DbXmlNsDomNode(ie, container, context);
	if (materialise)
		(void)node->isUpdateAble(); // will materialise
	return node;
}

Node::Ptr DbXmlFactoryImpl::createAttrNode(
	const XMLCh *prefix, const XMLCh *uri,
	const XMLCh *localName, const XMLCh *value,
	const XMLCh *typeName, const XMLCh *typeURI,
	const ContainerBase *container,
	Document *doc, const NsNid &nid, u_int32_t index,
	const DynamicContext *context) const
{
	return new DbXmlAttributeNode(prefix, uri, localName,
				      value, typeName, typeURI,
				      container, doc,
				      nid, index, context);
}


Node::Ptr DbXmlFactoryImpl::createTextNode(
	short type,
	const XMLCh *value,
	const DynamicContext *context) const
{
	return new DbXmlTextNode(type, value, context);
}

Node::Ptr DbXmlFactoryImpl::createPINode(
	const XMLCh *target,
	const XMLCh *value,
	const DynamicContext *context) const
{
	return new DbXmlTextNode(target, value, context);
}
