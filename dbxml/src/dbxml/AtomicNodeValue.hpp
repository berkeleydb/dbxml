//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __ATOMICNODEVALUE_H
#define	__ATOMICNODEVALUE_H

/*
 * An implementation of RawNodeValue (Value) that does not maintain
 * state in the form of references to nodes and documents.
 * This is for nodes that are not stored in any databases,
 * such as text or attribute nodes created by xquery constructors.
 *
 * For the time being (2.4), this class is ONLY used by the Java API.
 * If it weren't for Windows export issues, it would reside entirely
 * in Java-only code (and probably *not* implement NodeValue).
 *
 */
#include "RawNodeValue.hpp"
#include <dbxml/XmlResults.hpp>
#include <dbxml/XmlPortability.hpp>

namespace DbXml
{

class DbWrapper;
class DictionaryDatabase;
	
class AtomicNodeValue : public RawNodeValue {
public:
	AtomicNodeValue();
	AtomicNodeValue(XmlValue &value, XmlResults &results);  // ctor from NodeValue
	AtomicNodeValue(const void *data, XmlResults &results); // ctor from dumped content
	AtomicNodeValue(const AtomicNodeValue &other);
	virtual ~AtomicNodeValue();

	virtual double asNumber() const;
	virtual std::string asString() const;
	virtual bool asBoolean() const;
	virtual const XmlDocument &asDocument() const; // note: will throw for now
	virtual XmlEventReader &asEventReader() const;
	virtual bool equals(const Value &v) const;

	std::string getNodeName() const;
	std::string getNodeValue() const;
	std::string getNamespaceURI() const;
	std::string getPrefix() const;
	std::string getLocalName() const;
	XmlValue getParentNode() const;
	XmlValue getFirstChild() const { return XmlValue(); }
	XmlValue getLastChild() const { return XmlValue(); }
	XmlValue getPreviousSibling() const { return XmlValue(); }
	XmlValue getNextSibling() const { return XmlValue(); }
	XmlResults getAttributes() const;
 	XmlValue getOwnerElement() const;

	virtual const AtomicNodeValue *getAtomicNodeValueObject() const { return this; }

	// Methods used by Java API
	virtual int dumpValue(void *&data); // returns data and size of data
protected:
	void initIndexEntryFromThis(IndexEntry &ie) const;

private:
	void loadValue(const void *data);
	int marshal(void *&data);
private:
	//state
	xmlbyte_t *prefix_;
	xmlbyte_t *value_;
	xmlbyte_t *localName_;
};

}

#endif
