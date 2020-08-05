//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSDOM_HPP
#define __DBXMLNSDOM_HPP
/*
 * Node storage "DOM" classes.  These are primitive
 * classes that encapsulate NsNode.
 *
 */
#include "NsNode.hpp"
#include "NsUtil.hpp"
#include "NsDoc.hpp"
#include "../ReferenceCounted.hpp"

namespace DbXml
{
// forward
class NsDomElement;
class NsDomText;
class NsDomAttr;
class OperationContext;
	
typedef enum {
	nsNodeElement = 1,
	nsNodeAttr = 2,
	nsNodeText = 3,
	nsNodeCDATA = 4,
	nsNodeEntStart = 5,
	nsNodeEntEnd = 6,
	nsNodePinst = 7,
	nsNodeComment = 8,
	nsNodeDocument = 9
} NsNodeType_t;

class NsDomNode : public ReferenceCounted {
public:
	NsDomNode(NsDoc *doc) :
		doc_(doc) {}
	NsDoc *getNsDoc() const { return doc_; }
	virtual ~NsDomNode() {}
	// content/navigation
	virtual const xmlch_t *getNsNodeName() const = 0;
	virtual const xmlch_t *getNsNodeValue() const = 0;
	virtual NsDomElement *getNsParentNode() = 0;
	// child/sibling navigation overridden when necessary
	virtual NsDomNode *getNsFirstChild() { return 0; }
	virtual NsDomNode *getNsLastChild() { return 0; }
	virtual NsDomNode *getNsNextSibling() { return 0; }
	virtual NsDomNode *getNsPrevSibling() { return 0; }
	virtual NsNodeType_t getNsNodeType() const = 0;
	virtual bool getNsIsEntityType() const { return false; }
	virtual bool isTextType() const { return false; }
	
	// namespaces
	virtual const xmlch_t *getNsPrefix() const { return 0; }
	virtual const xmlch_t *getNsUri() const { return 0; }
	virtual const xmlch_t *getNsLocalName() const { return 0; }
	virtual const xmlch_t *getNsBaseUri(const xmlch_t *base) const { return 0; }	

	virtual const xmlbyte_t *getNsNodeName8() const { return 0; }
	virtual const xmlbyte_t *getNsLocalName8() const { return 0; }
	virtual const xmlbyte_t *getNsUri8() const { return 0; }
	virtual const xmlbyte_t *getNsPrefix8() const { return 0; }

	virtual const NsNid getNodeId() const = 0;
	virtual const NsNid getLastDescendantNid() const {
		const NsFullNid *fn = const_cast<NsDomNode*>(this)->
			getNsNode()->getLastDescendantNidOrSelf();
		return NsNid(fn->getBytes());
	}
	virtual int32_t getNsLevel() const = 0; // attrs return level of owner
	virtual const NsNode *getNsNode() const = 0;

	virtual int32_t getIndex() const { return -1; }

	// duplicate this node (used by DbXmlNodeImpl only right now
	// This is simpler than rewriting that code to use NsNode or
	// other reference counting mechanism.
	virtual NsDomNode *duplicate() const = 0;
	// re-read the node from the database, if possible
	virtual void refreshNode(OperationContext &oc, bool forWrite) {}

	// Node comparison
	bool operator==(const NsDomNode &other) const;
	bool operator!=(const NsDomNode &other) const {
		return ((*this == other) == false);
	}
protected:
	NsDoc *doc_;
};

/**
 * NsDomElement -- add NsNode and element tree links, as
 * well as NsDomNav child pointers, and an element name cache
 */
class NsDomElement : public NsDomNode {
public:
	NsDomElement(NsNode *node, NsDoc *doc);
	virtual ~NsDomElement();
	//
	// methods from NsDomNode
	//
	virtual const xmlch_t *getNsNodeName() const;
	virtual const xmlch_t *getNsLocalName() const;
	virtual const xmlch_t *getNsPrefix() const;
	virtual const xmlch_t *getNsUri() const;
	virtual const xmlch_t *getNsNodeValue() const;
	virtual const xmlch_t *getNsBaseUri(const xmlch_t *base) const;
	int32_t getNsPrefixID() const {
		return node_->namePrefix();
	}
	int32_t getNsUriID() const {
		return node_->uriIndex();
	}
	bool hasUri() const {
		return node_->hasUri();
	}
	bool hasPrefix() const {
		return node_->hasNamePrefix();
	}
	virtual const xmlbyte_t *getNsLocalName8() const;
	virtual const xmlbyte_t *getNsUri8() const;
	virtual const xmlbyte_t *getNsPrefix8() const;

	virtual NsDomNode *getNsFirstChild();
	virtual NsDomNode *getNsLastChild();
	virtual NsNodeType_t getNsNodeType() const;
	virtual NsDomElement *getNsParentNode();
	virtual NsDomNode *getNsNextSibling();
	virtual NsDomNode *getNsPrevSibling();
	virtual const xmlch_t *getNsTextContent() const;
	// NsDomNode extensions
	virtual const NsNid getNodeId() const;
	virtual int32_t getNsLevel() const;
	virtual const NsNode *getNsNode() const { return node_.get(); }

	virtual NsDomNode *duplicate() const;
	virtual void refreshNode(OperationContext &oc, bool forWrite);
	//
	// methods introduced in NsDomElement
	//
	bool isDocumentNode() const {
		return node_->isDoc();
	}

	NsDomText *getNsTextNode(int index);
	NsDomAttr *getNsAttr(int index);
	int getNumAttrs() const {
		return node_->numAttrs();
	}
		
	// TBD GMF Hack to indicate that a constructed element
	// is the "root" of its tree -- i.e. don't navigate to
	// the parent (document) node.  A better solution is to
	// allow trees without a document node in node storage
	void setIsRoot();
	bool isRoot() const;

	//
	// Navigational methods that operate on element nodes
	// vs nodes which may be text.  These will fetch
	// persistent node, if necessary
	NsDomElement *getElemParent();
	NsDomElement *getElemFirstChild();
	NsDomElement *getElemLastChild();
	NsDomElement *getElemPrev();
	NsDomElement *getElemNext();
	bool hasElemNext() const {
		return node_->hasNext();
	}
	NsNode * getParentNode() const;
protected:
	
	/// utility -- internal
	void _getName() const;
protected:
	NsNodeRef  node_;

	// Names, created/cached on demand
	mutable NsString qname_;
	mutable NsString textContent_; // getTextContent result
	mutable const xmlch_t *lname_;  // points into, or at _qname string
	mutable NsString baseUri_;
	mutable NsString uri_;
	mutable NsString prefix_;
};

//
// NsDomText
//
class NsDomText : public NsDomNode {
public:
	NsDomText(NsNode *owner,
		  NsDoc *doc,
		  int32_t index);
	//
	// methods from NsDomNode
	//
	virtual const xmlch_t *getNsNodeName() const;
	virtual const xmlch_t *getNsNodeValue() const;
	virtual const xmlbyte_t *getNsNodeName8() const;
	virtual const xmlbyte_t *getNsValue8() const;

	virtual NsNodeType_t getNsNodeType() const;
	virtual bool getNsIsEntityType() const;
	virtual bool isTextType() const { return true; }
	virtual NsDomElement *getNsParentNode();
	virtual NsDomNode *getNsNextSibling();
	virtual NsDomNode *getNsPrevSibling();
	virtual int32_t getNsLevel() const;
	virtual const NsNid getNodeId() const;
	virtual const NsNode *getNsNode() const { return owner_.get(); }
	virtual int32_t getIndex() const { return index_; }
	virtual NsDomNode *duplicate() const;
	virtual void refreshNode(OperationContext &oc, bool forWrite);
	void setIndex(int32_t index) { index_ = index; }
	
	uint32_t getNsTextType() const { return type_; }
	bool nsIgnorableWhitespace() const;
	bool isChildText() const;
private:
	const xmlch_t *_getText() const;
protected:
	NsNodeRef owner_; // owning node: may be parent or sibling
	uint32_t type_;
	int32_t index_;
	bool ownerIsParent_;
	mutable NsString text_;
	mutable NsString value_; ///< For processing instructions
};

//
// NsDomAttr -- a single attribute
//
class NsDomAttr : public NsDomNode {
public:
	NsDomAttr(const NsNode *owner, NsDoc *doc, int index);
	NsDomAttr(const NsDomAttr &other);
	virtual ~NsDomAttr() {}
	//
	// methods from NsDomNode
	//
	virtual const xmlch_t *getNsNodeName() const;
	virtual const xmlch_t *getNsLocalName() const;
	virtual const xmlch_t *getNsPrefix() const;
	virtual const xmlch_t *getNsUri() const;
	virtual const xmlch_t *getNsNodeValue() const;
	virtual const xmlch_t *getNsBaseUri(const xmlch_t *base) const;
	
	virtual const xmlbyte_t *getNsLocalName8() const;
	virtual const xmlbyte_t *getNsUri8() const;
	virtual const xmlbyte_t *getNsPrefix8() const;
	virtual const xmlbyte_t *getNsValue8() const;
	
	virtual NsDomElement *getNsParentNode();
	virtual NsNodeType_t getNsNodeType() const {
		return nsNodeAttr;
	}
	virtual int32_t getNsLevel() const {
		if (owner_) return owner_->getLevel();
		else return -1;
	}
	
	virtual const NsNode *getNsNode() const { return owner_.get(); }
	virtual int32_t getIndex() const {
		return index_;
	}
	virtual const NsNid getNodeId() const;
	virtual NsDomNode *duplicate() const;
	virtual void refreshNode(OperationContext &oc, bool forWrite);
	int32_t getNsPrefixID() const;
	int32_t getNsUriID() const;
	bool hasUri() const {
		return owner_->attrHasUri(index_);
	}
	bool hasPrefix() const {
		return (owner_->attrNamePrefix(index_) !=
			NS_NOPREFIX);
	}

private:
	void _getName() const;
protected:
	NsNodeRef owner_;
	int32_t index_;
	mutable NsString name_;
	mutable const xmlch_t *lname_;
	mutable NsString value_;
	mutable NsString prefix_;
	mutable NsString uri_;
};

}

#endif
