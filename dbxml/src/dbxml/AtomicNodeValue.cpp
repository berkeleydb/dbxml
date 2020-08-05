//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "AtomicNodeValue.hpp"
#include "Results.hpp"
#include "nodeStore/NsDom.hpp"
#include "nodeStore/NsFormat.hpp"
#include "Value.hpp"
#include "UTF8.hpp"
#include <dbxml/XmlResults.hpp>

using namespace DbXml;

#define GET_RESULTS() ((Results &) ((XmlResults&)results_))
#define GET_MANAGER() GET_RESULTS().getManager()

// Implementation

AtomicNodeValue::AtomicNodeValue()
	: prefix_(0), value_(0), localName_(0)
{}

AtomicNodeValue::AtomicNodeValue(XmlValue &value, XmlResults &results)
	: RawNodeValue(results), prefix_(0), value_(0), localName_(0)
{
	DBXML_ASSERT(value.isNode());

	DbXmlNodeValue *val = const_cast<DbXmlNodeValue*>(((Value*)value)->
							  getDbXmlNodeValueObject());
	DBXML_ASSERT(val);
	type_ = val->getNodeType();
	const DbXmlNodeImpl *node = val->getNodeImpl(NULL, false);
	DBXML_ASSERT(node);
	//The strings are copied in marshal and the pointers changed to the copy
	XMLChToUTF8 pre(node->getPrefix());
	prefix_ = (xmlbyte_t*)pre.ucstr();
	XMLChToUTF8 loc;
	if (type_ == nsNodePinst) {
		loc.set(node->getPITarget());
		localName_ = (xmlbyte_t*)loc.ucstr();
	} else {
		loc.set(node->getLocalName());
		localName_ = (xmlbyte_t*)loc.ucstr();
	}
	XMLChToUTF8 nodeValue(node->getValue());
	value_ = (xmlbyte_t*)nodeValue.ucstr();
	void *data = 0;
	marshal(data);
}

AtomicNodeValue::AtomicNodeValue(const AtomicNodeValue &other)
	: RawNodeValue(other.results_), prefix_(0), value_(0), localName_(0)
{
	prefix_ = other.prefix_;
	value_ = other.value_;
	localName_ = other.localName_;
	void *data = 0;
	marshal(data);
}

AtomicNodeValue::AtomicNodeValue(const void *data, XmlResults &results)
	: RawNodeValue(results), prefix_(0), value_(0), localName_(0)
{
	loadValue(data);
}

AtomicNodeValue::~AtomicNodeValue()
{}

static const char *textName = "#text";
static const char *cdataName = "#cdata-section";
static const char *commentName = "#comment";

std::string AtomicNodeValue::getNodeName() const
{
	if (localName_ && NsUtil::nsStringLen(localName_) > 0) {
		if (prefix_ && NsUtil::nsStringLen(prefix_) > 0) {
			std::string name((char *)prefix_);
			name.append(":");
			name.append((char *)localName_);
			return name;
		}
		return (char *)localName_;
	}
	switch(type_) {
		case nsNodeText: return textName;
		case nsNodeCDATA: return cdataName;
		case nsNodeComment: return commentName;
		default: return "";
	};
}

std::string AtomicNodeValue::getNodeValue() const
{
	return (char *)value_;
}

std::string AtomicNodeValue::getNamespaceURI() const
{
	return "";
}

std::string AtomicNodeValue::getPrefix() const
{
	if (prefix_)
		return (char *)prefix_;
	return "";
}

std::string AtomicNodeValue::getLocalName() const
{
	if (localName_ && type_ != nsNodePinst)
		return (char *)localName_;
	return "";
}

std::string AtomicNodeValue::asString() const
{
	if (type_ == nsNodeAttr) {
		std::string str = "{";
		str.append("}");
		str.append((char*)localName_);
		str.append("=\"");
		str.append((char*)value_);
		str.append("\"");
		return str;
	} else if (type_ == nsNodeText) {
		return (char*)value_;
	} else if (type_ == nsNodeComment) {
		std::string str = "<!--";
		str.append((char*)value_);
		str.append("-->");
		return str;
	} else if (type_ == nsNodeCDATA) {
		std::string str = "<![CDATA[";
		str.append((char*)value_);
		str.append("]]>");
		return str;
	} else if (type_ == nsNodePinst) {
		std::string str = "<?";
		// PI target is localName
		str.append((char*)localName_);
		str.append(" ");
		str.append((char*)value_);
		str.append("?>");
		return str;
	} else {
		throw XmlException(XmlException::INVALID_VALUE,
				   "cannot create string for type");
	}
}

bool AtomicNodeValue::asBoolean() const
{
	std::string s(asString());
	return (!s.empty() && s.compare("false") != 0);
}

double AtomicNodeValue::asNumber() const
{
	return strtod(asString().c_str(), 0);
}

void AtomicNodeValue::initIndexEntryFromThis(IndexEntry &) const
{
	throw XmlException(XmlException::INVALID_VALUE,
				   "getNodeHandle() requires a live node");
}

bool AtomicNodeValue::equals(const Value &other) const
{
	const RawNodeValue *raw = other.getRawNodeValueObject();
	if (raw) {
		const AtomicNodeValue *otherVal = raw->getAtomicNodeValueObject();
		if (otherVal) {
			if (NsUtil::nsStringEqual(otherVal->prefix_, prefix_) &&
				NsUtil::nsStringEqual(otherVal->localName_, localName_) &&
				NsUtil::nsStringEqual(otherVal->value_, value_))
				return true;
		}
	}
	return false;
}

XmlValue AtomicNodeValue::getParentNode() const
{
	return XmlValue();
}

XmlValue AtomicNodeValue::getOwnerElement() const {

	if (type_ ==  nsNodeAttr) 
		return XmlValue();
	throw XmlException(XmlException::INVALID_VALUE,
			   "Node is not an attribute node");

}

XmlResults AtomicNodeValue::getAttributes() const {
	XmlManager &mgr = GET_MANAGER();
	return new ValueResults(mgr, 0);
}

XmlEventReader &AtomicNodeValue::asEventReader() const
{
	throw XmlException(XmlException::INVALID_VALUE,
			   "XmlValue::asEventReader requires an element node");
}

const XmlDocument &AtomicNodeValue::asDocument() const
{
	return xdoc_;
}

// Order is:
// RawNode type
// type
// value
// local name
// prefix
int AtomicNodeValue::marshal(void *&data)
{
	size_t valueLen = NsUtil::nsStringLen(value_) + 1;
	size_t localNameLen = NsUtil::nsStringLen(localName_) + 1;
	size_t prefixLen = NsUtil::nsStringLen(prefix_) + 1;
	dataSize_ = 1; // RawNode type
	dataSize_ += 1; // type
	dataSize_ += (int)valueLen;
	dataSize_ += (int)localNameLen;
	dataSize_ += (int)prefixLen;
	data_ = NsUtil::allocate(dataSize_, "AtomicValueNode dump");
	xmlbyte_t *ptr = (xmlbyte_t *)data_;
	*ptr = AtomicNodeType;
	ptr++;
	ptr += NsFormat::marshalInt(ptr, type_);
	if (value_)
		memcpy(ptr, value_, valueLen);
	else
		*ptr = '\0';
	value_ = ptr;
	ptr += valueLen;
	if (localName_)
		memcpy(ptr, localName_, localNameLen);
	else
		*ptr = '\0';
	localName_ = ptr;
	ptr += localNameLen;
	if (prefix_)
		memcpy(ptr, prefix_, prefixLen);
	else 
		*ptr = '\0';
	prefix_ = ptr;
	data = data_;
	return dataSize_;
}

int AtomicNodeValue::dumpValue(void *&data)
{
	if (!data_)
		return marshal(data);
	data = data_;
	return dataSize_;
}

void AtomicNodeValue::loadValue(const void *data)
{
	const xmlbyte_t *ptr = (const xmlbyte_t *)data;
	int32_t ttype;
	ptr ++; //RawNode Type
	ptr += NsFormat::unmarshalInt(ptr, &ttype);
	type_ = (short) ttype;
	value_ = const_cast<xmlbyte_t*>(ptr);
	ptr += NsUtil::nsStringLen(ptr) + 1;
	localName_ = const_cast<xmlbyte_t*>(ptr);
	ptr += NsUtil::nsStringLen(ptr) + 1;
	prefix_ = const_cast<xmlbyte_t*>(ptr);
	void *data2 = 0;
	marshal(data2);
}
