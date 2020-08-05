//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlValue.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlData.hpp"
#include "Value.hpp"
#include "Globals.hpp"
#include <sstream>

using namespace DbXml;
using namespace std;

// function to test for whether the system has
// been initialized, which is triggered by active
// existence of at least one XmlManager object
static void
testInit()
{
	if (Globals::refCount_ == 0) {
		throw XmlException(
			XmlException::INVALID_VALUE, 
			"Cannot construct XmlValue instance without an active XmlManager object to initialize BDB XML");
	}
}

static void
throwNull(const char *s) {
	ostringstream oss;
	oss << "Cannot convert null XmlValue to " << s;
	throw XmlException(XmlException::INVALID_VALUE, oss.str());
}

static void throwNotNode(XmlValue::Type t) {
	ostringstream oss;
	oss << "Cannot convert XmlValue type " << t <<  " to Node";
	
	throw XmlException(XmlException::INVALID_VALUE, oss.str());
}

#define CHECKNODE(t) if ((t) != XmlValue::NODE) throwNotNode((t))

#define CHECKNULL(s) if (value_ == 0) throwNull((s))

XmlValue::XmlValue()
	: value_(0)
{
	testInit();
}

XmlValue::XmlValue(Value *value)
	: value_(value)
{
	testInit();
	if(value_ != 0) value_->acquire();
}

XmlValue::XmlValue(const string &v)
	: value_(0)
{
	testInit();
	value_ = new AtomicTypeValue(v);
	value_->acquire();
}

XmlValue::XmlValue(const char *v)
	: value_(0)
{
	testInit();
	value_ = new AtomicTypeValue(v);
	value_->acquire();
}

XmlValue::XmlValue(double v)
	: value_(0)
{
	testInit();
	value_ = new AtomicTypeValue(v);
	value_->acquire();
}

XmlValue::XmlValue(bool v)
	: value_(0)
{
	testInit();
	value_ = new AtomicTypeValue(v);
	value_->acquire();
}

XmlValue::XmlValue(const XmlDocument &document)
	:value_(0)
{
	testInit();
	value_ = new DbXmlNodeValue(document);
	value_->acquire();
}

XmlValue::XmlValue(Type type, const string &v)
	: value_(0)
{
	testInit();
	// validate type
	value_ = Value::create(type, v, true);
	if(value_ != 0) value_->acquire();
}

XmlValue::XmlValue(const string &typeURI, const string &typeName,
		   const string &v)
	: value_(0)
{
	testInit();
	value_ = new AtomicTypeValue(typeURI, typeName, v);
	value_->acquire();
}

XmlValue::XmlValue(Type type, const XmlData &dbt)
	: value_(0)
{
	testInit();
	// validate type
	value_ = Value::create(type, dbt, true);
	if(value_ != 0) value_->acquire();
}

XmlValue::~XmlValue()
{
	if(value_ != 0)
		value_->release();
}

XmlValue::XmlValue(const XmlValue &o)
	: value_(o.value_)
{
	if(value_ != 0) value_->acquire();
}

string
XmlValue::getNodeHandle() const
{
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getNodeHandle();
}

XmlValue &XmlValue::operator = (const XmlValue &o)
{
	if (this != &o && value_ != o.value_) {
		if (value_ != 0)
			value_->release();
		value_ = o.value_;
		if (value_ != 0)
			value_->acquire();
	}
	return *this;
}

string XmlValue::getNodeName() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getNodeName();
}

string XmlValue::getNamespaceURI() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getNamespaceURI();
}

string XmlValue::getPrefix() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getPrefix();
}

string XmlValue::getLocalName() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getLocalName();
}

string XmlValue::getNodeValue() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getNodeValue();
}

short XmlValue::getNodeType() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getNodeType();
}

XmlValue XmlValue::getParentNode() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getParentNode();
}

XmlValue XmlValue::getFirstChild() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getFirstChild();
}

XmlValue XmlValue::getLastChild() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getLastChild();
}

XmlValue XmlValue::getPreviousSibling() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getPreviousSibling();
}

XmlValue XmlValue::getNextSibling() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getNextSibling();
}

XmlResults XmlValue::getAttributes() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getAttributes();
}

XmlValue XmlValue::getOwnerElement() const {
	CHECKNODE(getType());
	return ((NodeValue*)value_)->getOwnerElement();
}

/// What type is the Value
XmlValue::Type XmlValue::getType() const
{
	if(value_ == 0) return NONE;
	return (value_->getType());
}

string XmlValue::getTypeURI() const
{
	if(value_ == 0) return "";
	return  value_->getTypeURI();
}

string XmlValue::getTypeName() const
{
	if(value_ == 0) return "";
	return  value_->getTypeName();
}

/// Does the value_ have no value_.
bool XmlValue::isNull() const
{
	return (value_ == 0);
}

bool XmlValue::isType(Type type) const
{
	if(value_ == 0) return type == NONE;
	return (value_->getType() == type);
}

bool XmlValue::isNumber() const
{
	if(value_ == 0) return false;
	return isType(XmlValue::DECIMAL) || isType(XmlValue::DOUBLE) ||
		isType(XmlValue::FLOAT);
}

bool XmlValue::isString() const
{
	if(value_ == 0) return false;
	return isType(XmlValue::STRING);
}

bool XmlValue::isBoolean() const
{
	if(value_ == 0) return false;
	return isType(XmlValue::BOOLEAN);
}

bool XmlValue::isBinary() const
{
	if(value_ == 0) return false;
	return isType(XmlValue::BINARY);
}

bool XmlValue::isNode() const
{
	if(value_ == 0) return false;
	return isType(XmlValue::NODE);
}

/// Return the value_ as a Number.
double XmlValue::asNumber() const
{
	CHECKNULL("Number");
	return value_->asNumber();
}

/// Return the value_ as a String.
string XmlValue::asString() const
{
	CHECKNULL("String");
	return value_->asString();
}

/// Return the value_ as a Boolean.
bool XmlValue::asBoolean() const
{
	CHECKNULL("Boolean");
	return value_->asBoolean();
}

/// Return the value_ as a Binary
XmlData XmlValue::asBinary() const
{
	CHECKNULL("Binary");
	const DbXmlDbt &dbt = value_->asBinary();
	return XmlData(dbt.data, dbt.size);
}

/// Return the value_ as a Document.
const XmlDocument &XmlValue::asDocument() const
{
	CHECKNULL("Document");
	return value_->asDocument();
}

/// Return the value_ as XmlEventReader &
XmlEventReader &XmlValue::asEventReader() const
{
	CHECKNULL("EventReader");
	return value_->asEventReader();
}

/// Compare two value_s for equality.
bool XmlValue::equals(const XmlValue &v) const
{
	if (isNull()) {
		return v.isNull();
	} else {
		if (v.isNull()) {
			return false;
		} else {
			return value_->equals(*v.value_);
		}
	}
}

/// Compare two value_s for equality.
bool XmlValue::operator==(const XmlValue &v) const
{
	return equals(v);
}
