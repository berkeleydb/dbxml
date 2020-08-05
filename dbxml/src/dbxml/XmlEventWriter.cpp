//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlEventWriter.hpp"
#include "dbxml/XmlException.hpp"

using namespace DbXml;
using namespace std;

/*
 * XmlEventWriter
 *
 * Default (no-op) implementation
 */
// no-op default ctors
XmlEventWriter::XmlEventWriter(){}
XmlEventWriter::XmlEventWriter(XmlEventWriter &){}

XmlEventWriter::~XmlEventWriter() {}

void XmlEventWriter::writeAttribute(const unsigned char *localName,
				    const unsigned char *prefix,
				    const unsigned char *uri,
				    const unsigned char *value,
				    bool isSpecified)
{

}

void XmlEventWriter::writeText(XmlEventReader::XmlEventType type,
			       const unsigned char *text,
			       size_t length)
{

}

void XmlEventWriter::writeProcessingInstruction(const unsigned char *target,
						const unsigned char *data)
{

}

void XmlEventWriter::writeStartElement(const unsigned char *localName,
				       const unsigned char *prefix,
				       const unsigned char *uri,
				       int numAttributes,
				       bool isEmpty)
{

}

void XmlEventWriter::writeEndElement(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri)
{

}
	
void XmlEventWriter::writeDTD(const unsigned char *dtd, size_t length)
{
}

void XmlEventWriter::writeStartDocument(const unsigned char *version,
					const unsigned char *encoding,
					const unsigned char *standalone)
{

}

void XmlEventWriter::writeEndDocument()
{

}

void XmlEventWriter::writeStartEntity(const unsigned char *name,
				      bool expandedInfoFollows)
{
}

void XmlEventWriter::writeEndEntity(const unsigned char *name)
{
}
