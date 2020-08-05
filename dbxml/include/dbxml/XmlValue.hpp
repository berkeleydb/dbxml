//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLVALUE_HPP
#define	__XMLVALUE_HPP

#include "XmlPortability.hpp"
#include "DbXmlFwd.hpp"
#include <string>

namespace DbXml
{

class Value;
class XmlEventReader;
	
class DBXML_EXPORT XmlValue
{
public:
	// if this enum changes, you MUST change dist/swig/dbxml.i
	enum NodeType {
		ELEMENT_NODE = 1,
		ATTRIBUTE_NODE = 2,
		TEXT_NODE = 3,
		CDATA_SECTION_NODE = 4,
		ENTITY_REFERENCE_NODE = 5,
		ENTITY_NODE = 6,
		PROCESSING_INSTRUCTION_NODE = 7,
		COMMENT_NODE = 8,
		DOCUMENT_NODE = 9,
		DOCUMENT_TYPE_NODE = 10,
		DOCUMENT_FRAGMENT_NODE = 11,
		NOTATION_NODE = 12
	};

	// if this enum changes, you MUST change dist/swig/dbxml.i
	enum Type
	{
		NONE               = 0,
		NODE               = 3,

		/// abstract type (separates type ids for DB XML and XML Schema atomic types)
		ANY_SIMPLE_TYPE    = 10,

		ANY_URI            = 11,
		BASE_64_BINARY     = 12,
		BOOLEAN            = 13,
		DATE               = 14,
		DATE_TIME          = 15,
		/// not a built-in primitive type
		DAY_TIME_DURATION  = 16,
		DECIMAL            = 17,
		DOUBLE             = 18,
		DURATION           = 19,
		FLOAT              = 20,
		G_DAY              = 21,
		G_MONTH            = 22,
		G_MONTH_DAY        = 23,
		G_YEAR             = 24,
		G_YEAR_MONTH       = 25,
		HEX_BINARY         = 26,
		NOTATION           = 27,
		QNAME              = 28,
		STRING             = 29,
		TIME               = 30,
		/// not a built-in primitive type
		YEAR_MONTH_DURATION= 31,

		/// untyped atomic data
		UNTYPED_ATOMIC     = 32,
		BINARY             = 40
	};

	/** @name Constructors */
	XmlValue();
	XmlValue(const std::string &v);
	XmlValue(const char *v);
	XmlValue(double v);
	XmlValue(bool v);
	XmlValue(const XmlDocument &document);
	XmlValue(Type type, const std::string &v);
	XmlValue(const std::string &typeURI, const std::string &typeName,
		 const std::string &v);
	XmlValue(Type type, const XmlData &dbt);

	/** @name For Reference Counting */
	XmlValue(const XmlValue &);
	XmlValue &operator=(const XmlValue &);
	virtual ~XmlValue();

	/** @name DOMNode replicated methods */

	/// Returns the node name as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	std::string getNodeName() const;
	/// Returns the node value as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	std::string getNodeValue() const;
	/// Returns the namespace URI as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	std::string getNamespaceURI() const;
	/// Returns the prefix as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	std::string getPrefix() const;
	/// Returns the local name as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	std::string getLocalName() const;
	/// Returns the node type as per the DOM Spec or throws an
	/// XMLExpection if this value is not a node
	short getNodeType() const;
	/// Returns the parent node if this value is a node 
	/// XMLExpection if this value is not a node
	XmlValue getParentNode() const;
	/// Returns the first child node if this value is a node 
	/// XMLExpection if this value is not a node
	XmlValue getFirstChild() const;
	/// Returns the last child node if this value is a node 
	/// XMLExpection if this value is not a node
	XmlValue getLastChild() const;
	/// Returns the previous sibling node if this value is a node 
	/// XMLExpection if this value is not a node
	XmlValue getPreviousSibling() const;
	/// Returns the next sibling node if this value is a node 
	/// XMLExpection if this value is not a node
	XmlValue getNextSibling() const;
	/// Returns the attribute nodes if this value is a node 
	/// XMLExpection if this value is not a node
	XmlResults getAttributes() const;
	/// Returns the owner element if this value is a attribute 
	/// XMLExpection if this value is not a attribute
 	XmlValue getOwnerElement() const;

	/** @name Information Methods */

	/// What type is the Value.
	Type getType() const;

	/// Returns the specific type URI
	std::string getTypeURI() const;
	/// Returns the specific type name
	std::string getTypeName() const;

	/// Does the value have no value.
	bool isNull() const;
	/// Is the value of the given Type.
	bool isType(Type type) const;

	bool isNumber() const;
	bool isString() const;
	bool isBoolean() const;
	bool isBinary() const;
	bool isNode() const;

	/** @name Accessor Methods */

	/// Return the value as a Number.
	double asNumber() const;
	/// Return the value as a String.
	std::string asString() const;
	/// Return the value as a Boolean.
	bool asBoolean() const;
	/// Return the value as binary
	XmlData asBinary() const;
	/// Return the value as a Document.
	const XmlDocument &asDocument() const;
	/// Return the value as XmlEventReader (must be a Node)
	XmlEventReader &asEventReader() const;
	/// Return the (Node) value as a node handle
	std::string getNodeHandle() const;

	/** @name Equals Methods */

	/// Compare two values for equality.
	bool operator==(const XmlValue &v) const;
	/// Compare two values for equality.
	bool equals(const XmlValue &v) const;

	/** @name Private Methods (for internal use) */
	// @{

	XmlValue(Value *value);
	operator Value*() const
	{
		return value_;
	}
	operator Value&() const
	{
		return *value_;
	}

	//@}

private:
	Value *value_;
};

}

#endif
