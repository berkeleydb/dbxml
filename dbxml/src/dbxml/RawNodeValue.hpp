//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __RAWNODEVALUE_H
#define	__RAWNODEVALUE_H

/*
 * An interface of NodeValue (Value) for classes that do not maintain
 * state in the form of references to nodes and documents.
 *
 * For the time being (2.4), this class is ONLY used by the Java API.
 * If it weren't for Windows export issues, it would reside entirely
 * in Java-only code (and probably *not* implement NodeValue).
 *
 */
#include "Value.hpp"
#include <dbxml/XmlResults.hpp>
#include <dbxml/XmlPortability.hpp>

namespace DbXml
{

class DatabaseNodeValue;
class AtomicNodeValue;
class CacheDatabaseMinder;

static const xmlbyte_t DatabaseNodeType = '0'; //The node is stored in a database
static const xmlbyte_t AtomicNodeType  = '1';   //The node is stored as a prefix, local name, and value
	
class DBXML_EXPORT RawNodeValue : public NodeValue {
public:
	static RawNodeValue *create(const void *data, XmlResults &results);
	static RawNodeValue *create(XmlValue &value, XmlResults &results);

	RawNodeValue();
	RawNodeValue(XmlResults &results);
	virtual ~RawNodeValue();

	virtual std::string getTypeURI() const;
	virtual std::string getTypeName() const;
	virtual short getNodeType() const { return type_; }

	virtual const RawNodeValue *getRawNodeValueObject() const { return this; }	
	virtual const DatabaseNodeValue *getDatabaseNodeValueObject() const { return 0; }
	virtual const AtomicNodeValue *getAtomicNodeValueObject() const { return 0; }
	virtual DbXmlNodeImpl *getNodeImpl(DynamicContext *context, bool validate) { return 0; }

	// Methods used by Java API
	virtual int dumpValue(void *&data) = 0; // returns data and size of data
	void setDocument(XmlDocument &doc) { xdoc_ = doc; }
	void setReturnResults(XmlResults *results) { returnResults_ = results; }
	XmlResults *getReturnResults() { return returnResults_; }
	Transaction *getTransaction() const;
	virtual CacheDatabaseMinder *getDBMinder() const { return 0; }
protected:
	// state
	short type_; // node type
	void *data_;
	int dataSize_;
	mutable XmlDocument xdoc_;
	mutable XmlResults results_;
	XmlResults *returnResults_; // Set the Java XmlResults to this pointer
};

}

#endif
