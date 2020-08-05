//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

/*
 * NsRawNode.hpp
 *
 * A class that knows how to retrive information and state
 * from the raw, marshaled node format (tied to a format version)
 * The assumption is that an instance of this object is used
 * in iterator-style access, where the actual body of the node
 * changes.  It caches offsets to known/resolved locations.
 *
 * It is a requirement that memory allocation be either eliminated
 * or kept to a bare minimum.
 *
 * Pattern of usage:
 *  o an iterator will construct a single instance of this object, most
 *    likely as a member variable.
 *  o an instance has current container and document IDs.  Container ID
 *    is explicitly set by the owner (setContainerID()).  Document ID
 *    is pulled out of the current iteration state (see below)
 *  o as it iterates, it will call setNode() with the key and data Dbts
 *    for the "current" position.
 */

#ifndef __DBXMLNSRAWNODE_HPP
#define __DBXMLNSRAWNODE_HPP

#include "NsFormat.hpp"
#include "NsNode.hpp"
#include "../query/NodeInfo.hpp"
#include "NsNid.hpp"
#include "../DocID.hpp"

namespace DbXml
{

class DbXmlDbt;

class NsRawNode {
public:
	// construction/initialization
	NsRawNode();
	void clear();
	void setNode(const DbXmlDbt &key, const DbXmlDbt &data);
	void setContainerID(int cid) {
		cid_ = cid;
	}
	bool isNull() const { return (data_ == 0); }
	// accessors
	NodeInfo::Type getType() {
		return NodeInfo::ELEMENT; // TBD support attrs and others
	}
	int getContainerID() { return cid_; }
	const DocID getDocID() const { return did_; }
	const NsNid getNodeID() const { return nid_; }
	const NsNid getLastDescendantID();

	u_int32_t getNodeLevel() {
		initialize();
		return level_;
	}
	int32_t getURIIndex() {
		initialize();
		return uri_;
	}

	int32_t getPrefixIndex() {
		initialize();
		return pfx_;
	}
	const unsigned char *getNodeName() {
		initialize();
		return name_;
	}

	u_int32_t getNumAttrs() {
		initialize();
		return nattrs_;
	}

	u_int32_t getNumText() {
		initialize();
		return ntext_;
	}

	bool hasTextChild() {
		initialize();
		return ((flags_ & NS_HASTEXTCHILD) != 0);
	}

	bool hasText() {
		initialize();
		return ((flags_ & NS_HASTEXT) != 0);
	}
	
	bool hasElemChild() {
		initialize();
		return ((flags_ & NS_HASCHILD) != 0);
	}

	bool hasURI() {
		initialize();
		return ((flags_ & NS_HASURI) != 0);
	}

	u_int32_t getNumChildText(); // requires seeking

	bool hasNext() {
		initialize();
		return ((flags_ & NS_HASNEXT) != 0);
	}

	u_int32_t getFlags() {
		initialize();
		return flags_;
	}
		
	// handle attributes
	const unsigned char *getNextAttr(
		const unsigned char *current,
		nsAttr_t *attr, int index = -1);
	int getAttrIndex() {
		DBXML_ASSERT(unmarshaled_);
		return attrIndex_;
	}
	// valid for text types (text, comment, PI)
	nsTextEntry_t *getTextEntry(
		nsTextEntry_t *entry, int index);

	u_int32_t getDataSize() { return dataSize_; }
	
	static bool isRootOrMetaData(DbXmlDbt &key);
private:
	void initialize() {
		if (!unmarshaled_) initialize_internal();
	}
	void initialize_internal();
private:
	NsNid nid_;
	const unsigned char *data_;
	DocID did_;
	int cid_;
	// cached values
	NsNid lastDescendant_;
	u_int32_t flags_;
	const unsigned char *name_;
	int32_t uri_;
	int32_t pfx_;
	u_int32_t level_;
	bool unmarshaled_;
	u_int32_t nattrs_;
	u_int32_t ntext_;
	u_int32_t nchildtext_;
	int attrIndex_;
	int textIndex_;
	u_int32_t dataSize_;
};

class NsRawNodeIndexNodeInfo : public IndexNodeInfo {
public:
	NsRawNodeIndexNodeInfo() : node_(0) {}
	NsRawNodeIndexNodeInfo(NsRawNode *node) : node_(node) {}
	const NsNid getNodeID() { return node_->getNodeID(); }
	u_int32_t getNodeLevel() { return node_->getNodeLevel(); }
	size_t getNodeDataSize() { return node_->getDataSize(); }
	NsRawNode *node_;
};

class NsRawNodeNodeInfo : public NodeInfo {
public:
	NsRawNodeNodeInfo(NsRawNode *node) : node_(node) {}

	virtual Type getType() const { return node_->getType(); }
	virtual int getContainerID() const { return node_->getContainerID(); }
	virtual DocID getDocID() const { return node_->getDocID(); }
	virtual const NsNid getNodeID() const { return node_->getNodeID(); }
	virtual const NsNid getLastDescendantID() const { return node_->getLastDescendantID(); }
	virtual u_int32_t getNodeLevel() const { return node_->getNodeLevel(); }
	virtual u_int32_t getIndex() const { return 0; }
	virtual bool isLeadingText() const { return false; }

	NsRawNode *node_;
};

}
#endif
