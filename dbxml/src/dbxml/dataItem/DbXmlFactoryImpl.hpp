//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLFACTORYIMPL_HPP
#define	__DBXMLFACTORYIMPL_HPP

#include <xqilla/context/impl/ItemFactoryImpl.hpp>
#include <xqilla/items/Node.hpp>

#include "../IndexEntry.hpp"

namespace DbXml
{

class ContainerBase;
class NsDomNode;
class DbXmlNsDomNode;
class DbXmlAttributeNode;
	
class DbXmlFactoryImpl : public ItemFactoryImpl
{
public:
	DbXmlFactoryImpl(const DocumentCache* dc,
			 XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* memMgr);
	DbXmlFactoryImpl(DatatypeLookup *dl);
	virtual ~DbXmlFactoryImpl();

	DatatypeLookup *getDatatypeLookup() { return datatypeLookup_; }

	/* @name DbXmlNodeImpl pooling methods */

	Node::Ptr createNode(Document *doc, const DynamicContext *context) const;
	Node::Ptr createNode(const NsDomNode *node, Document *doc,
			     const DynamicContext *context) const;
	Node::Ptr createNode(const IndexEntry::Ptr &ie,
			     const ContainerBase *container,
			     const DynamicContext *context,
			     bool materialise) const;

	Node::Ptr createAttrNode(const XMLCh *prefix, const XMLCh *uri,
				 const XMLCh *localName, const XMLCh *value,
				 const XMLCh *typeName, const XMLCh *typeURI,
				 const ContainerBase *container,
				 Document *doc, const NsNid &nid,
				 u_int32_t index,
				 const DynamicContext *context) const;			     
	Node::Ptr createTextNode(short type,
				 const XMLCh *value,
				 const DynamicContext *context) const;
	Node::Ptr createPINode(const XMLCh *target,
			       const XMLCh *value,
			       const DynamicContext *context) const;
};

}

#endif
