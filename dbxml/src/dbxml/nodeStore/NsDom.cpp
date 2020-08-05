//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "NsUtil.hpp"
#include "NsDoc.hpp"
#include "NsConstants.hpp"
#include "NsDom.hpp"
#include "NsEventReader.hpp"
#include "../UTF8.hpp" // for XMLChToUTF8
#include <xercesc/util/XMLUri.hpp>

using namespace DbXml;
XERCES_CPP_NAMESPACE_USE;

static xmlch_t _nsDomDocName[] = { '#', 'd', 'o', 'c', 'u',
				'm', 'e', 'n', 't', 0 };
static xmlch_t _nsDomTextName[] = { '#', 't', 'e', 'x', 't', 0 };
static xmlch_t _nsDomCommentName[] = { '#', 'c', 'o', 'm',
				       'm', 'e', 'n', 't', 0 };
static xmlch_t _nsDomCdataName[] = { '#', 'c', 'd', 'a', 't', 'a', '-',
				       's', 'e', 'c', 't', 'i', 'o', 'n', 0 };

static void setString16(NsString &ds,
			NsDoc *doc,
			uint32_t id)
{
	ds.set(doc->getStringForID16(id));
}

static void NsDomNoMemory(const char *name)
{
	char buf[500];
	buf[0] = '\0';
	strcat(buf, "NsDom out of memory: ");
	strcat(buf, name);
	NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR, buf,
				 __FILE__, __LINE__);
}

// find{First,Last}[Child]TextIndex
// utility methods to skip entity text nodes
static int findLastChildTextIndex(NsNode *node, int index = -2)
{
	if (index == -2)
		index = node->getNumText() - 1;
	int firstChild = node->getFirstTextChildIndex();
	DBXML_ASSERT(firstChild >= 0);
	while (index >= firstChild) {
		if (!nsIsEntityType(node->textType(index)))
			return index;
		--index;
	}
	return -1;
}

static int findFirstChildTextIndex(NsNode *node, int index = -2)
{
	if (index == -2)
		index = node->getFirstTextChildIndex();
	int last = node->getNumText();
	while (index < last) {
		if (!nsIsEntityType(node->textType(index)))
			return index;
		++index;
	}
	return -1;
}

static int findFirstTextIndex(NsNode *node, int index = 0)
{
	int numLeading = node->getNumLeadingText();
	DBXML_ASSERT(numLeading);
	while (index < numLeading) {
		if (!nsIsEntityType(node->textType(index)))
			return index;
		++index;
	}
	return -1;
}

static int findLastTextIndex(NsNode *node, int index = -2)
{
	if (index == -2)
		index = node->getNumLeadingText() - 1;
	while (index >= 0) {
		if (!nsIsEntityType(node->textType(index)))
			return index;
		--index;
	}
	return -1;
}

//
// NsDomElement implementation
//
NsDomElement::NsDomElement(NsNode *node,
			   NsDoc *document) :

	NsDomNode(document), node_(node),
	lname_(0)
{
	if (isDocumentNode()) {
		qname_.set(_nsDomDocName, false);
		lname_ = qname_.get();
	}
}

NsDomElement::~NsDomElement()
{
	// no-op for now
}

NsDomNode *NsDomElement::duplicate() const
{
	return new NsDomElement(*node_, doc_);
}

void NsDomElement::setIsRoot()
{
	DBXML_ASSERT(node_);
	node_->setIsRoot();
}

bool NsDomElement::isRoot() const
{
	DBXML_ASSERT(node_);
	return node_->isRoot();
}

void NsDomElement::refreshNode(OperationContext &oc,
			       bool forWrite)
{
	DBXML_ASSERT(doc_ && doc_->getDocDb());
	node_ = NsFormat::fetchNode(getNodeId(),
				    doc_->getDocID(),
				    *doc_->getDocDb(),
				    oc, forWrite);
}

// NOTE: this method will *NOT* recurse and perform the
// entire base-uri() function. It just looks for xml:base
// attributes.  In addition, it's an error to call this method
// on the document node
const xmlch_t *
NsDomElement::getNsBaseUri(const xmlch_t *base) const
{
        static const xmlch_t xmlBaseString[] = { 'x', 'm', 'l', ':', 'b', 'a', 's', 'e', 0 };

	if (getNsNodeType() == nsNodeDocument)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "Called getNsBaseUri() on document node",
					 __FILE__, __LINE__);
        const xmlch_t *ret = baseUri_.get();
	if (!ret) {
		if(node_->hasAttributes()) {
			for (unsigned int i = 0; i < node_->numAttrs(); ++i) {
				NsDomAttr attr(*node_, doc_, i);
				if (NsUtil::nsStringEqual(xmlBaseString,
							  attr.getNsNodeName())) {
					const xmlch_t *uri = attr.getNsNodeValue();
					if(uri && *uri && base && *base) {
						XMLUri temp(base,
							    Globals::defaultMemoryManager);
						XMLUri temp2(&temp, uri,
							     Globals::defaultMemoryManager);
						baseUri_.set(temp2.getUriText());
					} else
						baseUri_.set(uri);
					break;
				}
			}
		}
	}
        return baseUri_.get();
}

void
NsDomElement::_getName() const
{
	const xmlch_t *qname = doc_->getQname(node_->getName());
	DBXML_ASSERT(qname);
	qname_.set(qname, true);
	lname_ = qname;
	if (node_->namePrefix() != NS_NOPREFIX)
		while (*lname_++ != xmlchColon);
}

const xmlbyte_t *
NsDomElement::getNsLocalName8() const
{
	if (isDocumentNode()) return 0;
	return (const xmlbyte_t*)node_->getName()->n_text.t_chars;
}

const xmlbyte_t *
NsDomElement::getNsUri8() const
{
	if (isDocumentNode() || !node_->hasUri())
		return 0;
	// NOTE: the string returned here is only valid
	// unti the OperationContext in the _document is
	// reused.  This is safe (for now), as the only caller
	// of this is just using the return value for a string comparison
	return (const xmlbyte_t *) doc_->getStringForID(
		node_->uriIndex());
}

const xmlbyte_t *
NsDomElement::getNsPrefix8() const
{
	if (isDocumentNode() || !node_->hasNamePrefix())
		return 0;
	// NOTE: the string returned here is only valid
	// unti the OperationContext in the _document is
	// reused.  This is safe (for now), as the only caller
	// of this is just using the return value for a string comparison
	return (const xmlbyte_t *) doc_->getStringForID(
		node_->namePrefix());
}

// nodeName is the qname (vs local part), so need to address
// namespaces.
const xmlch_t *
NsDomElement::getNsNodeName() const
{
	if (!qname_.get()) {
		_getName();
		DBXML_ASSERT(qname_.get() && lname_);
	}
	return qname_.get();
}

const xmlch_t *
NsDomElement::getNsLocalName() const
{
	if (isDocumentNode()) return 0;
	if (!lname_) {
		_getName();
		DBXML_ASSERT(qname_.get() && lname_);
	}
	return lname_;
}

const xmlch_t *
NsDomElement::getNsPrefix() const
{
	if (isDocumentNode() || node_->namePrefix() == NS_NOPREFIX)
		return 0;

	if (!prefix_.get()) {
		setString16(prefix_, doc_,
			    node_->namePrefix());
	}
	return prefix_.get();
}

const xmlch_t *
NsDomElement::getNsUri() const
{
	if (isDocumentNode() || !node_->hasUri())
		return 0;

	if (!uri_.get()) {
		setString16(uri_, doc_,
			    node_->uriIndex());
	}
	return uri_.get();
}

const xmlch_t *
NsDomElement::getNsNodeValue() const
{
	return 0;
}

NsNode *
NsDomElement::getParentNode() const
{
	return doc_->getNode(node_->getParentNid());
}

NsDomElement *
NsDomElement::getElemParent()
{
	if (isRoot() || isDocumentNode())
		return 0;
	NsDomElement *ret = new NsDomElement(getParentNode(), doc_);
	if (!ret)
		NsDomNoMemory("getElemParent");
	return ret;
}

NsDomElement *
NsDomElement::getElemPrev()
{
	if (node_->hasPrev()) {
		NsDomElement *ret = new NsDomElement(
			doc_->getNode(node_->getPrevNid()), doc_);
		if (!ret)
			NsDomNoMemory("getElemPrev");
		return ret;
	}
	return 0;
}

// next sibling elem is the next one after this element, or
// the next one after its last descendant
NsDomElement *
NsDomElement::getElemNext()
{
	if (node_->hasNext()) {
		NsDomElement *ret = new NsDomElement(
			doc_->getNode(node_->getLastDescendantNidOrSelf(),
				      true /* get next node */),
			doc_);
		if (!ret)
			NsDomNoMemory("getElemNext");
		return ret;
	}
	return 0;
}

NsDomElement *
NsDomElement::getElemLastChild()
{
	if (node_->hasChildElem()) {
		NsDomElement *ret = new NsDomElement(
			doc_->getNode(node_->getLastChildNid()), doc_);
		if (!ret)
			NsDomNoMemory("getElemLastChild");
		return ret;
	}
	return 0;
}

// first child is the next node in document order after this one
NsDomElement *
NsDomElement::getElemFirstChild()
{
	if (node_->hasChildElem()) {
		NsDomElement *ret = new NsDomElement(
			doc_->getNode(node_->getNid(), true /* get next */),
			doc_);
		if (!ret)
			NsDomNoMemory("getElemFirstChild");
		return ret;
	}
	return 0;
}

// If the node has element children, the first child is either
// the "next" node in doc order or leading text off that node.
// If no element children, it's a direct text child of this one
NsDomNode *
NsDomElement::getNsFirstChild()
{
	NsDomNode *ret = 0;
	if (node_->hasChildNode()) {
		if (node_->hasChildElem()) {
			NsNode *node = doc_->getNode(node_->getNid(),
						     true /* get next */);
			if (node->hasLeadingText())
				ret = new NsDomText(node, doc_, 0);
			else
				ret = new NsDomElement(node, doc_);
		} else {
			// skip leading text, if present
			int index = findFirstChildTextIndex(*node_);
			if (index >= 0)
				ret = new NsDomText(*node_, doc_, index);
			else
				return 0;
		}
		if (!ret)
			NsDomNoMemory("getNsFirstChild");
	}
	return ret;
}
// If the node has text children, the last child is 
// the last child text node; otherwise, it's the last child element.
// Note that it's possible to return 0 if there are no child elements,
// and all text children are "entity" types
NsDomNode *
NsDomElement::getNsLastChild()
{
	NsDomNode *ret = 0;
	if (node_->hasChildNode()) {
		if (node_->hasTextChild()) {
			// this is complicated by the need to skip
			// the "entity" text types
			int index = findLastChildTextIndex(*node_);
			if (index >= 0)
				ret = new NsDomText(*node_, doc_, index);
			else if (node_->hasChildElem())
				ret = getElemLastChild();
			else
				return 0; // used up all possible children
		} else {
			ret = getElemLastChild();
		}
		if (!ret)
			NsDomNoMemory("getNsLastChild");
	}
	return ret;
}

NsDomElement *
NsDomElement::getNsParentNode()
{
	return getElemParent();
}

//
// sibling navigation is complicated by materialization of
// text nodes and skipping of entities
//

// next sibling is (1) element or (2) leading text or (3) child
// text of the parent node
NsDomNode *
NsDomElement::getNsNextSibling()
{
	NsDomNode *ret = 0;
	if (node_->hasNext()) {
		// either next element, or leading text, so
		// fetch it
		NsNode *nextNode = doc_->getNode(
			node_->getLastDescendantNidOrSelf(),
			true /* get next node */);
		DBXML_ASSERT(nextNode);
		if (nextNode->hasLeadingText()) {
			int index = findFirstTextIndex(nextNode);
			if (index >= 0)
				ret = new NsDomText(nextNode, doc_, index);
			else
				ret = new NsDomElement(nextNode, doc_);
		} else
			ret = new NsDomElement(nextNode, doc_);
		if (!ret)
			NsDomNoMemory("getNsNextSibling");
	} else if (!isDocumentNode()) {
		// need to get parent to know if there is a next
		// (text) sibling.  This is very inefficient, but the
		// format doesn't track text sibling info
		NsNodeRef pnode(getParentNode());
		if (pnode && pnode->getNumChildText()) {
			int index = findFirstChildTextIndex(pnode.get());
			if (index >= 0) {
				ret = new NsDomText(pnode.get(), doc_, index);
				if (!ret)
					NsDomNoMemory("getNsNextSibling");
			}
		}
	}
	return ret;
}

// previous sibling is one of
// (1) prev sibling element/node or (2) leading text
NsDomNode *
NsDomElement::getNsPrevSibling()
{
	NsDomNode *ret = 0;
	if (node_->hasLeadingText()) {
		int index = findLastTextIndex(*node_);
		if (index >= 0) {
			ret = new NsDomText(*node_, doc_, index);
			if (!ret)
				NsDomNoMemory("getNsPrevSibling");
		}
	}
	if (!ret)
		ret = getElemPrev();
	return ret;
}

NsDomText *
NsDomElement::getNsTextNode(int index)
{
	// verify text at index
	if (node_->getNumText() <= index)
		return NULL;
	NsDomText *ret = new NsDomText(*node_, doc_, index);
	if (!ret)
		NsDomNoMemory("getNsTextNode");
	return ret;
}

NsDomAttr *
NsDomElement::getNsAttr(int index)
{
	// verify attribute at index
	if (node_->numAttrs() <= (unsigned int) index)
		return NULL;
	NsDomAttr *ret = new NsDomAttr(*node_, doc_, index);
	if (!ret)
		NsDomNoMemory("getNsAttr");
	return ret;
}

NsNodeType_t
NsDomElement::getNsNodeType() const
{
	if (isDocumentNode())
		return nsNodeDocument;
	return nsNodeElement;
}

//
// Structure and functions to implement
// getNsTextContent().
//

struct textContent {
public:
	textContent()
		: buf(0), bufsize(0), offset(0) {}
	xmlch_t *buf;
	size_t bufsize;
	size_t offset;
};

static void
_reserve(struct textContent &buf, size_t needed)
{
	size_t avail = buf.bufsize - buf.offset;
	if (avail < needed) {
		size_t allocSize = (buf.bufsize + needed) << 1;
		xmlch_t *newbuf = (xmlch_t*)NsUtil::allocate(allocSize<<1);
		DBXML_ASSERT(newbuf);
		memcpy(newbuf, buf.buf, (buf.bufsize << 1));
		NsUtil::deallocate(buf.buf);
		buf.buf = newbuf;
		buf.bufsize = allocSize;
	}
}

static void
_addReaderText(const unsigned char *data, size_t len,
	       struct textContent &buf)
{
	if (len) {
		size_t needed = len;
		_reserve(buf, needed);
		// transcode
		xmlch_t *dest = buf.buf + buf.offset;
		buf.offset +=
			NsUtil::nsFromUTF8(&dest,
					   (const xmlbyte_t *)data,
					   needed, needed);
	}
}

static void getNsTextFromReader(NsEventReader &reader,
				struct textContent &buf)
{
	while (reader.hasNext()) {
		XmlEventReader::XmlEventType type = reader.next();
		if ((type == XmlEventReader::Characters) ||
		    (type == XmlEventReader::CDATA)) {
			size_t len;
			const unsigned char *val = reader.getValue(len);
			_addReaderText(val, len, buf);
		}
	}
}

const xmlch_t *
NsDomElement::getNsTextContent() const
{
	if (!textContent_.get()) {
		struct textContent buf;
		int32_t numChildText = node_->getNumChildText();
		// optimize the single child text node case
		if ((numChildText == 1) &&
		    !node_->hasChildElem()) {
			int32_t childIndex = node_->getFirstTextChildIndex();
			DBXML_ASSERT(childIndex >= 0);
			nsTextEntry_t *txt = node_->getTextEntry(childIndex);
			uint32_t ttype = nsTextType(txt->te_type);
			if (ttype == NS_TEXT ||
			    (ttype == NS_CDATA &&
			    (txt->te_type & NS_IGNORABLE) == 0)) {
				size_t needed = txt->te_text.t_len;
				if (needed) {
					_reserve(buf, needed);
					xmlch_t *dest = buf.buf + buf.offset;
					buf.offset += NsUtil::nsFromUTF8(
						&dest,
						(const xmlbyte_t *)txt->te_text.t_chars,
						needed, needed);
				}
			}
		} else if (numChildText || node_->hasChildElem()) {
			// either has multiple child text nodes, or
			// child elements to search
			NsNid nid = getNodeId();
			NsEventReader reader(*doc_, NS_EVENT_VALUE_BUFSIZE, &nid);
			
			getNsTextFromReader(reader, buf);
		}
		// always add null
		_reserve(buf, 1);
		buf.buf[buf.offset] = 0;
		textContent_.set(buf.buf, true);
	}
	return textContent_.get();
}

const NsNid
NsDomElement::getNodeId() const
{
	return node_->getNid();
}

int32_t
NsDomElement::getNsLevel() const
{
	return node_->getLevel();
}

//
// NsDomText implementation
//
NsDomText::NsDomText(NsNode *owner,
		     NsDoc *doc,
		     int32_t index)
	: NsDomNode(doc),
	  owner_(owner),
	  index_(index)
{
	type_ = owner_->textType(index_);
	// determine if the owner is also parent, this is
	// true if the index is *past* the leading text nodes
	if (index_ >= owner_->getNumLeadingText())
		ownerIsParent_ = true;
	else
		ownerIsParent_ = false;
}

NsDomNode *NsDomText::duplicate() const
{
	return new NsDomText(*owner_, doc_, index_);
}

void NsDomText::refreshNode(OperationContext &oc,
			    bool forWrite)
{
	DBXML_ASSERT(doc_ && doc_->getDocDb());
	owner_ = NsFormat::fetchNode(getNodeId(),
				     doc_->getDocID(),
				     *doc_->getDocDb(),
				     oc, forWrite);
}

NsNodeType_t
NsDomText::getNsNodeType() const
{
	if (!nsIsEntityText(type_)) {
		uint32_t type = nsTextType(type_);
		if (type == NS_TEXT)
			return nsNodeText;
		else if (type == NS_CDATA)
			return nsNodeCDATA;
		else if (type == NS_COMMENT)
			return nsNodeComment;
		else if (type == NS_PINST)
			return nsNodePinst;
	} else {
		if (nsIsEntityStart(type_))
			return nsNodeEntStart;
		return nsNodeEntEnd;
	}
	DBXML_ASSERT(false);
	return (NsNodeType_t)-1;
}

bool
NsDomText::getNsIsEntityType() const
{
	return nsIsEntityType(type_);
}

NsDomElement *
NsDomText::getNsParentNode()
{
	if (ownerIsParent_)
		return new NsDomElement(*owner_, getNsDoc());
	else {
		NsDomElement owner(*owner_, getNsDoc());
		return owner.getNsParentNode();
	}
}

// 2 cases:  leading text or child text.
// 1.  leading text.  If it has another leading text sib, that's it.
//     otherwise, next sib is owner element.
// 2.  child text.  If it has another child text sib, that's it; otherwise,
//     there is no next sib.
NsDomNode *
NsDomText::getNsNextSibling()
{
	int numLeading = owner_->getNumLeadingText();
	bool isLeading = (index_ < numLeading);
	int next = index_ + 1;
	if (isLeading) {
		next = findFirstTextIndex(*owner_, next);
		if (next >= 0)
			return new NsDomText(*owner_,
					     getNsDoc(),
					     next);
		else
			return new NsDomElement(*owner_,
						getNsDoc());
	}
	// child text
	if (next < owner_->getNumText()) {
		next = findFirstChildTextIndex(*owner_, next);
		if (next >= 0)
			return new NsDomText(*owner_,
					     getNsDoc(),
					     next);
	}
	// no sibling
	return 0;
}

// 2 cases:  leading text or child text.
// 1.  leading text.  If it has another leading text sib, that's it.
//     otherwise, need to look for owner's previous element.
// 2.  child text.  If it has another child text sib, that's it; otherwise,
//     if the owner has element children, it's the last one
NsDomNode *
NsDomText::getNsPrevSibling()
{
	int numLeading = owner_->getNumLeadingText();
	bool isLeading = index_ < numLeading;
	int next = index_ - 1;
	if (isLeading) {
		// next may be -1, in which case, it'll remain so
		next = findLastTextIndex(*owner_, next);
		if (next >= 0)
			return new NsDomText(*owner_,
					     getNsDoc(),
					     next);
		else if (owner_->hasPrev()) {
			// get owner's previous sib
			NsDomElement telem(*owner_, getNsDoc());
			return telem.getElemPrev();
		}
		return 0; // no prev sib
	}
	// child text
	if (next >= owner_->getFirstTextChildIndex()) {
		next = findLastChildTextIndex(*owner_, next);
		if (next >= 0)
			return new NsDomText(*owner_,
					     getNsDoc(),
					     next);
	} else if (owner_->hasChildElem()) {
		// get owner's last child elem
		NsDomElement telem(*owner_, getNsDoc());
		return telem.getElemLastChild();
	}
	// no sibling
	return 0;
}

int32_t
NsDomText::getNsLevel() const
{
	// text nodes are one level down from parents
	if (ownerIsParent_)
		return owner_->getLevel() + 1;
	return owner_->getLevel();
}

const xmlbyte_t *
NsDomText::getNsNodeName8() const
{
	// PINST has node name, but others don't
	if(nsTextType(type_) == NS_PINST)
		return owner_->getText(index_)->t_chars;
	return 0;
}

const xmlbyte_t *
NsDomText::getNsValue8() const
{
	const xmlbyte_t *val = owner_->getText(index_)->t_chars;
	if(nsTextType(type_) == NS_PINST)
		while (*val++); // get past NULL
	return val;
}

const xmlch_t *
NsDomText::_getText() const
{
	const xmlch_t *text = text_.get();
	if(text == 0) {
		text = getNsDoc()->getText(owner_->getText(index_));
		DBXML_ASSERT(text);
		text_.set(text, true);
	}
	return text;
}

const xmlch_t *
NsDomText::getNsNodeValue() const
{
	uint32_t type = nsTextType(type_);
	switch (type) {
	case NS_TEXT:
	case NS_COMMENT:
	case NS_CDATA:
	case NS_SUBSET:
		return _getText();
	case NS_PINST: {
		if(value_.get() == 0) {
			const xmlch_t *val = _getText();
			while (*val++); // get past NULL
			value_.set(val, false);
		}
		return value_.get();
	}
	case NS_ENTSTART:
	case NS_ENTEND:
		return _getText();
	default:
		DBXML_ASSERT(0);
		break;
	}
	return 0;
}

const xmlch_t *
NsDomText::getNsNodeName() const
{
	uint32_t type = nsTextType(type_);
	switch (type) {
	case NS_TEXT:
	case NS_SUBSET:
		return _nsDomTextName;
	case NS_COMMENT:
		return _nsDomCommentName;
	case NS_CDATA:
		return _nsDomCdataName;
	case NS_PINST: {
		return _getText(); // PI target is name
	}
	case NS_ENTSTART:
	case NS_ENTEND:
		return 0;
	default:
		DBXML_ASSERT(0);
		break;
	}
	return 0;
}

const NsNid 
NsDomText::getNodeId() const
{
	return owner_->getNid();
}

bool
NsDomText::nsIgnorableWhitespace() const
{
	DBXML_ASSERT(owner_);
	if (owner_->getTextEntry(index_)->te_type
	    & NS_IGNORABLE)
		return true;
	return false;
}

bool NsDomText::isChildText() const
{
	int32_t firstChild = owner_->getFirstTextChildIndex();
	DBXML_ASSERT(firstChild >= 0);
	return (index_ >= firstChild);
}

//
// NsDomAttr implementation
//
NsDomAttr::NsDomAttr(const NsNode *owner, NsDoc *doc, int index)
	: NsDomNode(doc),
	  owner_(owner),
	  index_(index),
	  lname_(0)
{
}

NsDomAttr::NsDomAttr(const NsDomAttr &other)
	: NsDomNode(other.getNsDoc()),
	  owner_(*other.owner_),
	  index_(other.index_),
	  lname_(0)
{
}

NsDomNode *NsDomAttr::duplicate() const
{
	return new NsDomAttr(*owner_, doc_, index_);
}

void NsDomAttr::refreshNode(OperationContext &oc,
			    bool forWrite)
{
	DBXML_ASSERT(doc_ && doc_->getDocDb());
	owner_ = NsFormat::fetchNode(getNodeId(),
				     doc_->getDocID(),
				     *doc_->getDocDb(),
				     oc, forWrite);
}

const xmlch_t *
NsDomAttr::getNsBaseUri(const xmlch_t *base) const
{
	NsDomElement elem(*owner_, getNsDoc());
        return elem.getNsBaseUri(base);
}

const xmlbyte_t *
NsDomAttr::getNsLocalName8() const
{
	DBXML_ASSERT(owner_);
	nsName_t *aname = owner_->getAttrName(index_);
	return (const xmlbyte_t*)aname->n_text.t_chars;
}

const xmlbyte_t *
NsDomAttr::getNsValue8() const
{
	DBXML_ASSERT(owner_);
	nsName_t *aname = owner_->getAttrName(index_);
	const xmlbyte_t *val = (const xmlbyte_t*)aname->n_text.t_chars;
	while(*(val++)); // get past null to value
	return val;
}

const xmlbyte_t *
NsDomAttr::getNsUri8() const
{
	DBXML_ASSERT(owner_);
	int32_t uriIndex = owner_->attrUri(index_);
	if(uriIndex != NS_NOURI) {
		// NOTE: the string returned here is only valid
		// unti the OperationContext in the _document is
		// reused.  This is safe (for now), as the only caller
		// of this is just using the return value for a
		// string comparison
		return (const xmlbyte_t *) getNsDoc()->
			getStringForID(uriIndex);
	}
	return 0;
}

const xmlbyte_t *
NsDomAttr::getNsPrefix8() const
{
	DBXML_ASSERT(owner_);
	int32_t pfxIndex = owner_->attrNamePrefix(index_);
	if(pfxIndex != NS_NOPREFIX) {
		// NOTE: the string returned here is only valid
		// unti the OperationContext in the _document is
		// reused.  This is safe (for now), as the only caller
		// of this is just using the return value for a
		// string comparison
		return (const xmlbyte_t *) getNsDoc()->
			getStringForID(pfxIndex);
	}
	return 0;
}

void
NsDomAttr::_getName() const
{
	DBXML_ASSERT(owner_);
	nsName_t *aname = owner_->getAttrName(index_);
	const xmlch_t *qname = getNsDoc()->
		getQname(aname);
	DBXML_ASSERT(qname);
	name_.set(qname, true);
	lname_ = qname;
	if (aname->n_prefix != NS_NOPREFIX)
		while (*lname_++ != xmlchColon);
	const xmlch_t *val = lname_;
	while(*(val++)); // get past null to value
	value_.set(val, false);
}

const xmlch_t *
NsDomAttr::getNsNodeValue() const
{
	if (value_.get() == 0) {
		_getName();
		DBXML_ASSERT(value_.get());
	}
	return value_.get();
}

const xmlch_t *
NsDomAttr::getNsNodeName() const
{
	if (name_.get() == 0) {
		_getName();
		DBXML_ASSERT(name_.get() && lname_);
	}
	return name_.get();
}

const xmlch_t *
NsDomAttr::getNsLocalName() const
{
	if (lname_ == 0) {
		_getName();
		DBXML_ASSERT(name_.get() && lname_);
	}
	return lname_;
}

const xmlch_t *
NsDomAttr::getNsPrefix() const
{
	if(prefix_.get() == 0) {
		DBXML_ASSERT(owner_);
		nsName_t *aname =
			owner_->getAttrName(index_);
		if (aname->n_prefix != NS_NOPREFIX)
			setString16(prefix_,
				    getNsDoc(),
				    aname->n_prefix);
	}
	return prefix_.get();
}

const xmlch_t *
NsDomAttr::getNsUri() const
{
	if(uri_.get() == 0 && owner_) {
		int32_t uriIndex = owner_->attrUri(index_);
		if (uriIndex != NS_NOURI) {
			setString16(uri_, getNsDoc(),
				    uriIndex);
		}
	}
	return uri_.get();
}

const NsNid
NsDomAttr::getNodeId() const
{
	return owner_ ? owner_->getNid() : NsNid();
}

NsDomElement *
NsDomAttr::getNsParentNode()
{
	return new NsDomElement(*owner_, getNsDoc());
}

//
// utility functions
//


// node identity comparison is not just pointer equality
// Also check:
//  o container and doc id (isSameDocument())
//  o node id
//  o index if element if attr or text
//
bool
NsDomNode::operator==(const NsDomNode &other) const
{
	if (this == &other)
		return true;
	bool ret = false;
	// do the more thorough comparison that doesn't assume
	// pointer identity.  Use comparisons more likely to fail first
	NsNodeType_t type = getNsNodeType();
	NsNodeType_t otype = other.getNsNodeType();
	if (type == otype) {
		const NsNid &nid = getNodeId();
		const NsNid &onid = other.getNodeId();
		if (!nid.isNull() && !onid.isNull() &&
		    (nid == onid)) {
			if ((type == nsNodeElement) || (type == nsNodeDocument))
				ret = true;
			else if (getIndex() == other.getIndex())
				ret = true;
		}
	}
	if (ret && getNsDoc()->isSameDocument(other.getNsDoc()))
		return true;
	return false;
}
