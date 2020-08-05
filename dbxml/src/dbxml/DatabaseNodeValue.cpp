//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "DatabaseNodeValue.hpp"
#include "Results.hpp"
#include "DocID.hpp"
#include "nodeStore/NsNid.hpp"
#include "nodeStore/NsNode.hpp"
#include "nodeStore/NsDom.hpp"
#include "nodeStore/NsFormat.hpp"
#include "nodeStore/NsWriter.hpp"
#include "nodeStore/NsEventReader.hpp"
#include "Value.hpp"
#include "dataItem/DbXmlNodeImpl.hpp"
#include "dataItem/DbXmlFactoryImpl.hpp"
#include "UTF8.hpp"
#include <dbxml/XmlResults.hpp>

using namespace DbXml;

#define GET_RESULTS() ((Results &) ((XmlResults&)results_))
#define GET_MANAGER() GET_RESULTS().getManager()

// Implementation

DatabaseNodeValue::DatabaseNodeValue()
	: cid_(0), index_(0), nodePtr_(0)
{}

DatabaseNodeValue::DatabaseNodeValue(XmlValue &value, XmlResults &results)
	: RawNodeValue(results), nodePtr_(0)
{
	DBXML_ASSERT(value.isNode());

	DbXmlNodeValue *val = const_cast<DbXmlNodeValue*>(((Value*)value)->
							  getDbXmlNodeValueObject());
	DBXML_ASSERT(val);
	type_ = val->getNodeType();
	const DbXmlNodeImpl *node = val->getNodeImpl(NULL, false);
	DBXML_ASSERT(node || type_ == nsNodeDocument);
	const Document *doc = val->getDocument();
	DBXML_ASSERT(doc);
	did_ = doc->getID();
	cid_ = doc->getContainerID();
	if (node) {
		nid_.copy(node->getNodeID());
		index_ = node->getIndex();
	}else
		nid_.set(rootNid);
}

DatabaseNodeValue::DatabaseNodeValue(const DatabaseNodeValue &other)
	: RawNodeValue(other.results_), nodePtr_(0)
{
	type_ = other.type_;
	did_ = other.did_;
	cid_ = other.cid_;
	nid_.copy(other.nid_);
	index_ = other.index_;
}

DatabaseNodeValue::DatabaseNodeValue(const void *data, XmlResults &results)
	: RawNodeValue(results), nodePtr_(0)
{
	loadValue(data);
}

// if the container/did can be found in the
// minder owned by Results, use the DB; otherwise,
// it comes from the Container.
DbWrapper *DatabaseNodeValue::getDocDB() const
{
	if (!xdoc_.isNull()) {
		DbWrapper *docDb = (*xdoc_).getDocDb();
		if (docDb) return docDb;
	}

	Results &res = GET_RESULTS();
	XmlManager &mgr = res.getManager();
	CacheDatabaseMinder &minder = res.getDbMinder();
	CacheDatabase *db = minder.findOrAllocate(mgr, cid_,
						  false /* don't allocate */);
	if (db)
		return db->getDb();

	// get container's DB
	DBXML_ASSERT(cid_ > 0);
	// don't acquire reference on the container
	ContainerBase *cont = ((Manager&)mgr).getContainerFromID(cid_, false);
	if (!cont)
		throw XmlException(XmlException::CONTAINER_CLOSED, "Cannot complete operation because container is closed.");
	 DbWrapper *docDb = cont->getDbWrapper();
	 //Materialize whole documents that have not already been parsed into the temporary nodestorage db
	 if (!docDb){
		XmlDocument doc = asDocument();
		NsDomNodeRef ref = (*doc).getElement(*NsNid::getRootNid(),0);
		docDb = (*doc).getDocDb();
	 }
	 return docDb;
}

CacheDatabaseMinder *DatabaseNodeValue::getDBMinder() const 
{
	Results &res = GET_RESULTS();
	return &res.getDbMinder();
}

DictionaryDatabase *DatabaseNodeValue::getDictDB() const
{
	XmlManager &mgr = GET_MANAGER();
	// don't acquire reference on the container
	ContainerBase *cont = ((Manager&)mgr).getContainerFromID(cid_, false);
	if (!cont)
		throw XmlException(XmlException::CONTAINER_CLOSED, "Cannot complete operation because container is closed.");
	return cont->getDictionaryDatabase();
}

void DatabaseNodeValue::getNsDomNode() const
{ 
	if (!node_) {
		doc_.init(GET_RESULTS().getOperationContext().txn(),
			  getDocDB(), getDictDB(),
			  did_, cid_, 0);
		NsNode *node = doc_.getNode(nid_);
		if (node) {
			// create NsDom object
			if (type_ == nsNodeElement ||
			    type_ == nsNodeDocument)
				node_ = new NsDomElement(node, &doc_);
			else {
				NsDomElement telem(node, &doc_);
				if (type_ == nsNodeAttr)
					node_ = telem.getNsAttr(index_);
				else
					node_ = telem.getNsTextNode(index_);
			}
		}
		if (!node_) {
			// this is most likely caused by referencing a now-deleted node
			throw XmlException(XmlException::INVALID_VALUE,
					   "An attempt was made to reference a node that no longer exists; the node may be a bound variable or part of a query context");
		}
	}
}

std::string DatabaseNodeValue::getNodeName() const
{
	if(type_ == nsNodeDocument)
		return "#document";
	getNsDomNode();
	const XMLCh *name =node_->getNsNodeName();
	return XMLChToUTF8(name).str();
}

std::string DatabaseNodeValue::getNodeValue() const
{
	if(type_ == nsNodeDocument)
		return "";
	// TBD -- maybe use utf-8 interfaces for attrs and text
	getNsDomNode();
	const XMLCh *value =node_->getNsNodeValue();
	return XMLChToUTF8(value).str();
}

std::string DatabaseNodeValue::getNamespaceURI() const
{
	if(type_ == nsNodeDocument)
		return "";
	getNsDomNode();
	const char *str = (const char *)node_->getNsUri8();
	if(str)
		return str;
	else
		return "";
}

std::string DatabaseNodeValue::getPrefix() const
{
	if(type_ == nsNodeDocument)
		return "";
	getNsDomNode();
	const char *str = (const char *)node_->getNsPrefix8();
	if(str)
		return str;
	else
		return "";
}

std::string DatabaseNodeValue::getLocalName() const
{
	if(type_ == nsNodeDocument)
		return "#document";
	getNsDomNode();
	const char *str = (const char *)node_->getNsLocalName8();
	if(str)
		return str;
	else
		return "";
}

std::string DatabaseNodeValue::asString() const
{
	if(type_ == nsNodeDocument) {
		(void)asDocument();
		DBXML_ASSERT(!xdoc_.isNull());
		std::string content;
		return xdoc_.getContent(content);
	}
	if (type_ == nsNodeElement) {
		// Make output writer
		std::string outstr;
		StringNsStream output(outstr);
		NsWriter writer(&output);

		// Make an event reader
		ScopedPtr<EventReader> reader(
			new NsEventReader(
				GET_RESULTS().getOperationContext().txn(),
				getDocDB(),
				getDictDB(),
				did_, cid_, 0, NS_EVENT_BULK_BUFSIZE, &nid_));
		writer.writeFromReader(*reader);
		return outstr;
	} else if (type_ == nsNodeAttr) {
		getNsDomNode();
		NsDomAttr *attr = (NsDomAttr*)node_.get();
		DBXML_ASSERT(attr);
		std::string str = "{";
		const char *uri = (const char *)attr->getNsUri8();
		if (uri)
			str.append(uri);
		str.append("}");
		str.append((const char *)attr->getNsLocalName8());
		str.append("=\"");
		str.append((const char *)attr->getNsValue8());
		str.append("\"");
		return str;
	} else {
		getNsDomNode();
		NsDomText *text = (NsDomText*)node_.get();
		DBXML_ASSERT(text);
		if(type_ == nsNodeText) {
			return (const char *)text->getNsValue8();
		} else if(type_ == nsNodeComment) {
			std::string str = "<!--";
			str.append((const char *)text->getNsValue8());
			str.append("-->");
			return str;
		} else if(type_ == nsNodeCDATA) {
			std::string str = "<![CDATA[";
			str.append((const char *)text->getNsValue8());
			str.append("]]>");
			return str;
		} else if(type_ == nsNodePinst) {
			std::string str = "<?";
			// PI target is localName
			str.append((const char *)text->getNsNodeName8());
			str.append(" ");
			str.append((const char *)text->getNsValue8());
			str.append("?>");
			return str;
		} else {
			DBXML_ASSERT(false);
		}
	}
	return "";
}

bool DatabaseNodeValue::asBoolean() const
{
	std::string s(asString());
	return (!s.empty() && s.compare("false") != 0);
}

double DatabaseNodeValue::asNumber() const
{
	return strtod(asString().c_str(), 0);
}

void DatabaseNodeValue::initIndexEntryFromThis(IndexEntry &ie) const
{
	ie.setDocID(did_);
	if(type_ != nsNodeDocument) {
		ie.setNodeID(nid_);
		if(type_ == nsNodeElement) {
			ie.setFormat(IndexEntry::NH_ELEMENT_FORMAT);
		} else if(type_ == nsNodeAttr) {
			ie.setFormat(IndexEntry::NH_ATTRIBUTE_FORMAT);
			ie.setIndex(index_);
		} else if(type_ == nsNodeText ||
			type_ == nsNodeCDATA) {
			ie.setFormat(IndexEntry::NH_TEXT_FORMAT);
			ie.setIndex(index_);
		} else if(type_ == nsNodeComment) {
			ie.setFormat(IndexEntry::NH_COMMENT_FORMAT);
			ie.setIndex(index_);
		} else if(type_ == nsNodePinst) {
			ie.setFormat(IndexEntry::NH_PI_FORMAT);
			ie.setIndex(index_);
		} else {
			throw XmlException(XmlException::INVALID_VALUE,
				"Node handle unavailable for node type");
		}
	} else
		ie.setFormat(IndexEntry::NH_DOCUMENT_FORMAT);
}

bool DatabaseNodeValue::equals(const Value &other) const
{
	const RawNodeValue *raw = other.getRawNodeValueObject();
	if (raw) {
		const DatabaseNodeValue *otherVal = raw->getDatabaseNodeValueObject();
		if (otherVal) {
			if ((otherVal->type_ == type_) &&
				(otherVal->nid_ == nid_) &&
				(otherVal->did_ == did_) &&
				(otherVal->cid_ == cid_) &&
				(otherVal->index_ == index_))
				return true;
		}
	}
	return false;
}

DatabaseNodeValue *DatabaseNodeValue::makeRelative(const NsNid &nid,
					 short type, int index) const
{
	DatabaseNodeValue *val = new DatabaseNodeValue(*this);
	val->type_ = type;
	val->index_ = index;
	try{
		val->nid_.copy(nid);
	}catch(XmlException &e){
		delete val;
		throw e;
	}catch(std::exception &e){
		delete val;
		throw e;
	}catch(...){
		delete val;
		throw XmlException(XmlException::INTERNAL_ERROR, "Unexpected internal error");
	}
	return val;
}

XmlValue DatabaseNodeValue::getParentNode() const
{
	if (type_ != nsNodeDocument) {
		getNsDomNode();
		NsDomNodeRef parent = node_->getNsParentNode();
		if (parent)
			return makeRelative(parent->getNodeId(),
					    parent->getNsNodeType(), 0);
	}
	return XmlValue();
}

XmlValue DatabaseNodeValue::getChild(bool isFirst) const
{
	getNsDomNode();
	if (node_) {
		NsDomNodeRef child = (isFirst ? node_->getNsFirstChild() :
				      node_->getNsLastChild());
		if (child)
			return makeRelative(child->getNodeId(),
					    child->getNsNodeType(),
					    child->getIndex());
	}
	return XmlValue();
}

XmlValue DatabaseNodeValue::getSibling(bool isNext) const
{
	if (type_ != nsNodeDocument) {
		getNsDomNode();
		if (node_) {
			NsDomNodeRef sib = (isNext ? node_->getNsNextSibling() :
					    node_->getNsPrevSibling());
			if (sib)
				return makeRelative(sib->getNodeId(),
						    sib->getNsNodeType(),
						    sib->getIndex());
		}
	}
	return XmlValue();
}

XmlValue DatabaseNodeValue::getOwnerElement() const {

	if (type_ ==  nsNodeAttr)
		return makeRelative(nid_, nsNodeElement, 0);
	throw XmlException(XmlException::INVALID_VALUE,
			   "Node is not an attribute node");

}

XmlResults DatabaseNodeValue::getAttributes() const {
	XmlManager &mgr = GET_MANAGER();
	ValueResults *vr = 0;
	try{
		vr = new ValueResults(mgr, getTransaction());
		vr->getDbMinder() = GET_RESULTS().getDbMinder();
		if (type_ == nsNodeElement) {
			getNsDomNode();
			NsDomElement *elem = (NsDomElement*)node_.get();
			DBXML_ASSERT(elem);
			int size = elem->getNumAttrs();
			for(int i = 0; i < size; ++i){
				DatabaseNodeValue *att = makeRelative(nid_, nsNodeAttr, i);
				att->copyContext(cid_, did_, results_, xdoc_, returnResults_);
				vr->add(att);
			}
		}
	}catch(XmlException &e){
		if(vr) delete vr;
		throw e;
	}catch(std::exception &e){
		if(vr) delete vr;
		throw e;
	}catch(...){
		if(vr) delete vr;
		throw XmlException(XmlException::INTERNAL_ERROR, "Unexpected internal error");
	}
	return vr;
}

XmlEventReader &DatabaseNodeValue::asEventReader() const
{
	const NsNid *nid = (type_ == nsNodeElement ? &nid_ : 0);
	if ((type_ == nsNodeDocument) ||
	    (type_ == nsNodeElement)) {
		return *(new NsEventReader(
				 GET_RESULTS().getOperationContext().txn(),
				 getDocDB(),
				 getDictDB(),
				 did_, cid_, 0, NS_EVENT_BULK_BUFSIZE, nid));
	}
	// The only way to support asEventReader on non-elements is if
	// the XmlEventReader interface were able to iterate through attributes
	// one at a time, rather than using an indexed interface.
	throw XmlException(XmlException::INVALID_VALUE,
			   "XmlValue::asEventReader requires an element node");
}

const XmlDocument &DatabaseNodeValue::asDocument() const
{
	if (xdoc_.isNull()) {
		XmlManager &mgr = GET_MANAGER();
		xdoc_ = mgr.createDocument();
		Document *doc = (Document*)xdoc_;
		doc->setID(did_);
		doc->setContainerID(cid_);
		doc->setTransaction(GET_RESULTS().getOperationContext().txn());
		if (cid_ == 0) {
			doc->setDbMinder(GET_RESULTS().getDbMinder());
			//Set query constructed documents as DOM
			if(!doc->getDbMinder().isNull()) { 
				doc->getDocDb();
				doc->setContentAsNsDom(did_, doc->getCacheDatabase());
			}
		} else
			doc->setAsNotMaterialized();
	}
	return xdoc_;
}

void DatabaseNodeValue::copyContext(int cid, const DocID &did,
			       XmlResults &results, 
			       XmlDocument &xdoc, XmlResults *returnResults)
{
	cid_ = cid;
	did_ = did;
	results_ = results;
	xdoc_ = xdoc;
	returnResults_ = returnResults;
}

DbXmlNodeImpl *DatabaseNodeValue::getNodeImpl(DynamicContext *context, bool validate)
{
	if(!context || nid_.isNull() || type_ == nsNodeDocument)
		return 0;
	if(nodePtr_.isNull()){
		getNsDomNode();
		nodePtr_ = ((DbXmlFactoryImpl*)context->getItemFactory())->createNode(node_.get(), asDocument(), context);
		// double-check type of node vs what it should be
		if ((validate) && (type_ != node_->getNsNodeType())) {
				throw XmlException(XmlException::INVALID_VALUE,
					   "An attempt was made to reference a node that no longer exists; the node may be a bound variable or part of a query context");
		}
	} else if (validate) {
		// validate if not fetching
		validateNode((DbXmlNodeImpl*)nodePtr_.get(), context);
	}
	return (DbXmlNodeImpl*)nodePtr_.get();
}
// Order is:
// RawNodeType
// doc ID
// container id
// type
// index
// nid
int DatabaseNodeValue::marshal(void *&data)
{
	dataSize_ = 1; //RawNode Type
	dataSize_ += did_.marshalSize();
	dataSize_ += NsFormat::countInt(cid_);
	dataSize_ += 1; // type
	dataSize_ += NsFormat::countInt(index_);
	if (type_ != nsNodeDocument)
		dataSize_ += NsFormat::countId(nid_);
	dataSize_ += 4; // add padding
	data_ = NsUtil::allocate(dataSize_, "DatabaseNodeValue dump");
	xmlbyte_t *ptr = (xmlbyte_t *)data_;
	*ptr = DatabaseNodeType; 
	ptr++;
	ptr += did_.marshal((void*)ptr);
	ptr += NsFormat::marshalInt(ptr, cid_);
	ptr += NsFormat::marshalInt(ptr, type_);
	ptr += NsFormat::marshalInt(ptr, index_);
	if (type_ != nsNodeDocument)
		NsFormat::marshalId(ptr, nid_);
	data = data_;
	return dataSize_;
}

int DatabaseNodeValue::dumpValue(void *&data)
{
	if (!data_)
		return marshal(data);
	data = data_;
	return dataSize_;
}

void DatabaseNodeValue::loadValue(const void *data)
{
	const xmlbyte_t *ptr = (const xmlbyte_t *)data;
	int32_t ttype;
	ptr++;
	ptr += did_.unmarshal((const void*)ptr);
	ptr += NsFormat::unmarshalInt(ptr, (int32_t*)&cid_);
	ptr += NsFormat::unmarshalInt(ptr, &ttype);
	type_ = (short) ttype;
	ptr += NsFormat::unmarshalInt(ptr, (int32_t*)&index_);
	if (type_ != nsNodeDocument)
		nid_.set((const unsigned char *)ptr);
	else
		nid_.set(rootNid);
}

