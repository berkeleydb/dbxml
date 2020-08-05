//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "EventReader.hpp"
#include "NsDocument.hpp"
#include "NsUtil.hpp"

/*
 * EventReader
 *
 * An abstract base for NsEventReader
 */

using namespace DbXml;
using namespace std;

EventReader::EventReader()
	: 
	  expandEntities_(true),
	  reportEntityInfo_(false),
	  hasNext_(true)
{}

void
EventReader::throwIllegalOperation(XmlEventType type, const char *method) const
{
	std::string err ="Cannot call method, XmlEventReader::";
	err.append(method);
	err.append(" on event type: ");
	err.append(typeToString(type));
	throw XmlException(XmlException::EVENT_ERROR, err);
}

const char *
EventReader::typeToString(XmlEventType type) const
{
	switch(type) {
	case StartElement:
		return "StartElement";
	case EndElement:
		return "EndElement";
	case Characters:
		return "Characters";
	case CDATA:
		return "CDATA";
	case Comment:
		return "Comment";
	case Whitespace:
		return "Whitespace";
	case StartDocument:
		return "StartDocument";
	case EndDocument:
		return "EndDocument";
	case StartEntityReference:
		return "StartEntityReference";
	case EndEntityReference:
		return "EndEntityReference";
	case ProcessingInstruction:
		return "ProcessingInstruction";
	case DTD:
		return "DTD";
	default:
		return "UNKNOWN";
	}
}

void
EventReader::setReportEntityInfo(bool value)
{
	reportEntityInfo_ = value;
}

bool
EventReader::getReportEntityInfo() const
{
	return reportEntityInfo_;
}

void
EventReader::setExpandEntities(bool value)
{
	expandEntities_ = value;
}

bool
EventReader::getExpandEntities() const
{
	return expandEntities_;
}

void
EventReader::ensureType(XmlEventType type, const char *method) const
{
	if (type != type_) {
		throwIllegalOperation(type, method);
	}
}

// verify attributes
void
EventReader::ensureAttributes(int index, const char *method) const
{
	ensureType(StartElement, method);
	if ((index + 1) > getAttributeCount())
		throw XmlException(
			XmlException::EVENT_ERROR,
			"Attempt to read an attribute out of range of the current element");
}

XmlEventReader::XmlEventType
EventReader::getEventType() const
{
	return type_;
}


const unsigned char *
EventReader::getSystemId() const
{
	ensureType(StartDocument, "getSystemId");
	// don't have the name handy, but could probably
	// make it so... Currently, no internal methods
	// need the name
	return 0;
}
	
bool
EventReader::hasEntityEscapeInfo() const {
	return true;
}

bool
EventReader::hasEmptyElementInfo() const {
	return true;
}

static unsigned int cr = 0x0d;
static unsigned int lf = 0x0a;
static unsigned int tab = 0x09;
static unsigned int sp = 0x20;

bool
EventReader::isWhiteSpace() const
{
	if (type_ == Whitespace)
		return true;
	if (!value_)
		throwIllegalOperation(type_, "isWhiteSpace");
	if (type_ != Characters && type_ != CDATA)
		throwIllegalOperation(type_, "isWhiteSpace");
	// use the raw string
	const char *ptr = (const char *)value_;
	unsigned int ch = (unsigned int) (*ptr);
	while (ch != 0) {
		if (ch > sp)
			return false;
		if ((ch == sp) || (ch == tab) ||
		    (ch == lf) || (ch == cr))
			ch = *++ptr;
		else
			return false;
	}
	return true;
}

XmlEventReader::XmlEventType
EventReader::nextTag()
{
	XmlEventType type = next();
	while ((type == Characters) ||
	       (type == CDATA) ||
	       (type == Whitespace) ||
	       (type == ProcessingInstruction) ||
	       (type == Comment))
		type = next();
	
	if ((type != StartElement) &&
	    (type != EndElement) &&
	    (type != StartDocument) &&
	    (type != EndDocument))
		throw XmlException(XmlException::EVENT_ERROR,
				   "expected start or end tag in nextTag");
	return type;
}


