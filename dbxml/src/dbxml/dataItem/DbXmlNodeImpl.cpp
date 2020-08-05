//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include <algorithm>

#include "DbXmlNodeImpl.hpp"
#include "DbXmlNodeTest.hpp"
#include "DbXmlFactoryImpl.hpp"
#include "DbXmlConfiguration.hpp"
#include "Join.hpp"
#include "../nodeStore/NsDoc.hpp"
#include "../nodeStore/NsDocumentDatabase.hpp"
#include "../nodeStore/NsNid.hpp"
#include "../UTF8.hpp"
#include "../Value.hpp"
#include "../Document.hpp"
#include "../QueryContext.hpp"
#include "../Container.hpp"
#include "../nodeStore/NsEventReader.hpp"
#include "../nodeStore/NsDom.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/context/ItemFactory.hpp>
#include <xqilla/items/ATAnyURIOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATQNameOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/exceptions/ItemException.hpp>
#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/schema/DocumentCache.hpp>
#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/exceptions/XPath2TypeCastException.hpp>
#include <xqilla/exceptions/XQillaException.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/events/EventHandler.hpp>
#include <xqilla/events/EventSerializer.hpp>
#include <xqilla/events/NSFixupFilter.hpp>

#include <xqilla/axis/SelfAxis.hpp>

#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <cassert>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

#define MATERIALISE_NODE \
  if(!node_) { \
    if(ie_) { \
	    const_cast<DbXmlNsDomNode*>(this)->node_ = ie_->fetchNode(*getXmlDocument(), getTransaction(), conf_); \
    } \
    else { \
      const_cast<DbXmlNsDomNode*>(this)->getDocumentAsNode(); \
    } \
  }

#define MATERIALISE_DOCUMENT \
  if(document_.isNull()) { \
    DBXML_ASSERT(ie_ && container_ != 0 && conf_ != 0); \
    ie_->getDocID().fetchDocument(const_cast<ContainerBase*>(container_)->getContainer(), *conf_, \
      const_cast<DbXmlNsDomNode*>(this)->document_, conf_->getMinder()); \
  } else if (getTransaction()) ((Document&)document_).setTransaction(getTransaction())

// macro to encapsulate the fact that XQilla requires XMLCh* strings
// returned from some dm* functions to be "persistent" (i.e. pooled)
// and last the duration of the query.  Assumes there is a "context"
// parameter available.

#define POOLED(s) (context ? (const XMLCh*)context->getMemoryManager()->getPooledString((s)) : (const XMLCh*)(s))

const XMLCh DbXmlNodeImpl::gDbXml[] =
{
	chLatin_D, chLatin_b,
	chLatin_X, chLatin_m,
	chLatin_l,
	chNull
};

//
// DbXmlNodeImpl Implementation (base)
//
DbXmlNodeImpl::~DbXmlNodeImpl()
{
}

bool DbXmlNodeImpl::hasInstanceOfType(const XMLCh* typeURI,
				      const XMLCh* typeName,
				      const DynamicContext* context) const
{
	return context->isTypeOrDerivedFromType(getTypeURI(), getTypeName(),
						typeURI, typeName);
}

Sequence DbXmlNodeImpl::dmBaseURI(const DynamicContext* context) const
{
	return Sequence(context->getMemoryManager());
}

Sequence DbXmlNodeImpl::dmDocumentURI(const DynamicContext* context) const
{	
	return Sequence(context->getMemoryManager());
}

const XMLCh* DbXmlNodeImpl::dmNodeKind() const
{  
	switch(getNodeType()) {
	case nsNodeDocument:
		return document_string;
	case nsNodeElement:
		return element_string;
	case nsNodeAttr:
		return attribute_string;
	case nsNodeCDATA:
	case nsNodeText:
		return text_string;
	case nsNodePinst:
		return processing_instruction_string;
	case nsNodeComment:
		return comment_string;
	}
    
	XQThrow2(ItemException, X("DbXmlNodeImpl::dmNodeKind"), X("Unknown node type."));
}

ATQNameOrDerived::Ptr DbXmlNodeImpl::dmTypeName(
	const DynamicContext* context) const
{
	// We don't currently store type information
	switch(getNodeType()) {
	case nsNodeElement: {
		return context->getItemFactory()->createQName(
			FunctionConstructor::XMLChXPath2DatatypesURI,
			XMLUni::fgZeroLenString,
			DocumentCache::g_szUntyped, context);
		}
	case nsNodeText:
	case nsNodeCDATA:
	case nsNodeAttr: {
		return context->getItemFactory()->createQName(
			FunctionConstructor::XMLChXPath2DatatypesURI,
			XMLUni::fgZeroLenString,
			ATUntypedAtomic::fgDT_UNTYPEDATOMIC, context);
	}
	default: {
		return 0;
	}
	}
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmNilled(
	const DynamicContext* context) const
{
	// We don't currently store type information
	if(getNodeType() != nsNodeElement) {
		return 0;
	}
	return context->getItemFactory()->createBoolean(false, context);
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmIsId(
	const DynamicContext* context) const
{
	// We don't currently store type information
	return context->getItemFactory()->createBoolean(false, context);
}

ATBooleanOrDerived::Ptr DbXmlNodeImpl::dmIsIdRefs(
	const DynamicContext* context) const
{
	// We don't currently store type information
	return context->getItemFactory()->createBoolean(false, context);
}

const XMLCh* DbXmlNodeImpl::getTypeName() const
{
	// We don't currently store type information
	switch(getNodeType()) {
	case nsNodeElement: {
		return DocumentCache::g_szUntyped;
	}
	case nsNodeText:
	case nsNodeCDATA:
	case nsNodeAttr: {
		return ATUntypedAtomic::fgDT_UNTYPEDATOMIC;
	}
	default: {
		return 0;
	}
	}
}

const XMLCh* DbXmlNodeImpl::getTypeURI() const
{
	// We don't currently store type information
	switch(getNodeType()) {
	case nsNodeText:
	case nsNodeCDATA:
	case nsNodeAttr:
	case nsNodeElement: {
		return FunctionConstructor::XMLChXPath2DatatypesURI;
	}
	default: {
		return 0;
	}
	}
}

Sequence DbXmlNodeImpl::dmTypedValue(DynamicContext* context) const
{
	switch(getNodeType()) {

	case nsNodeElement:
	case nsNodeDocument:
	case nsNodeAttr:
	case nsNodeText:
	case nsNodeCDATA: {
		return Sequence(context->getItemFactory()->createUntypedAtomic(dmStringValue(context), context),
			context->getMemoryManager()); 
        }
	case nsNodeComment:
	case nsNodePinst: {
		return Sequence(context->getItemFactory()->createString(dmStringValue(context), context),
				context->getMemoryManager());
	default:
		break;
        }
		
    }
    return Sequence(context->getMemoryManager());
}

bool DbXmlNodeImpl::lessThan(const Node::Ptr &other,
			     const DynamicContext *context) const
{
	return compare(this, (const DbXmlNodeImpl*)other->
		       getInterface(DbXmlNodeImpl::gDbXml)) < 0;
}

bool DbXmlNodeImpl::equals(const Node::Ptr &other) const
{
	return compare(this, (const DbXmlNodeImpl*)other->
		       getInterface(DbXmlNodeImpl::gDbXml)) == 0;
}

bool DbXmlNodeImpl::uniqueLessThan(const Node::Ptr &other,
				   const DynamicContext *context) const
{
	return compare(this, (const DbXmlNodeImpl*)other->
		       getInterface(DbXmlNodeImpl::gDbXml)) < 0;
}

Node::Ptr DbXmlNodeImpl::root(const DynamicContext* context) const
{
	Node::Ptr result = this;
	Node::Ptr parent = dmParent(context);
	while(parent.notNull()) {
		result = parent;
		parent = result->dmParent(context);
	}
	return result;
}

const XMLCh* DbXmlNodeImpl::asString(const DynamicContext* context) const
{
	XPath2MemoryManager *mm = context->getMemoryManager();

	MemBufFormatTarget target(1023, mm);
	EventSerializer writer(&target, mm);
	NSFixupFilter nsfilter(&writer, mm);
	generateEvents(&nsfilter, context);
	nsfilter.endEvent();

	return XMLString::replicate((XMLCh*)target.getRawBuffer(), mm);
}

void DbXmlNodeImpl::getNodeHandle(IndexEntry &ie) const
{
	ie.setDocID(getDocID());

	short type = getNodeType();
	if(type != nsNodeDocument) {
		ie.setNodeID(getNodeID());

		if(type == nsNodeElement) {
			ie.setFormat(IndexEntry::NH_ELEMENT_FORMAT);
		} else if(type == nsNodeAttr) {
			ie.setFormat(IndexEntry::NH_ATTRIBUTE_FORMAT);
			ie.setIndex(getIndex());
		} else if(type == nsNodeText ||
			type == nsNodeCDATA) {
			ie.setFormat(IndexEntry::NH_TEXT_FORMAT);
			ie.setIndex(getIndex());
		} else if(type == nsNodeComment) {
			ie.setFormat(IndexEntry::NH_COMMENT_FORMAT);
			ie.setIndex(getIndex());
		} else if(type == nsNodePinst) {
			ie.setFormat(IndexEntry::NH_PI_FORMAT);
			ie.setIndex(getIndex());
		} else {
			throw XmlException(XmlException::INVALID_VALUE,
				"Node handle unavailable for node type");
		}
	} else {
		ie.setFormat(IndexEntry::NH_DOCUMENT_FORMAT);
	}
}

void DbXmlNodeImpl::checkReadOnly() const
{
	ContainerBase *base = getContainer();
	if (base)
		base->checkReadOnly();
}
	
//
// DbxmlNsDomNode Implementation
//
DbXmlNsDomNode::DbXmlNsDomNode(Document *doc, const DynamicContext *context)
	: ie_(0),
	  container_(0),
	  conf_(GET_CONFIGURATION(context)),
	  document_(doc),
	  node_(0)
{
	if (conf_)
		txn_ = conf_->getTransaction();
	DBXML_ASSERT(doc != 0);
}

// NOTE: there may no longer be callers using a null context (TBD)
DbXmlNsDomNode::DbXmlNsDomNode(const NsDomNode *node, Document *doc,
			       const DynamicContext *context)
	: ie_(0),
	  container_(0),
	  conf_(context ? GET_CONFIGURATION(context) : 0),
	  document_(doc),
	  node_(const_cast<NsDomNode*>(node))
{
	if (conf_)
		txn_ = conf_->getTransaction();
	DBXML_ASSERT(doc != 0);
}

DbXmlNsDomNode::DbXmlNsDomNode(const NsDomNode *node, Document *doc,
			       DbXmlConfiguration *conf)
	: ie_(0),
	  container_(0),
	  conf_(conf),
	  document_(doc),
	  node_(const_cast<NsDomNode*>(node))
{
	if (conf_)
		txn_ = conf_->getTransaction();
	DBXML_ASSERT(doc != 0);
}

DbXmlNsDomNode::DbXmlNsDomNode(const IndexEntry::Ptr &ie,
			       const ContainerBase *container,
			       const DynamicContext *context)
	: ie_(ie),
	  container_(container),
	  conf_(GET_CONFIGURATION(context)),
	  document_(0),
	  node_(0)
{
	if (conf_)
		txn_ = conf_->getTransaction();
	DBXML_ASSERT(container != 0);
}

DbXmlNsDomNode::~DbXmlNsDomNode()
{

}

short DbXmlNsDomNode::getNodeType() const
{
	if(!node_) {
		if(ie_) {
			if(ie_->isSpecified(IndexEntry::ATTRIBUTE_INDEX)) {
				return nsNodeAttr;
			} else if(ie_->isSpecified(IndexEntry::TEXT_INDEX)) {
				return nsNodeText;
			} else if(ie_->isSpecified(IndexEntry::COMMENT_INDEX)) {
				return nsNodeComment;
			} else if(ie_->isSpecified(IndexEntry::PI_INDEX)) {
				return nsNodePinst;
			} else if(ie_->isSpecified(IndexEntry::NODE_ID)) {
				return nsNodeElement;
			}
		}

		return nsNodeDocument;
	}
	return node_->getNsNodeType();
}

const XmlDocument *DbXmlNsDomNode::getXmlDocument() const
{
	MATERIALISE_DOCUMENT;

	// For DLS and transient documents, set the DB minder
	// if it's not already set and it exists.
	// For NLS, it'll be a no-op.
	if (conf_ && !conf_->getDbMinder().isNull())
		((Document&)document_).setDbMinderIfNull(conf_->getDbMinder());
	return &document_;
}

void DbXmlNsDomNode::getDocumentAsNode()
{
	// handle DB_NOTFOUND for no-content documents
	try {
		// Find the ImpliedSchemaNodes for the document, so we can
		// perform document projection
		ISNVector isns;
		if(conf_) conf_->getImpliedSchemaNodes(*getXmlDocument(), isns);

		node_ =	((Document&)(*getXmlDocument())).
			getContentAsNsDom(
				&isns, getTransaction());
		if (conf_ && conf_->getDbMinder().isNull() &&
		    !((Document&)document_).getDbMinder().isNull())
			conf_->setDbMinder(((Document&)document_).getDbMinder());
	} catch (XmlException &xe) {
		if (xe.getExceptionCode() == XmlException::DATABASE_ERROR &&
		    xe.getDbErrno() == DB_NOTFOUND)
			node_ =0;
		else
			throw;
	}
}

const Document *DbXmlNsDomNode::getDocument() const
{
       MATERIALISE_DOCUMENT;

	return (Document*)document_;
}

bool DbXmlNsDomNode::isUpdateAble() const
{
	MATERIALISE_NODE;
	checkReadOnly();
	if ((Document*)document_ &&
	    ((Document*)document_)->getDocDb())
		return true;
	return (getContainerID() != 0);
}

void DbXmlNsDomNode::returnedFromQuery()
{
	conf_ = 0;
}

NsDomNode* DbXmlNsDomNode::getParentNode() const
{
	MATERIALISE_NODE;

	return node_->getNsParentNode();
}

NsDomNodeRef DbXmlNsDomNode::getNsDomNode() const
{
	MATERIALISE_NODE;

	return node_;
}

Item::Ptr DbXmlNsDomNode::getMetaData(const XMLCh *uri, const XMLCh *name,
				     DynamicContext *context) const
{
	MATERIALISE_DOCUMENT;

	XmlValue value;
	if(document_.getMetaData(XMLChToUTF8(uri).str(),
				 XMLChToUTF8(name).str(), value)) {
		return Value::convertToItem(value, context, false);
	}
	return 0;
}

u_int32_t DbXmlNsDomNode::getNodeLevel() const
{
	if(!node_) {
		if(!ie_ || !ie_->isSpecified(IndexEntry::NODE_ID))
			return 0;
		if(ie_->isSpecified(IndexEntry::NODE_LEVEL))
			return ie_->getNodeLevel();
	}

	MATERIALISE_NODE;

	return node_->getNsLevel();
}

NodeInfo::Type DbXmlNsDomNode::getType() const
{
	switch(getNodeType()) {
	case nsNodeDocument:
		return NodeInfo::DOCUMENT;
	case nsNodeElement:
		return NodeInfo::ELEMENT;
	case nsNodeAttr:
		return NodeInfo::ATTRIBUTE;
	case nsNodeText:
	case nsNodeCDATA:
		return NodeInfo::TEXT;
	case nsNodePinst:
		return NodeInfo::PI;
	case nsNodeComment:
		return NodeInfo::COMMENT;
	}
	DBXML_ASSERT(false);
	return (NodeInfo::Type)-1;
}

int DbXmlNsDomNode::getContainerID() const
{
	if(document_.isNull()) return container_->getContainerID();
	return ((Document&)document_).getContainerID();
}

ContainerBase *DbXmlNsDomNode::getContainer() const
{
	if(document_.isNull()) {
		if (container_)
			return const_cast<ContainerBase*>(container_);
		return 0;
	}
	ScopedContainer sc(((Document&)document_).getManager(),
			   ((Document&)document_).getContainerID(), false);
	return sc.get();
}

DocID DbXmlNsDomNode::getDocID() const
{
	if(document_.isNull()) return ie_->getDocID();
	return ((Document&)document_).getID();
}

const NsNid DbXmlNsDomNode::getNodeID() const
{
	if (!node_) {
		if(ie_ && ie_->isSpecified(IndexEntry::NODE_ID))
			return ie_->getNodeID();
		else return *NsNid::getRootNid();
	}

	return node_->getNodeId();
}

u_int32_t DbXmlNsDomNode::getIndex() const
{
	if (!node_) {
		if(ie_ && (ie_->isSpecified(IndexEntry::ATTRIBUTE_INDEX) ||
			   ie_->isSpecified(IndexEntry::TEXT_INDEX) ||
			   ie_->isSpecified(IndexEntry::COMMENT_INDEX) ||
			   ie_->isSpecified(IndexEntry::PI_INDEX)))
			return ie_->getIndex();
		return (u_int32_t)-1;
	}

	return node_->getIndex();
}

bool DbXmlNsDomNode::isLeadingText() const
{
	if (!node_) return false;

	return node_->getIndex() != -1 &&
		node_->getIndex() < node_->getNsNode()->getNumLeadingText();
}

const NsNid DbXmlNsDomNode::getLastDescendantID() const
{
	if(ie_ && ie_->isSpecified(IndexEntry::LAST_DESCENDANT_ID))
		return ie_->getLastDescendant();

	MATERIALISE_NODE;
	if (!node_) return *NsNid::getRootNid(); // No content documents

	return node_->getLastDescendantNid();
}

const XMLCh* DbXmlNsDomNode::getLocalName() const
{
	switch(getNodeType())
	{
	case nsNodeAttr:
	case nsNodeElement:
		MATERIALISE_NODE;
		return node_->getNsLocalName();
	case nsNodePinst:
		MATERIALISE_NODE;
		return node_->getNsNodeName();
	}
	return 0;
}

const XMLCh* DbXmlNsDomNode::getUri() const
{
	switch(getNodeType())
	{
	case nsNodeAttr:
	case nsNodeElement:
		MATERIALISE_NODE;
		return node_->getNsUri();
	}
	return 0;
}

const XMLCh* DbXmlNsDomNode::getPrefix() const
{
	switch(getNodeType())
	{
	case nsNodeAttr:
	case nsNodeElement:
		MATERIALISE_NODE;
		return node_->getNsPrefix();
	}
	return 0;
}

const XMLCh* DbXmlNsDomNode::getValue() const
{
	return dmStringValue(0);
}

const XMLCh* DbXmlNsDomNode::getPITarget() const
{
	return getLocalName();
}

static inline bool emptyString(const XMLCh * const str)
{
  return str == 0 || *str == 0;
}

static inline bool emptyString(const unsigned char * const str)
{
  return str == 0 || *str == 0;
}

static void outputInheritedNamespaces(NsDomNodeRef node, EventHandler *events)
{
	// TBD: maybe use prefix ID in DoneSet to avoid allocation
	DoneSet done;
	done.insert("xml");
	
	while(node && node->getNsNodeType() == nsNodeElement) {
		NsDomElement *enode = (NsDomElement*)node.get();
		if (enode->hasUri()) {
			const char *pfx = (const char *)enode->getNsPrefix8();
			std::string spfx(pfx ? pfx : "");
			if (done.insert(spfx).second) {
				events->namespaceEvent(emptyToNull(enode->getNsPrefix()),
						       enode->getNsUri());
			}
		}
		const NsNode *nsnode = enode->getNsNode();
		NsDoc *nsdoc = enode->getNsDoc();
		for (unsigned int i = 0; i < nsnode->numAttrs(); ++i) {
			NsDomAttr attr(nsnode, nsdoc, i);
			const XMLCh *prefix = emptyString(attr.getNsPrefix()) ? 0 :
							  attr.getNsLocalName();

			std::string spfx(prefix ?
					 XMLChToUTF8(prefix).str() : "");
			if(XPath2Utils::equals(XMLUni::fgXMLNSURIName, attr.getNsUri())) {
				if(done.insert(spfx).second &&
				   !emptyString(attr.getNsNodeValue())) {
					events->namespaceEvent(prefix,
							       attr.getNsNodeValue());
				}
			} else if (attr.hasUri() && 
				   done.insert(spfx).second) {
				events->namespaceEvent(emptyToNull(attr.getNsPrefix()),
						       attr.getNsUri());
			}
		}
		node = node->getNsParentNode();
	}
}

static void nodeToEventHandler(const NsDomNodeRef &node, EventHandler *events,
	bool outputNamespaces, bool inheritedNamespaces)
{
	switch(node->getNsNodeType()) {
	case nsNodeText:
	case nsNodeCDATA:
		events->textEvent(node->getNsNodeValue());
		break;
	case nsNodeComment:
		events->commentEvent(node->getNsNodeValue());
		break;
	case nsNodePinst:
		events->piEvent(node->getNsNodeName(), node->getNsNodeValue());
		break;
	case nsNodeAttr:
		if(XPath2Utils::equals(XMLUni::fgXMLNSURIName, node->getNsUri())) {
			if(outputNamespaces && !inheritedNamespaces) {
				const XMLCh *prefix = emptyString(node->getNsPrefix()) ? 0 :
					node->getNsLocalName();
				events->namespaceEvent(prefix, emptyToNull(node->getNsNodeValue()));
			}
		} else {
			events->attributeEvent(emptyToNull(node->getNsPrefix()),
					       emptyToNull(node->getNsUri()),
					       node->getNsLocalName(), node->getNsNodeValue(),
					       SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
					       ATUntypedAtomic::fgDT_UNTYPEDATOMIC);
		}
		break;
	case nsNodeElement:
	case nsNodeDocument: {
		DBXML_ASSERT(false);
		break;
	}
	default:
		break;
	}
}

void DbXmlNsDomNode::readerToEventHandler(EventHandler *events, EventReader *reader,
	bool outputNamespaces) const
{
	bool inheritedNamespaces = outputNamespaces;

	while(reader->hasNext()) {
		switch(reader->next()) {
		case XmlEventReader::StartElement: {
			events->startElementEvent(UTF8ToXMLCh((char*)reader->getPrefix()).str(),
				UTF8ToXMLCh((char*)reader->getNamespaceURI()).str(),
				UTF8ToXMLCh((char*)reader->getLocalName()).str());

			static XMLChToUTF8 xmlns_uri(XMLUni::fgXMLNSURIName);

			for(int i = 0; i < reader->getAttributeCount(); ++i) {
				if(NsUtil::nsStringEqual((xmlbyte_t*)xmlns_uri.str(),
					   reader->getAttributeNamespaceURI(i))) {
					if(outputNamespaces && !inheritedNamespaces) {
						UTF8ToXMLCh prefix(emptyString(reader->getAttributePrefix(i)) ? 0 :
							(char*)reader->getAttributeLocalName(i));
						events->namespaceEvent(prefix.str(),
							emptyToNull(UTF8ToXMLCh((char*)reader->
									    getAttributeValue(i)).str()));
					}
				}
				else {
					events->attributeEvent(emptyToNull(UTF8ToXMLCh((char*)reader->
										   getAttributePrefix(i)).str()),
						emptyToNull(UTF8ToXMLCh((char*)reader->getAttributeNamespaceURI(i)).str()),
						UTF8ToXMLCh((char*)reader->getAttributeLocalName(i)).str(),
						UTF8ToXMLCh((char*)reader->getAttributeValue(i)).str(),
						SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
						ATUntypedAtomic::fgDT_UNTYPEDATOMIC);
				}
			}

			if(inheritedNamespaces) {
				MATERIALISE_NODE;
				outputInheritedNamespaces(node_, events);
			}

			inheritedNamespaces = false;

			if(reader->isEmptyElement()) {
				events->endElementEvent(UTF8ToXMLCh((char*)reader->getPrefix()).str(),
					UTF8ToXMLCh((char*)reader->getNamespaceURI()).str(),
					UTF8ToXMLCh((char*)reader->getLocalName()).str(),
					SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
					DocumentCache::g_szUntyped);
			}
			break;
		}
		case XmlEventReader::EndElement:
			events->endElementEvent(UTF8ToXMLCh((char*)reader->getPrefix()).str(),
				UTF8ToXMLCh((char*)reader->getNamespaceURI()).str(),
				UTF8ToXMLCh((char*)reader->getLocalName()).str(),
				SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
				DocumentCache::g_szUntyped);
			break;
		case XmlEventReader::Characters:
		case XmlEventReader::CDATA:
		case XmlEventReader::Whitespace: {
			size_t len;
			const unsigned char *chars = reader->getValue(len);
			UTF8ToXMLCh val((char*)chars, len);
			events->textEvent(val.str());
			break;
		}
		case XmlEventReader::Comment: {
			size_t len;
			const unsigned char *chars = reader->getValue(len);
			UTF8ToXMLCh val((char*)chars, len);
			events->commentEvent(val.str());
			break;
		}
		case XmlEventReader::StartDocument: {
			const XMLCh *documentURI = 0;
			if(!document_.isNull()) {
				documentURI = ((Document*)document_)->getDocumentURI();
			}
			events->startDocumentEvent(documentURI,
				UTF8ToXMLCh((char*)reader->getEncoding()).str());
			inheritedNamespaces = false;
			break;
		}
		case XmlEventReader::EndDocument:
			events->endDocumentEvent();
			break;
		case XmlEventReader::ProcessingInstruction: {
			size_t len;
			const unsigned char *chars = reader->getValue(len);
			UTF8ToXMLCh val((char*)chars, len);
			events->piEvent(UTF8ToXMLCh((char*)reader->getLocalName()).str(),
				val.str());
			break;
		}
		case XmlEventReader::StartEntityReference:
		case XmlEventReader::EndEntityReference:
		case XmlEventReader::DTD:
			// ignore
			break;
		}
	}
}

void DbXmlNsDomNode::generateEvents(EventHandler *events,
				    const DynamicContext *context,
				    bool preserveNS, bool preserveType) const
{
	short ntype = getNodeType();
	if (ntype != nsNodeDocument && ntype != nsNodeElement) {
		MATERIALISE_NODE;
		ntype = getNodeType();
	}
	if (ntype == nsNodeDocument || ntype == nsNodeElement) {
		AutoDelete<EventReader> reader(getEventReader(context));
		readerToEventHandler(events, reader, preserveNS);
		return;
	}
	// Generate events from the non-element node
	nodeToEventHandler(node_, events, preserveNS, preserveNS);
}

EventReader *DbXmlNsDomNode::getEventReader(const DynamicContext *context) const
{
	short ntype = getNodeType();
	if (ntype != nsNodeDocument && ntype != nsNodeElement)
		return 0;
	if (!node_)
		MATERIALISE_NODE;
	DbWrapper *docdb = 0;
	DictionaryDatabase *ddb = 0;
	int cid = 0;
	if (document_) {
		docdb = ((Document&)document_).getDocDb();
		ddb = ((Document&)document_).getDictionaryDB();
		cid = ((Document&)document_).getContainerID();
	
	} else {
		ContainerBase *container = getContainer();
		if (container) {
			docdb = container->getDbWrapper();
			ddb = container->getDictionaryDatabase();
			cid = container->getContainerID();
		}
	}
	if (docdb) {
		DBXML_ASSERT(ddb);
		// Get events directly from the database
		Transaction *txn =
			(docdb->isTransacted() ?
			 GET_CONFIGURATION(context)->getTransaction() :
			 0);
		NsNid tnid(getNodeID());
		return new NsEventReader(
			txn, docdb, ddb, getDocID(), cid, /*no flags*/0, NS_EVENT_BULK_BUFSIZE, &tnid);
	}
	return 0;
}

Sequence DbXmlNsDomNode::dmBaseURI(const DynamicContext* context) const
{
	MATERIALISE_NODE;
	if (!node_) return Sequence(context->getMemoryManager()); // No content documents

	switch(node_->getNsNodeType()) {
	case nsNodeDocument: {
		const XMLCh *baseURI = context->getBaseURI();
		const XMLCh* docURI = ((Document*)document_)->getDocumentURI();
		if(docURI != 0 && *docURI != 0)
			baseURI = docURI;
		if(baseURI == 0 || *baseURI == 0)
			return Sequence(context->getMemoryManager());
		return Sequence(context->getItemFactory()->createAnyURI(baseURI, context),
				context->getMemoryManager());
	}
	case nsNodeElement: {
		const XMLCh *baseURI = context->getBaseURI();
		Node::Ptr parent = dmParent(context);
		if(parent.notNull()) {
			Sequence pb = parent->dmBaseURI(context);
			if(!pb.isEmpty())
				baseURI = pb.first()->asString(context);
		}
		const XMLCh *elemURI = node_->getNsBaseUri(baseURI);
		
		if (elemURI)
			baseURI = elemURI;
		if(baseURI == NULL || *baseURI == 0)
			return Sequence(context->getMemoryManager());
		return Sequence(context->getItemFactory()->createAnyURI(baseURI, context),
				context->getMemoryManager());
	}
	case nsNodeAttr:
	case nsNodePinst:
	case nsNodeComment:
	case nsNodeText:
	case nsNodeCDATA: {
		Node::Ptr parent = dmParent(context);
		if(parent.notNull()) {
			return parent->dmBaseURI(context);
		}
		return Sequence(context->getMemoryManager());
	}
	default:
		break;
	}
	XQThrow2(ItemException, X("DbXmlNodeImpl::dmBaseURI"), X("Unknown node type."));	
}

Sequence DbXmlNsDomNode::dmDocumentURI(const DynamicContext* context) const
{	
	if(getNodeType() != nsNodeDocument)
		return Sequence(context->getMemoryManager());

	MATERIALISE_DOCUMENT;

	const XMLCh *documentURI = ((Document*)document_)->getDocumentURI();
	if(documentURI == 0) return Sequence(context->getMemoryManager());

	return Sequence(context->getItemFactory()->createAnyURI(documentURI, context), context->getMemoryManager());
}

ATQNameOrDerived::Ptr DbXmlNsDomNode::dmNodeName(const DynamicContext* context) const
{  
	switch(getNodeType())
	{
	case nsNodeAttr:
	case nsNodeElement:
		MATERIALISE_NODE;
		return context->getItemFactory()->createQName(node_->getNsUri(),
			node_->getNsPrefix(), node_->getNsLocalName(), context);
	case nsNodePinst:
		MATERIALISE_NODE;
		return context->getItemFactory()->createQName(XMLUni::fgZeroLenString,
			XMLUni::fgZeroLenString, node_->getNsNodeName(), context);
	}
	return 0;
}

const XMLCh* DbXmlNsDomNode::dmStringValue(const DynamicContext* context) const
{
	MATERIALISE_NODE;
	if (!node_) return XMLUni::fgZeroLenString; // No content documents

	// TBD store the node value in the IndexEntry object - jpcs
	// We don't currently store type information
	
	switch(node_->getNsNodeType()) {
	case nsNodeElement:
	case nsNodeDocument: {
		// Use getNsTextContent(), as that returns the value
		// for a document node properly.  The string value
		// is stored in the NsDomElement.
		return  POOLED(((NsDomElement*)node_.get())->getNsTextContent());
	}
	case nsNodeAttr:
	case nsNodePinst:
	case nsNodeComment:
	case nsNodeText:
	case nsNodeCDATA: {
		return POOLED(node_->getNsNodeValue());
	}
	default:
		break;
	}
	return XMLUni::fgZeroLenString;
}

Result DbXmlNsDomNode::dmAttributes(const DynamicContext* context,
				    const LocationInfo *location) const
{
	if(getNodeType() == nsNodeElement) {
		return new DbXmlAttributeAxis(location, this, NULL);
	}
	return 0;
}

Result DbXmlNsDomNode::dmNamespaceNodes(const DynamicContext* context,
					const LocationInfo *location) const
{
	if(getNodeType() == nsNodeElement) {
		return new DbXmlNamespaceAxis(location, this, NULL);
	}
	return 0;
}

Result DbXmlNsDomNode::dmChildren(const DynamicContext *context,
				  const LocationInfo *location) const
{
	if(getNodeType() == nsNodeElement || getNodeType() == nsNodeDocument) {
		return new DbXmlChildAxis(location, this, NULL);
	}
	return 0;
}

Node::Ptr DbXmlNsDomNode::dmParent(const DynamicContext* context) const
{
	if (!node_) {
		if(ie_ && ie_->isSpecified(IndexEntry::NODE_ID))
			const_cast<DbXmlNsDomNode*>(this)->node_ =
				ie_->fetchNode(*getXmlDocument(), getTransaction(),
					       conf_);
		else return 0;
	}

	NsDomNode *parent = node_->getNsParentNode();
	if(parent == 0) return 0;

	return ((DbXmlFactoryImpl*)context->getItemFactory())->
		createNode(parent, document_, context);
}

Result DbXmlNsDomNode::getAxisResult(XQStep::Axis axis,
				     const NodeTest *nt,
				     const DynamicContext *context,
				     const LocationInfo *location) const
{
	DBXML_ASSERT(nt == 0 || nt->getInterface(DbXmlNodeTest::gDbXml) != 0);
	const DbXmlNodeTest *nodeTest = nt == 0 ? 0 : (const DbXmlNodeTest*)nt;

	switch((Join::Type)axis) {
	case Join::ANCESTOR: {
		MATERIALISE_NODE;
		if (!node_) return 0; // No content documents
		return new DbXmlAncestorAxis(location, this, nodeTest);
	}
	case Join::ANCESTOR_OR_SELF: {
		if(getNodeType() == nsNodeDocument) {
			if(nodeTest == 0) return new SelfAxis(location, this);
			return nodeTest->filterResult(new SelfAxis(location, this), location);
		}

		MATERIALISE_NODE;
		return new DbXmlAncestorOrSelfAxis(location, this, nodeTest);
	}
	case Join::ATTRIBUTE: {
		if(getNodeType() == nsNodeElement) {
			return new DbXmlAttributeAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::ATTRIBUTE_OR_CHILD: {
		if(getNodeType() == nsNodeDocument) {
			MATERIALISE_NODE;
			if (!node_) return 0; // No content documents

			if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementChildAxis(location, this, nodeTest);
			}
			return new DbXmlChildAxis(location, this, nodeTest);
		}
		else if(getNodeType() == nsNodeElement) {
			if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementChildAxis(location, this, nodeTest);
			}
			return new DbXmlAttributeOrChildAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::CHILD: {
		if(getNodeType() == nsNodeElement || getNodeType() == nsNodeDocument) {
			MATERIALISE_NODE;
			if (!node_) return 0; // No content documents

			if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementChildAxis(location, this, nodeTest);
			}
			return new DbXmlChildAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::DESCENDANT: {
		if(getNodeType() == nsNodeElement || getNodeType() == nsNodeDocument) {
			MATERIALISE_NODE;
			if (!node_) return 0; // No content documents

			if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
				   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
				return new ElementDescendantAxis(location, this, nodeTest);
			}
			return new DbXmlDescendantAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::DESCENDANT_OR_SELF: {
		MATERIALISE_NODE;
		if (!node_) {
			// No content documents
			if(nodeTest == 0) return new SelfAxis(location, this);
			return nodeTest->filterResult(new SelfAxis(location, this), location);
		}

		if(nodeTest != 0 && nodeTest->getItemType() == 0 && (nodeTest->getHasChildren() ||
			   (!nodeTest->getTypeWildcard() && nodeTest->getNodeType() == Node::element_string))) {
			return new ElementDescendantOrSelfAxis(location, this, nodeTest);
		}
		return new DbXmlDescendantOrSelfAxis(location, this, nodeTest);
	}
	case Join::FOLLOWING: {
		MATERIALISE_NODE;
		if (!node_) return 0; // No content documents
		return new DbXmlFollowingAxis(location, this, nodeTest);
	}
	case Join::FOLLOWING_SIBLING: {
		MATERIALISE_NODE;
		if (!node_) return 0; // No content documents
		return new DbXmlFollowingSiblingAxis(location, this, nodeTest);
	}
	case Join::NAMESPACE: {
		if(getNodeType() == nsNodeElement) {
			return new DbXmlNamespaceAxis(location, this, nodeTest);
		}
		break;
	}
	case Join::PARENT_A:
	case Join::PARENT_C:
	case Join::PARENT: {
		MATERIALISE_NODE;
		if (!node_) return 0; // No content documents

		return new DbXmlParentAxis(location, this, nodeTest);
	}
	case Join::PRECEDING: {
		MATERIALISE_NODE;
		if (!node_) return 0; // No content documents
		return new DbXmlPrecedingAxis(location, this, nodeTest);
	}
	case Join::PRECEDING_SIBLING: {
		MATERIALISE_NODE;
		if (!node_) return 0; // No content documents
		return new DbXmlPrecedingSiblingAxis(location, this, nodeTest);
	}
	case Join::SELF: {
		if(nodeTest == 0) return new SelfAxis(location, this);
		return nodeTest->filterResult(new SelfAxis(location, this), location);
	}
	case Join::NONE: break;
	}

	return 0;
}

//
// DbXmlNamespaceNode Implementation
//

DbXmlNamespaceNode::DbXmlNamespaceNode(const XMLCh *prefix, const XMLCh *uri,
				       NsDomElement *parent, Document *doc,
				       const DynamicContext *context)
	: document_(doc),
	  prefix_(prefix),
	  uri_(uri),
	  parent_(parent)
{
}

Node::Ptr DbXmlNamespaceNode::dmParent(const DynamicContext* context) const
{
	if (!parent_) return 0;

	return ((DbXmlFactoryImpl*)context->getItemFactory())->
		createNode(parent_.get(), document_, context);
}

const XMLCh* DbXmlNamespaceNode::dmStringValue(
	const DynamicContext* context) const
{
	return POOLED(uri_);
}


Sequence DbXmlNamespaceNode::dmTypedValue(
	DynamicContext* context) const
{
	return Sequence(context->getItemFactory()->createString(uri_, context),
			context->getMemoryManager());
}

ATQNameOrDerived::Ptr DbXmlNamespaceNode::dmNodeName(
	const DynamicContext* context) const
{  
	if(prefix_)
		return context->getItemFactory()->createQName(
			XMLUni::fgZeroLenString,
			XMLUni::fgZeroLenString, prefix_, context);
	return 0;
}

short DbXmlNamespaceNode::getNodeType() const {
	return nsNodeAttr;
}

//
// DbXmlAttributeNode Implementation
//
DbXmlAttributeNode::DbXmlAttributeNode(const XMLCh *prefix, const XMLCh *uri,
				       const XMLCh *localName,
				       const XMLCh *value,
				       const XMLCh *typeName, const XMLCh *typeURI,
				       const ContainerBase *container,
				       Document *doc, const NsNid &nid,
				       u_int32_t index,
				       const DynamicContext *context)
	: prefix_(prefix),
	  uri_(uri),
	  localName_(localName),
	  value_(value),
	  typeName_(typeName),
	  typeURI_(typeURI),
	  doc_(doc),
	  index_(index),
	  container_(container),
	  conf_(context ? GET_CONFIGURATION(context) : 0)
{
	DBXML_ASSERT(conf_ || doc); 

	nid_.clear();
	if (!nid.isNull())
		nid_.copyNid(nid.getBytes(), nid.getLen());
	if (doc) {
		const_cast<DocID&>(did_) = doc->getID();
	} else
		const_cast<DocID&>(did_) = conf_->getDbMinder().
			allocateDocID((Manager&)conf_->getManager());
}

DbXmlAttributeNode::DbXmlAttributeNode(const XMLCh *prefix, const XMLCh *uri,
                                       const XMLCh *localName,
                                       const XMLCh *value,
                                       const XMLCh *typeName, const XMLCh *typeURI,
                                       const ContainerBase *container,
                                       const DocID &did, const NsNid &nid,
                                       u_int32_t index)
        : prefix_(prefix),
          uri_(uri),
          localName_(localName),
          value_(value),
          typeName_(typeName),
          typeURI_(typeURI),
          doc_(0),
          index_(index),
          container_(container),
	  did_(did)
{
	nid_.clear();
	if (!nid.isNull())
		nid_.copyNid(nid.getBytes(), nid.getLen());
}

short DbXmlAttributeNode::getNodeType() const {
	return nsNodeAttr;
}

ContainerBase *DbXmlAttributeNode::getContainer() const
{
	return const_cast<ContainerBase*>(container_);
}

int DbXmlAttributeNode::getContainerID() const
{
	if(doc_.isNull()) {
		if (container_)
			return container_->getContainerID();
		return 0;
	}
	return ((Document&)doc_).getContainerID();
}

bool DbXmlAttributeNode::isUpdateAble() const
{
	checkReadOnly();
	// attributes are only updateable if they
	// exist in a real container (vs constructed)
	// TBD -- maybe think about updating standalone
	// attributes
	return (getContainerID() > 0);
}

void DbXmlAttributeNode::generateEvents(
	EventHandler *events,
	const DynamicContext *context,
	bool preserveNS,
	bool preserveType) const
{
	events->attributeEvent(emptyToNull(prefix_),
			       emptyToNull(uri_),
			       localName_, value_,
			       SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
			       ATUntypedAtomic::fgDT_UNTYPEDATOMIC);
}

ATQNameOrDerived::Ptr DbXmlAttributeNode::dmNodeName(
	const DynamicContext* context) const
{
	return context->getItemFactory()->createQName(
		uri_, prefix_, localName_, context);
}

const XMLCh* DbXmlAttributeNode::dmStringValue(const DynamicContext *context) const
{
	return POOLED(value_);
}

Sequence DbXmlAttributeNode::dmBaseURI(const DynamicContext* context) const
{
	Node::Ptr parent = dmParent(context);
	if(parent.notNull()) {
		return parent->dmBaseURI(context);
	}
	return Sequence(context->getMemoryManager());
}
	
const XMLCh* DbXmlAttributeNode::getTypeName() const
{
	if (typeName_ && *typeName_)
		return typeName_;
	else
		return ATUntypedAtomic::fgDT_UNTYPEDATOMIC;
}

const XMLCh* DbXmlAttributeNode::getTypeURI() const
{
	if (typeURI_ && *typeURI_)
		return typeURI_;
	else
		return FunctionConstructor::XMLChXPath2DatatypesURI;
}

void DbXmlAttributeNode::returnedFromQuery()
{
	conf_ = 0;
}

NsDomNode * DbXmlAttributeNode::getParentNode() const
{
	// if we have a NsNid and Document, we can get a parent node
	if (!doc_.isNull() && !nid_.isNull()) {
		// Find the ImpliedSchemaNodes for the document, so we can
		// perform document projection
		ISNVector isns;
		if(conf_) conf_->getImpliedSchemaNodes(doc_, isns);

		return ((Document*)doc_)->getElement(NsNid(&nid_), &isns);
	}
	return 0;
}

Node::Ptr DbXmlAttributeNode::dmParent(const DynamicContext* context) const
{
	NsDomNode *parent = getParentNode();
	if(parent) {
		return ((DbXmlFactoryImpl*)context->getItemFactory())->
			createNode(parent, (Document*)doc_, context);
	}
	return 0;
}

class DbXmlAttributeAncestorOrSelfAxis : public ResultImpl {
public:
	DbXmlAttributeAncestorOrSelfAxis(const LocationInfo *location,
					 const DbXmlAttributeNode *contextNode,
					 const DbXmlNodeTest *nodeTest)
		: ResultImpl(location), contextNode_(contextNode),
		nodeTest_(nodeTest), toDo_(true) {}
	Item::Ptr next(DynamicContext *context) {
		if (toDo_) {
			toDo_ = false;
			return contextNode_; 
		}
		// TBD -- do ancestor...
		return 0;
	}
	std::string asString(DynamicContext *context, int indent) const {
		return "DbXmlAttributeAncestorOrSelf";
	}
private:
	DbXmlAttributeNode::Ptr contextNode_;
	const DbXmlNodeTest *nodeTest_;
	bool toDo_;
};

class DbXmlAttributeIteratorAxis : public ResultImpl {
public:
	DbXmlAttributeIteratorAxis(const LocationInfo *location,
				   const DbXmlAttributeNode *contextNode,
				   const DbXmlNodeTest *nodeTest,
				   bool forward)
		: ResultImpl(location), contextNode_(contextNode),
		nodeTest_(nodeTest), toDo_(true), forward_(forward) {}
	Item::Ptr next(DynamicContext *context) {
		// TBD
		if (toDo_) {
			toDo_ = false;
		}
		return 0;
	}
	std::string asString(DynamicContext *context, int indent) const {
		if (forward_)
			return "DbXmlAttributeFollowingAxis";
		return "DbXmlAttributePrecedingAxis";
	}
private:
	DbXmlAttributeNode::Ptr contextNode_;
	const DbXmlNodeTest *nodeTest_;
	bool toDo_;
	bool forward_;
};


Result DbXmlAttributeNode::getAxisResult(
	XQStep::Axis axis,
	const NodeTest *nt,
	const DynamicContext *context,
	const LocationInfo *location) const
{
	DBXML_ASSERT(nt == 0 || nt->getInterface(DbXmlNodeTest::gDbXml) != 0);
	const DbXmlNodeTest *nodeTest = nt == 0 ? 0 : (const DbXmlNodeTest*)nt;
	switch((Join::Type)axis) {

	case Join::PARENT_A:
	case Join::PARENT: {
		Node::Ptr node = dmParent(context);
		if (!node.isNull()) {
			if(nodeTest == 0) return new SelfAxis(location, node);
			return nodeTest->filterResult(new SelfAxis(location, node), location);
		}
		break;
	}
	case Join::FOLLOWING:
		return new DbXmlAttributeIteratorAxis(location,
						      this, nodeTest,
						      true);
		break;
	case Join::PRECEDING:
		return new DbXmlAttributeIteratorAxis(location,
						      this, nodeTest,
						      false);
		break;
	case Join::DESCENDANT_OR_SELF:
	case Join::SELF:
		if(nodeTest == 0) return new SelfAxis(location, this);
		return nodeTest->filterResult(new SelfAxis(location, this), location);
		break;
	case Join::ANCESTOR_OR_SELF:
		return new DbXmlAttributeAncestorOrSelfAxis(location,
							    this, nodeTest);
		break;
	case Join::ANCESTOR: {
		Node::Ptr node = dmParent(context);
		if (!node.isNull()) {
			return new DbXmlAncestorOrSelfAxis(
				location,
				(const DbXmlNodeImpl*)node->
				getInterface(DbXmlNodeImpl::gDbXml),
				nodeTest);
		}
		break;
	}
	default:
		break;
	}
	return 0;
}

//
// DbXmlTextNode Implementation
//
DbXmlTextNode::DbXmlTextNode(short type,
			     const XMLCh *value,
			     const DynamicContext *context)
	: type_(type),
	  value_(value),
	  target_(0),
	  did_(GET_CONFIGURATION(context)->getDbMinder().
	       allocateDocID((Manager&)GET_CONFIGURATION(context)->
			     getManager()))
{
	DBXML_ASSERT((type_ == nsNodeText) ||
		     (type_ == nsNodeComment) ||
		     (type_ == nsNodeCDATA));
}

DbXmlTextNode::DbXmlTextNode(const XMLCh *target,
			     const XMLCh *value,
			     const DynamicContext *context)
	: type_(nsNodePinst),
	  value_(value),
	  target_(target),
	  did_(GET_CONFIGURATION(context)->getDbMinder().
	       allocateDocID((Manager&)GET_CONFIGURATION(context)->
			     getManager()))
{
}

DbXmlTextNode::DbXmlTextNode(short type,
			     const XMLCh *value,
			     const DocID &did)
	: type_(type),
	  value_(value),
	  target_(0),
	  did_(did)
{
	DBXML_ASSERT((type_ == nsNodeText) ||
		     (type_ == nsNodeComment) ||
		     (type_ == nsNodeCDATA));
}

DbXmlTextNode::DbXmlTextNode(const XMLCh *target,
			     const XMLCh *value,
			     const DocID &did)
	: type_(nsNodePinst),
	  value_(value),
	  target_(target),
	  did_(did)
{
}

NodeInfo::Type DbXmlTextNode::getType() const
{
	if (type_ == nsNodeText)
		return NodeInfo::TEXT;
	if (type_ == nsNodeComment)
		return NodeInfo::COMMENT;
	if (type_ == nsNodePinst)
		return NodeInfo::PI;
	DBXML_ASSERT(false);
	return (NodeInfo::Type) -1;
}

void DbXmlTextNode::generateEvents(
	EventHandler *events,
	const DynamicContext *context,
	bool preserveNS,
	bool preserveType) const
{
	if ((type_ == nsNodeText) ||
	    (type_ == nsNodeCDATA))
		events->textEvent(value_);
	else if (type_ == nsNodeComment)
		events->commentEvent(value_);
	else if (type_ == nsNodePinst)
		events->piEvent(target_, value_);
}

Result DbXmlTextNode::getAxisResult(XQStep::Axis axis,
	const NodeTest *nodeTest,
	const DynamicContext *context,
	const LocationInfo *location) const
{
	switch((Join::Type)axis) {
	case Join::SELF:
	case Join::ANCESTOR_OR_SELF:
	case Join::DESCENDANT_OR_SELF:
		if(nodeTest == 0) return new SelfAxis(location, this);
		else return nodeTest->filterResult(
			new SelfAxis(location, this),
			location);
	default:
		break;
	}

	return 0;
}

ATQNameOrDerived::Ptr DbXmlTextNode::dmNodeName(
	const DynamicContext* context) const
{
	if (type_ == nsNodePinst)
		return context->getItemFactory()->createQName(
			XMLUni::fgZeroLenString,
			XMLUni::fgZeroLenString, target_, context);
	return 0;
}

const XMLCh* DbXmlTextNode::dmStringValue(const DynamicContext *context) const
{
	return POOLED(value_);
}

//
// DbXmlAxis Implementation
//

DbXmlAxis::DbXmlAxis(const LocationInfo *location,
		     const DbXmlNodeImpl *contextNode,
		     const DbXmlNodeTest *nodeTest)
	: ResultImpl(location),
	  nodeObj_(contextNode),
	  contextNode_(contextNode->getNsDomNode()),
	  nodeTest_(nodeTest),
	  toDo_(true)
{
	DBXML_ASSERT(contextNode_ || (contextNode->getNodeType() == nsNodeAttr));
}

inline bool equals(const xmlbyte_t *const str1, const xmlbyte_t *const str2) {
	if(str1 == str2) return true;

	if(str1 == 0) return *str2 == 0; // str2 == 0 is handled by the first line
	if(str2 == 0) return *str1 == 0; // str1 == 0 is handled by the first line

	register const xmlbyte_t* psz1 = str1;
	register const xmlbyte_t* psz2 = str2;

	while(*psz1 == *psz2) {
		// If either has ended, then they both ended, so equal
		if(*psz1 == 0)
			return true;
                                                                                                                                                              
		// Move upwards for the next round
		++psz1;
		++psz2;
	}
	return false;
}

Item::Ptr DbXmlAxis::next(DynamicContext *context)
{
	DbXmlFactoryImpl *factory = (DbXmlFactoryImpl*)context->getItemFactory();

	NsDomNodeRef node;
	while (true) {
		node = nextNode(context);
		if (!node)
			break;
		context->testInterrupt();

		// Check for ignorable whitespace
		if(node->isTextType()) {
			NsDomText *text = (NsDomText*)node.get();
			if(nsTextType(text->getNsTextType()) == NS_TEXT ||
				nsTextType(text->getNsTextType()) == NS_CDATA) {
				if(((NsDomText*)node.get())->
				   nsIgnorableWhitespace()) {
					continue;
				}
			}
		}

		const XmlDocument &doc = *(nodeObj_->getXmlDocument());
		if(nodeTest_ == 0) return factory->createNode(node.get(),
							      doc, context);

		SequenceType::ItemType *itemType = nodeTest_->getItemType();
		if(itemType != 0) {
			Node::Ptr result = factory->createNode(node.get(),
							       doc, context);
			if(itemType->matches(result, context)) {
				return result;
			}
		} else {
			switch(node->getNsNodeType())
			{      
			case nsNodeDocument: {
				if(!nodeTest_->getTypeWildcard() &&
				   nodeTest_->getNodeType() !=
				   Node::document_string) continue;
				if(!nodeTest_->getNameWildcard() ||
				   !nodeTest_->getNamespaceWildcard()) continue;
				break;
			}
			case nsNodeElement: {
				if(!nodeTest_->getTypeWildcard() &&
				   nodeTest_->getNodeType() !=
				   Node::element_string) continue;
				if(!nodeTest_->getNameWildcard()) {
					if(!equals(node->getNsLocalName8(),
						   nodeTest_->getNodeName8())) continue;
				}
				if(!nodeTest_->getNamespaceWildcard())
				{
					if(!equals(node->getNsUri8(),
						   nodeTest_->getNodeUri8())) continue;
				}
				break;
			}
			case nsNodeAttr: {
				if(nodeTest_->getTypeWildcard()) {
					if(nodeTest_->getHasChildren()) continue;
				} else if(nodeTest_->getNodeType() !=
					  Node::attribute_string)
					continue;
				if(!nodeTest_->getNameWildcard()) {
					if(!equals(node->getNsLocalName8(),
						   nodeTest_->getNodeName8())) continue;
				}
				if(!nodeTest_->getNamespaceWildcard()) {
					if(!equals(node->getNsUri8(),
						   nodeTest_->getNodeUri8())) continue;
				}
				break;
			}
			case nsNodeCDATA:
			case nsNodeText: {
				if(nodeTest_->getTypeWildcard()) {
					if(nodeTest_->getHasChildren())
						continue;
				} else if(nodeTest_->getNodeType() !=
					  Node::text_string) continue;
				if(!nodeTest_->getNameWildcard() ||
				   !nodeTest_->getNamespaceWildcard()) continue;
				break;
			}
			case nsNodePinst: {
				if(nodeTest_->getTypeWildcard()) {
					if(nodeTest_->getHasChildren())
						continue;
				} else if(nodeTest_->getNodeType() !=
					  Node::processing_instruction_string) continue;
				if(!nodeTest_->getNameWildcard()) {
					if(!equals(node->getNsNodeName8(),
						   nodeTest_->getNodeName8())) continue;
				}
				if(!nodeTest_->getNamespaceWildcard()) continue;
				break;
			}
			case nsNodeComment: {
				if(nodeTest_->getTypeWildcard()) {
					if(nodeTest_->getHasChildren())
						continue;
				} else if(nodeTest_->getNodeType() !=
					  Node::comment_string) continue;
				if(!nodeTest_->getNameWildcard() ||
				   !nodeTest_->getNamespaceWildcard()) continue;
				break;
			}
			default: {
				continue;
			}
			}

			return factory->createNode(node.get(),
						   doc,
						   context);
		}
	}

	return 0;
}




DbXmlAttributeAxis::DbXmlAttributeAxis(const LocationInfo *location,
				       const DbXmlNodeImpl *contextNode,
				       const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  i_(0)
{
}

NsDomNodeRef DbXmlAttributeAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		DBXML_ASSERT(!nodeRef_);
		if(contextNode_->getNsNodeType() == nsNodeElement) {
			nodeRef_ = contextNode_->getNsNode();
		}
	}

	if (nodeRef_) {
		unsigned int nLen = nodeRef_->numAttrs();
		while(i_ < (int)nLen) {
			NsDomAttr attr(*nodeRef_,
				       contextNode_->getNsDoc(),
				       i_);
			++i_;
			// Check to see if this is a namespace attribute
			if(!XPath2Utils::equals(attr.getNsUri(), XMLUni::fgXMLNSURIName))
				return new NsDomAttr(attr);
		}
	}

	return 0;
}

DbXmlNamespaceAxis::DbXmlNamespaceAxis(const LocationInfo *location,
				       const DbXmlNodeImpl *contextNode,
				       const DbXmlNodeTest *nodeTest)
	: ResultImpl(location),
	  nodeObj_(contextNode),
	  contextNode_(contextNode->getNsDomNode()),
	  nodeTest_(nodeTest),
	  node_((contextNode_->getNsNodeType() == nsNodeElement) ?
		((NsDomElement*)contextNode_.get()) : 0),
	  i_(0),
	  state_(CHECK_ELEMENT)
{
	nodeRef_ = (node_ ? node_->getNsNode() : 0);
}

Item::Ptr DbXmlNamespaceAxis::next(DynamicContext *context)
{
	DbXmlNamespaceNode::Ptr node;
	while((node = nextNode(context)).notNull()) {
		if(nodeTest_ == 0) return node;

		SequenceType::ItemType *itemType = nodeTest_->getItemType();
		if(itemType != 0) {
			if(itemType->matches((Node::Ptr)node, context)) {
				return node;
			}
		}
		else {
			if(nodeTest_->getTypeWildcard()) { if(nodeTest_->getHasChildren()) continue; }
			else if(nodeTest_->getNodeType() != Node::namespace_string) continue;
			if(!nodeTest_->getNameWildcard() && !XPath2Utils::equals(node->getPrefix(), nodeTest_->getNodeName())) continue;
			if(!nodeTest_->getNamespaceWildcard() && !XPath2Utils::equals(XMLUni::fgZeroLenString, nodeTest_->getNodeUri())) continue;

			return node;
		} 
	}

	return 0;
}

DbXmlNamespaceNode::Ptr DbXmlNamespaceAxis::nextNode(DynamicContext *context)
{
	DbXmlNamespaceNode::Ptr result(0);

	while(result.isNull()) {
		switch(state_) {
		case CHECK_ELEMENT: {
			const XMLCh *uri = node_->getNsUri();
			const XMLCh *prefix = node_->getNsPrefix();
			std::string spfx;
			spfx = (prefix ? (XMLChToUTF8(prefix).str()) : "");
			bool inserted = done_.insert(spfx).second;	
			//if(done_.insert(prefix).second && uri && *uri) {
			if(inserted && uri && *uri) {
				result = new DbXmlNamespaceNode(
					prefix, uri,
					(NsDomElement*)contextNode_.get(),
					*nodeObj_->getXmlDocument(), context);
			}
			state_ = CHECK_ATTR;
			break;
		}
		case CHECK_ATTR: {
			if (nodeRef_ && i_ < (int)nodeRef_->numAttrs()) {
				NsDomAttr tmpAttr(*nodeRef_,
						  contextNode_->getNsDoc(),
						  i_);
				++i_;

				// Check to see if this is a namespace attribute
				if(!XPath2Utils::equals(tmpAttr.getNsUri(), XMLUni::fgXMLNSURIName)) {
					const XMLCh* uri = tmpAttr.getNsUri();
					const XMLCh* prefix = tmpAttr.getNsPrefix();
					std::string spfx;
					spfx = (prefix ? (XMLChToUTF8(prefix).str()) : "");
					bool inserted = done_.insert(spfx).second;	
					if(uri && *uri && inserted) {
						result = new DbXmlNamespaceNode(
							prefix, uri,
							(NsDomElement*)contextNode_.get(),
							*nodeObj_->getXmlDocument(), context);
					}
				} else {
					const XMLCh *prefix =
						(tmpAttr.getNsPrefix() == 0) ? 0 :
						tmpAttr.getNsLocalName();
					const XMLCh* uri = tmpAttr.getNsNodeValue();
					std::string spfx;
					spfx = (prefix ? (XMLChToUTF8(prefix).str()) : "");
					bool inserted = done_.insert(spfx).second;	
					//if(done_.insert(prefix).second && uri && *uri) {
					if(inserted && uri && *uri) {
						result = new DbXmlNamespaceNode(
							prefix, uri,
							(NsDomElement*)contextNode_.get(),
							*nodeObj_->getXmlDocument(), context);
					}
				}
			} else {
				node_ = node_->getNsParentNode();
				if(!node_ || node_->getNsNodeType() != nsNodeElement) {
					state_ = DO_XML;
				} else {
					nodeRef_ = node_->getNsNode();
					i_ = 0;
					state_ = CHECK_ELEMENT;
				}
			}
			break;
		}
		case DO_XML: {
			result = new DbXmlNamespaceNode(
				XMLUni::fgXMLString, XMLUni::fgXMLURIName,
				(NsDomElement*)contextNode_.get(),
				*nodeObj_->getXmlDocument(), context);
			state_ = DONE;
			break;
		}
		case DONE:
			return 0;
		}
	}

	return result;
}

DbXmlAttributeOrChildAxis::DbXmlAttributeOrChildAxis(const LocationInfo *location,
						     const DbXmlNodeImpl *contextNode,
						     const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  nodeRef_(0),
	  i_(0),
	  child_(0)
{
}

NsDomNodeRef DbXmlAttributeOrChildAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement)
			nodeRef_ = contextNode_->getNsNode();
	}

	if(nodeRef_) {
		unsigned int nLen = nodeRef_->numAttrs();
		while(i_ < (int)nLen) {
			NsDomAttr tattr(*nodeRef_,
					contextNode_->getNsDoc(), i_);
			++i_;
			// Check to see if this is a namespace attribute
			if(!XPath2Utils::equals(tattr.getNsUri(), XMLUni::fgXMLNSURIName))
				return new NsDomAttr(tattr);
		}

		nodeRef_ = 0;
		child_ = contextNode_->getNsFirstChild();
	} else if (child_) {
		child_ = child_->getNsNextSibling();
	}

	return child_;
}

DbXmlChildAxis::DbXmlChildAxis(const LocationInfo *location,
			       const DbXmlNodeImpl *contextNode,
			       const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  child_(0)
{
}

NsDomNodeRef DbXmlChildAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		child_ = contextNode_->getNsFirstChild();
	} else if (child_) {
		child_ = child_->getNsNextSibling();
	}

	return child_;
}


ElementChildAxis::ElementChildAxis(const LocationInfo *location,
				   const DbXmlNodeImpl *contextNode,
				   const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  child_(0)
{
}

NsDomNodeRef ElementChildAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement ||
			contextNode_->getNsNodeType() == nsNodeDocument) {
			child_ = ((NsDomElement*)contextNode_.get())->
				getElemFirstChild();
		}
	} else if (child_)
		child_ = ((NsDomElement*)child_.get())->getElemNext();

	return child_;
}


DbXmlDescendantAxis::DbXmlDescendantAxis(const LocationInfo *location,
					 const DbXmlNodeImpl *contextNode,
					 const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  descendant_(0)
{
}

NsDomNodeRef DbXmlDescendantAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		descendant_ = contextNode_->getNsFirstChild();
	} else if (descendant_) {
		NsDomNode *result = descendant_->getNsFirstChild();
		while(result == 0) {
			result = descendant_->getNsNextSibling();
			if(result == 0) {
				descendant_ = descendant_->getNsParentNode();
				if (!descendant_ ||
				    *descendant_ == *contextNode_) break;
			}
		}

		descendant_ = result;
	}

	return descendant_.get();
}


ElementDescendantAxis::ElementDescendantAxis(const LocationInfo *location,
					     const DbXmlNodeImpl *contextNode,
					     const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  nscontext_(0),
	  descendant_(0)
{
}

NsDomNodeRef ElementDescendantAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement ||
			contextNode_->getNsNodeType() == nsNodeDocument) {

			nscontext_ = contextNode_;
			descendant_ = ((NsDomElement*)nscontext_.get())->
				getElemFirstChild();
		}
	} else if (descendant_) {
		NsDomElement *result = ((NsDomElement*)descendant_.get())->
			getElemFirstChild();

		while(result == 0) {
			result = ((NsDomElement*)descendant_.get())->
				getElemNext();
			if(result == 0) {
				descendant_ =
					((NsDomElement*)descendant_.get())->
					getElemParent();
				if(!descendant_ || *descendant_ == *nscontext_) break;
			}
		}

		descendant_ = result;
	}

	return descendant_;
}



DbXmlDescendantOrSelfAxis::DbXmlDescendantOrSelfAxis(const LocationInfo *location,
						     const DbXmlNodeImpl *contextNode,
						     const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  descendant_(0)
{
}

NsDomNodeRef DbXmlDescendantOrSelfAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		descendant_ = contextNode_;
	}
	else if (descendant_) {
		NsDomNode *result = descendant_->getNsFirstChild();

		while(result == 0 && *descendant_ != *contextNode_) {
			result = descendant_->getNsNextSibling();
			if(result == 0) {
				descendant_ = descendant_->getNsParentNode();
				if (!descendant_ || *descendant_ == *contextNode_) break;
			}
		}

		descendant_ = result;
	}

	return descendant_;
}


ElementDescendantOrSelfAxis::ElementDescendantOrSelfAxis(const LocationInfo *location,
							 const DbXmlNodeImpl *contextNode,
							 const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  nscontext_(0),
	  descendant_(0)
{
}

NsDomNodeRef ElementDescendantOrSelfAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;

		if(contextNode_->getNsNodeType() == nsNodeElement ||
			contextNode_->getNsNodeType() == nsNodeDocument) {

			nscontext_ = contextNode_;
			descendant_ = nscontext_;
		}
	} else if (descendant_) {
		NsDomElement *result = ((NsDomElement*)descendant_.get())->
			getElemFirstChild();

		while ((result == 0) && (*descendant_ != *nscontext_)){
			result = ((NsDomElement*)descendant_.get())->
				getElemNext();
			if(result == 0) {
				descendant_ =
					((NsDomElement*)descendant_.get())->
					getElemParent();
				if(!descendant_ || *descendant_ == *nscontext_) break;
			}
		}

		descendant_ = result;
	}

	return descendant_;
}


DbXmlParentAxis::DbXmlParentAxis(const LocationInfo *location,
				 const DbXmlNodeImpl *contextNode,
				 const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest)
{
}

NsDomNodeRef DbXmlParentAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		return contextNode_->getNsParentNode();
	}
	return 0;
}

////////////////////
// DbXmlAncestorAxis

DbXmlAncestorAxis::DbXmlAncestorAxis(const LocationInfo *location,
				     const DbXmlNodeImpl *contextNode,
				     const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  ancestor_(0)
{
}

NsDomNodeRef DbXmlAncestorAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		ancestor_ = contextNode_;
	}
	// do not return contextNode_
	if(ancestor_)
		ancestor_ = ancestor_->getNsParentNode();
	return ancestor_;
}


////////////////////
// DbXmlAncestorOrSelfAxis

DbXmlAncestorOrSelfAxis::DbXmlAncestorOrSelfAxis(
	const LocationInfo *location,
	const DbXmlNodeImpl *contextNode,
	const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  ancestor_(0)
{
}

NsDomNodeRef DbXmlAncestorOrSelfAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		ancestor_ = contextNode_;
		// return contextNode_ the first time
	} else if (ancestor_)
		ancestor_ = ancestor_->getNsParentNode();
	return ancestor_;
}


//////////////////////
// DbXmlFollowingAxis

DbXmlFollowingAxis::DbXmlFollowingAxis(const LocationInfo *location,
				       const DbXmlNodeImpl *contextNode,
				       const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  node_(0)
{
}

// depth-first tree traversal
NsDomNodeRef DbXmlFollowingAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		node_ = contextNode_;
	}
	if (node_) {
		NsDomNode *result = 0;
		if (*node_ != *contextNode_)
			result = node_->getNsFirstChild();
		while (result == 0 && node_) {
			result = node_->getNsNextSibling();
			if (result == 0)
				node_ = node_->getNsParentNode();
		}
		node_ = result;
	}
	return node_;
}


////////////////////////////
// DbXmlFollowingSiblingAxis

DbXmlFollowingSiblingAxis::DbXmlFollowingSiblingAxis(
	const LocationInfo *location,
	const DbXmlNodeImpl *contextNode,
	const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  node_(0)
{
}

NsDomNodeRef DbXmlFollowingSiblingAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		node_ = contextNode_->getNsNextSibling();
	} else if (node_) {
		node_ = node_->getNsNextSibling();
	}
	return node_;
}

//////////////////////
// DbXmlPrecedingAxis

DbXmlPrecedingAxis::DbXmlPrecedingAxis(const LocationInfo *location,
				       const DbXmlNodeImpl *contextNode,
				       const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  node_(0), parent_(0)
{
}

// walk the tree from bottom-right up...
NsDomNodeRef DbXmlPrecedingAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		node_ = contextNode_;
		parent_ = node_->getNsParentNode();
	}
	if (node_) {
		NsDomNode *result = node_->getNsPrevSibling();
		while (result == 0) {
			node_ = node_->getNsParentNode();
			if (node_ &&
			    *node_ == *parent_) {
				parent_ = parent_->getNsParentNode();
				result = node_->getNsPrevSibling();
			} else
				break;
		}
		while (result != 0) {
			node_ = result;
			result = node_->getNsLastChild();
		}
	}
	return node_;
}



////////////////////////////
// DbXmlPrecedingSiblingAxis

DbXmlPrecedingSiblingAxis::DbXmlPrecedingSiblingAxis(
	const LocationInfo *location,
	const DbXmlNodeImpl *contextNode,
	const DbXmlNodeTest *nodeTest)
	: DbXmlAxis(location, contextNode, nodeTest),
	  sibling_(0)
{
}

// walk the tree from bottom-right up...
NsDomNodeRef DbXmlPrecedingSiblingAxis::nextNode(DynamicContext *context)
{
	if(toDo_) {
		// initialise
		toDo_ = false;
		sibling_ = contextNode_->getNsPrevSibling();
	} else if (sibling_)
		sibling_ = sibling_->getNsPrevSibling();
	return sibling_;
}


