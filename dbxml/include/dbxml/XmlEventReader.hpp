//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLEVENTREADER_HPP
#define __XMLEVENTREADER_HPP

#include "DbXmlFwd.hpp"
#include "XmlPortability.hpp"
#include <string>
 
namespace DbXml
{

class DBXML_EXPORT XmlEventReader
{
public:
	enum XmlEventType {
		StartElement,
		EndElement,
		Characters,
		CDATA,
		Comment,
		Whitespace,
		StartDocument,
		EndDocument,
		StartEntityReference,
		EndEntityReference,
		ProcessingInstruction,
		DTD
	};
public:
	virtual ~XmlEventReader();
	virtual void close() = 0;

	//
	// configuration (settable)
	//
	virtual void setReportEntityInfo(bool value);
	virtual bool getReportEntityInfo() const;
	virtual void setExpandEntities(bool value);
	virtual bool getExpandEntities() const;

	// methods common to all types and states

	// iterators
	virtual XmlEventType next();
	virtual XmlEventType nextTag(); // next start or end element

	// event type and state
	virtual bool hasNext() const;
	virtual XmlEventType getEventType() const;

	// naming
	virtual const unsigned char *getNamespaceURI() const;
	// getLocalName returns processing instruction target as well
	virtual const unsigned char *getLocalName() const;
	virtual const unsigned char *getPrefix() const;
	
	// value -- text values and processing instruction data
	virtual const unsigned char *getValue(size_t &len) const;

	// attribute access (includes namespace attributes)
	virtual int getAttributeCount() const;
	virtual bool isAttributeSpecified(int index) const;
	virtual const unsigned char *getAttributeLocalName(int index) const;
	virtual const unsigned char *getAttributeNamespaceURI(int index) const;
	virtual const unsigned char *getAttributePrefix(int index) const;
	virtual const unsigned char *getAttributeValue(int index) const;

	// start_document only
	virtual const unsigned char *getEncoding() const;
	virtual const unsigned char *getVersion() const;
	virtual const unsigned char *getSystemId() const;
	virtual bool isStandalone() const;
	virtual bool standaloneSet() const;
	virtual bool encodingSet() const;
	
	// some extra information
	virtual bool hasEntityEscapeInfo() const;
	virtual bool needsEntityEscape(int index = 0) const;
	virtual bool hasEmptyElementInfo() const;
	virtual bool isEmptyElement() const;
	virtual bool isWhiteSpace() const;

protected:
	// prevent accidental copying
	XmlEventReader();
	XmlEventReader(XmlEventReader &);
};
}

#endif

