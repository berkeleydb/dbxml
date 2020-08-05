//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlEventReader.hpp"
#include "dbxml/XmlException.hpp"

using namespace DbXml;
using namespace std;

/*
 * XmlEventReader
 *
 * Default (no-op) implementation
 */
/*
 * see:
 * stax -- http://stax.codehaus.org/maven/apidocs/
 * xmlpull --  http://www.xmlpull.org/v1/doc/api/
 * libxml2 -- http://xmlsoft.org/html/index.html
 */
/*
 * XmlEventReader -- events are pulled from this
 *
 * Requirements:
 *  o support StAX (JSR 173) (Java)
 *  o support libxml2 (C/C++)
 *  o support scripting interface parsers and processing for
 *    Python, Perl, and PHP
 *
 * Design notes:
 *   XmlEventReader interface is iterative, returning generic objects
 *     that represent parsed bits of XML.  Attributes are not themselves
 *     events, but are accessed from a StartElement event via API.
 *
 *   A close() method is included in order to let the object know that
 *   it can release any associated resources, or delete itself.
 *
 * Usage patterns:
 *  Document input/creation:
 *   Virtual behavior:
 *     o implement XmlEventReader, then call:
 *     myReader reader; // myReader is XmlEventReader
 *     doc.setContentAsEventReader(..., reader,...);
 *
 *  Document output (pull):
 *   Non-virtual behavior (pull processing):
 *     XmlEventReader &reader = doc.getContentAsEventReader();
 *     while (reader.next()) // do something -- e.g. generate sax events
 *     reader.close();
 *
 *  XmlValue (must be a node):
 *    output:
 *      XmlEventReader &reader = value.asEventReader();
 */

/*
 * Interfaces on existing objects
 *
 * XmlDocument
 *  XmlEventReader &XmlDocument::getContentAsEventReader() const;
 *  void XmlDocument::setContentAsEventReader(XmlEventReader &reader);
 *  XmlEventWriter &XmlDocument::setContentAsEventWriter();
 *
 * XmlContainer
 *  std::string XmlContainer::putDocument(const std::string &name,
 *     XmlEventReader &reader, ....);
 *  This one returns a writer, allowing the user to push content
 *  until EndDocument.
 *  XmlEventWriter &XmlContainer::putDocumentAsEventWriter(...)
 *
 * XmlValue
 *  XmlEventReader &XmlValue::asEventReader()
 *  XmlValue::asEventWriter(XmlEventWriter &)
 *  -- there is no input mechanism to XmlValue using these classes
 */

// no-op default ctors
XmlEventReader::XmlEventReader(){}
XmlEventReader::XmlEventReader(XmlEventReader &){}

static void throwMustImplement(const char *method)
{
	string msg("Required method not implemented on XmlEventReader: ");
	msg += (string)method;
	throw XmlException(XmlException::EVENT_ERROR, msg);
}

XmlEventReader::~XmlEventReader() {}

void
XmlEventReader::setReportEntityInfo(bool)
{
}
bool
XmlEventReader::getReportEntityInfo() const
{
	return false;
}
void
XmlEventReader::setExpandEntities(bool)
{
}
bool
XmlEventReader::getExpandEntities() const
{
	return true;
}

enum XmlEventReader::XmlEventType XmlEventReader::next() {
	throwMustImplement("next");
	return (XmlEventType) -1;
}

enum XmlEventReader::XmlEventType XmlEventReader::nextTag() {
	throwMustImplement("next");
	return (XmlEventType) -1;
}

bool XmlEventReader::hasNext() const {
	return false;
}

enum XmlEventReader::XmlEventType XmlEventReader::getEventType() const {
	throwMustImplement("next");
	return (XmlEventType) -1;
}
const unsigned char *XmlEventReader::getNamespaceURI() const {
	return NULL;
}
const unsigned char *XmlEventReader::getLocalName() const {
	return NULL;
}
const unsigned char *XmlEventReader::getPrefix() const {
	return NULL;
}
const unsigned char *XmlEventReader::getValue(size_t &len) const {
	len = 0;
	return NULL;
}
int XmlEventReader::getAttributeCount() const {
	return 0;
}
bool XmlEventReader::isAttributeSpecified(int) const {
	return false;
}
const unsigned char *XmlEventReader::getAttributeLocalName(int index) const {
	return NULL;
}
const unsigned char *XmlEventReader::getAttributeNamespaceURI(int index) const {
	return NULL;
}
const unsigned char *XmlEventReader::getAttributePrefix(int index) const {
	return NULL;
}
const unsigned char *XmlEventReader::getAttributeValue(int index) const {
	return NULL;
}
const unsigned char *XmlEventReader::getEncoding() const {
	return NULL;
}
const unsigned char *XmlEventReader::getVersion() const {
	return NULL;
}
const unsigned char *XmlEventReader::getSystemId() const {
	return NULL;
}
bool XmlEventReader::isStandalone() const {
	return false;
}
bool XmlEventReader::standaloneSet() const {
	return false;
}
bool XmlEventReader::encodingSet() const {
	return false;
}

bool XmlEventReader::hasEntityEscapeInfo() const {
	return false;
}
	
bool XmlEventReader::needsEntityEscape(int index) const
{
	return true;
}

bool XmlEventReader::hasEmptyElementInfo() const {
	return false;
}

bool XmlEventReader::isEmptyElement() const
{
	// must be implemented if hasEmptyElementInfo is true
	return false;
}

bool XmlEventReader::isWhiteSpace() const
{
	// this can be entirely implemented here, but is
	// not, because the underlying implementation can
	// be more efficient.
	throwMustImplement("isWhiteSpace");
	return false;
}

