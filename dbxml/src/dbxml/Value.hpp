//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __VALUE_H
#define	__VALUE_H

#include <map>
#include <string>
#include <dbxml/XmlException.hpp>
#include <dbxml/XmlValue.hpp>
#include "ReferenceCounted.hpp"
#include "dbxml/XmlDocument.hpp"
#include "Syntax.hpp"
#include "dbxml/XmlValue.hpp"
#include "dataItem/DbXmlNodeImpl.hpp"

#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/items/Item.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/items/AnyAtomicType.hpp>
#include <dbxml/XmlPortability.hpp>
namespace DbXml
{
class NsDomNode;
class NodeValue;
class RawNodeValue;
class DbXmlNodeValue;
	
typedef std::vector<XmlValue> XmlValueVector;

class DBXML_EXPORT Value : public ReferenceCounted
{
public:
	static Value *create(const AnyAtomicType::Ptr &atom, DynamicContext *context);
	static Value *create(const Node::Ptr &item, bool lazyDocs);
	static Value *create(XmlValue::Type type, const std::string &v,
			     bool validate = false);
	static Value *create(XmlValue::Type type, const XmlData &dbt,
			     bool validate = false);
	static Item::Ptr convertToItem(const Value *value, DynamicContext *context, bool validate);
	static Syntax::Type getSyntaxType(const AnyAtomicType::Ptr &atom);
	
	/// Abstract Base Class.
	explicit Value(XmlValue::Type t) : t_(t)
	{}
	virtual ~Value()
	{}
	/// What type is the Value: Document, Variable or one of the simple types
	virtual XmlValue::Type getType() const
	{
		return t_;
	}
	virtual std::string getTypeURI() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "XmlValue has no type URI");
	}
	virtual std::string getTypeName() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "XmlValue has no type name");
	}
	/// What syntax type is the Value
	virtual Syntax::Type getSyntaxType() const
	{
		return Syntax::NONE;
	}
 	/// Return the value as a Number.
	virtual double asNumber() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to Number");
		return 0;
	}
	/// Return the value as a String.
	virtual std::string asString() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to String");
		return 0;
	}
	/// Return the value as a Boolean.
	virtual bool asBoolean() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to Boolean");
		return false;
	}
	/// Return the value as a Binary.
	virtual const DbXmlDbt & asBinary() const
	{
		throw XmlException(
			XmlException::INVALID_VALUE, "Can't convert XmlValue to Binary");
	}
	/// Return the value as a Document.
	virtual const XmlDocument &asDocument() const;

	/// Return the value as XmlEventReader
	virtual XmlEventReader &asEventReader() const 
	{
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Can't convert XmlValue to XmlEventReader");
	}

	/// Perform type validation of the node
	virtual void validate() const {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Can't validate an XmlValue that is not an atomic type");
	}

	/// Compare two values for equality.
	virtual bool equals(const Value &v) const = 0;

	// what is the underlying implementation?
	virtual const RawNodeValue *getRawNodeValueObject() const { return NULL; }
	virtual const DbXmlNodeValue *getDbXmlNodeValueObject() const { return NULL; }
	
	// Lexical representations of special numerical values
	static const std::string NaN_string;
	static const std::string PositiveInfinity_string;
	static const std::string NegativeInfinity_string;
	
protected:
	/// Assign a specific atomic type to the Value
	virtual void setType(XmlValue::Type type) {
		// only allow simple types
		if(type < XmlValue::ANY_SIMPLE_TYPE) {
			throw XmlException(
				XmlException::INVALID_VALUE,
				"The variable value must be of an atomic type.");
		}
		t_ = type;
	}

private:
	// no need for copy and assignment
	Value(const Value &);
	Value &operator = (const Value &);
	XmlValue::Type t_;
};

/**
 * NodeValue adds additional virtual behavior to Value
 */
class DBXML_EXPORT NodeValue : public Value
{
public:
	NodeValue() : Value(XmlValue::NODE) {}
	/// Return an opaque handle to the node (not virtual)
	std::string getNodeHandle() const;
	/// Returns the node name as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getNodeName() const = 0;
	/// Returns the node value as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getNodeValue() const = 0;
	/// Returns the namespace URI as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getNamespaceURI() const = 0;
	/// Returns the prefix as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getPrefix() const = 0;
	/// Returns the local name as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual std::string getLocalName() const = 0;
	/// Returns the node type as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	virtual short getNodeType() const = 0;
	/// Returns the parent node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getParentNode() const = 0;
	/// Returns the first child node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getFirstChild() const = 0;
	/// Returns the last child node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getLastChild() const = 0;
	/// Returns the previous sibling node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getPreviousSibling() const = 0;
	/// Returns the next sibling node if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlValue getNextSibling() const = 0;
	/// Returns the attribute nodes if this value is a node 
	/// XMLExpection if this value is not a node
	virtual XmlResults getAttributes() const = 0;
	/// Returns the owner element if this value is a attribute 
	/// XMLExpection if this value is not a attribute
 	virtual XmlValue getOwnerElement() const = 0;

	virtual DbXmlNodeImpl *getNodeImpl(DynamicContext *context, bool validate) = 0;
	Item::Ptr createNode(DynamicContext *context) const;
protected:
	virtual void initIndexEntryFromThis(IndexEntry &ie) const = 0;
	virtual int getContainerId() const { return 0; }
	void validateNode(DbXmlNodeImpl *node, DynamicContext *context);
};

/**
 * NodeValue based on DbXmlNodeImpl
 */
class DBXML_EXPORT DbXmlNodeValue : public NodeValue
{
public:
	/// Construct the Node value from a node list.
	DbXmlNodeValue(Document *d);
	/// Construct the Node value from a node list.
	DbXmlNodeValue(DbXmlNodeImpl *n, Document *d);
	virtual ~DbXmlNodeValue();

	// Value functions
	virtual std::string getTypeURI() const;
	virtual std::string getTypeName() const;

	/// Converts the Node to a number.
	virtual double asNumber() const;
	/// Converts the number to a string.
	virtual std::string asString() const;
	/// Converts the number to a boolean.
	virtual bool asBoolean() const;
	/// Returns the Node value.
	virtual const XmlDocument &asDocument() const;
	virtual XmlEventReader &asEventReader() const;
	virtual bool equals(const Value &v) const;
	virtual const DbXmlNodeValue *getDbXmlNodeValueObject() const { return this; }
	
	// NodeValue functions
	std::string getNodeName() const;
	std::string getNodeValue() const;
	std::string getNamespaceURI() const;
	std::string getPrefix() const;
	std::string getLocalName() const;
	short getNodeType() const;
	XmlValue getParentNode() const;
	XmlValue getFirstChild() const;
	XmlValue getLastChild() const;
	XmlValue getPreviousSibling() const;
	XmlValue getNextSibling() const;
	XmlResults getAttributes() const;
 	XmlValue getOwnerElement() const;

	const Document *getDocument() const;
	virtual DbXmlNodeImpl *getNodeImpl(DynamicContext *context,
					   bool validate);
	
	/// A "factory" method to create DbXmlNodeValue from NsDomNode *
	static DbXmlNodeValue *makeDbXmlNodeValue(const NsDomNode *node,
						  Document *doc,
						  DbXmlConfiguration *conf = 0);
protected:
	void initIndexEntryFromThis(IndexEntry &ie) const;
	int getContainerId() const;
private:
	// no need for copy and assignment
	DbXmlNodeValue(const DbXmlNodeValue &);
	DbXmlNodeValue & operator = (const DbXmlNodeValue &);

	mutable DbXmlNodeImpl::Ptr n_;
	XmlDocument d_;
};

/**
 * \brief Represents an atomic type
 */
class DBXML_EXPORT AtomicTypeValue : public Value
{
public:
	// Construct the value from native types
	AtomicTypeValue(bool v);
	AtomicTypeValue(double v);
	AtomicTypeValue(const std::string &v);
	AtomicTypeValue(const char *v);
	
	// Contruct the value from a type enumeration
	AtomicTypeValue(XmlValue::Type type, const std::string &v);
	AtomicTypeValue(const std::string &typeURI,
		const std::string &typeName, const std::string &v);
	AtomicTypeValue(XmlValue::Type type, const std::string &typeURI,
		const std::string &typeName, const std::string &v);

	virtual std::string getTypeURI() const
	{
		return typeURI_;
	}
	virtual std::string getTypeName() const
	{
		return typeName_;
	}
	
	/// What syntax type is the Value
	virtual Syntax::Type getSyntaxType() const;
	/// Return the value as a Number.
	virtual double asNumber() const;
	/// Return the value as a String.
	virtual std::string asString() const;
	/// Return the value as a Boolean.
	virtual bool asBoolean() const;
	/// Compare two values for equality.
	virtual bool equals(const Value &v) const;
	/// Perform type validation of the node
	virtual void validate() const;

	/// convert a DB XML type to an XML Schema primitive type name
	static Syntax::Type convertToSyntaxType(XmlValue::Type valueType);
	static const char * getTypeString(Syntax::Type syntaxType);
	static const char * getValueTypeString(XmlValue::Type valueType);
	static XmlValue::Type convertToValueType(Syntax::Type syntaxType);
	
private:
	// no need for copy and assignment
	AtomicTypeValue(const AtomicTypeValue &);
	AtomicTypeValue &operator = (const AtomicTypeValue &);

	void setTypeNameFromEnumeration();

	std::string typeURI_, typeName_;
	std::string value_;
};

/**
 * \brief Represents a binary value
 */
class BinaryValue : public Value
{
public:
	// Construct the value from native types
	BinaryValue();
	BinaryValue(const XmlData &dbt);
	// Allow construction from string
	BinaryValue(const std::string &content);

	virtual ~BinaryValue();
	
	virtual const DbXmlDbt & asBinary() const { return dbt_; }
	/// Compare two values for equality.
	virtual bool equals(const Value &v) const;
private:
	void init(const void *, size_t size);
	DbXmlDbt dbt_;
};
	
}

#endif

