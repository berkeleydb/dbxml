//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DATABASENODEVALUE_H
#define	__DATABASENODEVALUE_H

/*
 * An implementation of RawNodeValue (Value) that does not maintain
 * state in the form of references to nodes and documents.
 * It holds container, document and node IDs and resolves these
 * in the context of an XmlResults object when necessary.
 *
 * Actual nodes may be directly in container databases or in temporary
 * databases associated with Results.
 *
 * For the time being (2.4), this class is ONLY used by the Java API.
 * If it weren't for Windows export issues, it would reside entirely
 * in Java-only code (and probably *not* implement NodeValue).
 *
 */
#include "RawNodeValue.hpp"
#include <dbxml/XmlResults.hpp>
#include <dbxml/XmlPortability.hpp>
#include "nodeStore/NsDoc.hpp"

namespace DbXml
{

class DbWrapper;
class DictionaryDatabase;
	
class DatabaseNodeValue : public RawNodeValue {
public:
	DatabaseNodeValue();
	DatabaseNodeValue(XmlValue &value, XmlResults &results);  // ctor from NodeValue
	DatabaseNodeValue(const void *data, XmlResults &results); // ctor from dumped content
	DatabaseNodeValue(const DatabaseNodeValue &other);
	virtual ~DatabaseNodeValue() {}

	virtual double asNumber() const;
	virtual std::string asString() const;
	virtual bool asBoolean() const;
	virtual const XmlDocument &asDocument() const;
	virtual XmlEventReader &asEventReader() const;
	virtual bool equals(const Value &v) const;

	std::string getNodeName() const;
	std::string getNodeValue() const;
	std::string getNamespaceURI() const;
	std::string getPrefix() const;
	std::string getLocalName() const;
	XmlValue getParentNode() const;
	XmlValue getFirstChild() const { return getChild(true); }
	XmlValue getLastChild() const { return getChild(false); }
	XmlValue getPreviousSibling() const { return getSibling(false); }
	XmlValue getNextSibling() const { return getSibling(true); }
	XmlResults getAttributes() const;
 	XmlValue getOwnerElement() const;

	virtual const DatabaseNodeValue *getDatabaseNodeValueObject() const { return this; }
	virtual DbXmlNodeImpl *getNodeImpl(DynamicContext *context, bool validate);

	// Methods used by Java API
	virtual int dumpValue(void *&data); // returns data and size of data
	virtual CacheDatabaseMinder *getDBMinder() const;
protected:
	/// Return an opaque handle to the node
	void initIndexEntryFromThis(IndexEntry &ie) const;
	int getContainerId() const { return cid_; }
private:
	// internal methods
	void copyContext(int cid, const DocID &did, XmlResults &results,
			 XmlDocument &xdoc, XmlResults *returnResults);
	DbWrapper *getDocDB() const;
	DictionaryDatabase *getDictDB() const;
	void getNsDomNode() const;
	void loadValue(const void *data);
	int marshal(void *&data);
	DatabaseNodeValue *makeRelative(const NsNid &nid, short type, int index) const;
	XmlValue getChild(bool isFirst) const;
	XmlValue getSibling(bool isNext) const;
private:
	// state
	DocID did_;     // doc ID
	int cid_;       // container ID
	int index_;     // index for attribute or text types
	NsNidWrap nid_; // nid
	mutable NsDomNodeRef node_;
	mutable NsDoc doc_;
	Node::Ptr nodePtr_;
};

}

#endif
