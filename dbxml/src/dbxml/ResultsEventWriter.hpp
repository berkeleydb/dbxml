//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __RESULTSEVENTWRITER_HPP
#define __RESULTSEVENTWRITER_HPP

#include "nodeStore/NsEventWriter.hpp"

/*
 * ResultsEventWriter
 *
 * Implementation of XmlEventWriter.
 *
 */

namespace DbXml
{
class Document;
class Results;

// ResultsEventWriter
//
class ResultsEventWriter : public XmlEventWriter
{
public:
	// Construction
	ResultsEventWriter(Results *res);

	virtual ~ResultsEventWriter();

	virtual void close();

	//
	// XmlEventWriter interface
	//

	// attributes (including namespaces)
	virtual void writeAttribute(const unsigned char *localName,
				    const unsigned char *prefix,
				    const unsigned char *uri,
				    const unsigned char *value,
				    bool isSpecified);

	// text, comments, CDATA, ignorable whitespace
	// length does not include trailing null
	virtual void writeText(XmlEventReader::XmlEventType type,
			       const unsigned char *text,
			       size_t length);

	// processing Instruction
	virtual void writeProcessingInstruction(const unsigned char *target,
						const unsigned char *data);

	// elements
	virtual void writeStartElement(const unsigned char *localName,
				       const unsigned char *prefix,
				       const unsigned char *uri,
				       int numAttributes,
				       bool isEmpty);
	virtual void writeEndElement(const unsigned char *localName,
				     const unsigned char *prefix,
				     const unsigned char *uri);

	// xml declaration, DTD
	virtual void writeDTD(const unsigned char *dtd, size_t length);
	virtual void writeStartDocument(const unsigned char *version,
					const unsigned char *encoding,
					const unsigned char *standalone);
	virtual void writeEndDocument();

	// note the start of entity expansion
	virtual void writeStartEntity(const unsigned char *name,
				      bool expandedInfoFollows);

	// note the end of entity expansion
	virtual void writeEndEntity(const unsigned char *name);

private:
	DocID allocateDocID(void);
	void createEventWriter(void);
	void addNode(void);
	void throwBadWrite(const char *msg);

private:
	Results *res_;
	XmlDocument xmlDoc_;
	bool hasStartDocument_;
	int depth_;
	NsEventWriter *writer_;
	const unsigned char *version_;
	const unsigned char *encoding_;
	const unsigned char *standalone_;
};
}
#endif

