/*
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004,2009 Oracle.  All rights reserved.
 *
 */

#include <dbxml/DbXml.hpp>
#include <xercesc/dom/DOMDocument.hpp>

/*
 * This is sample code that uses Berkeley DB XML XmlEventReader
 * and XmlEventWriter interfaces to translate documents to and
 * from Xerces-C DOM documents.  It is provided as-is without
 * extensive testing.
 *
 * It will do 2 things:
 * 1.  Create DOMDocument from XmlValue (XML document or fragment)
 *   o This is implemented as an XmlEventWriter class that "writes"
 *     events to Xerces-C DOM.  The events come from the fragment
 *     via XmlEventReader.  The XmlEventReaderToWriter class plugs
 *     the two together.
 * 2.  Create Berkeley DB XML document from Xerces-C DOMDocument.
 *   o This is implemented by a class that knows how to walk
 *   a Xerces-C DOM, pushing events to an XmlEventWriter obtained
 *   using XmlContainer::putDocumentAsEventWriter().
 *
 * The approaches used here can be used to hook BDB XML to any
 * XML parser or DOM model, in most supported language bindings.
 */

//
// The public API to get/put content using Xerces-C DOM
//
XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getContentAsDOM(
	const DbXml::XmlValue &value);

void putDocumentAsDOM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *domDoc,
		      const std::string &name,
		      DbXml::XmlManager &manager,
		      DbXml::XmlContainer &container,
		      DbXml::XmlTransaction &txn, u_int32_t flags);

void putDocumentAsDOM(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *domDoc,
		      const std::string &name,
		      DbXml::XmlManager &manager,
		      DbXml::XmlContainer &container,
		      u_int32_t flags);

//
// Implementation classes from here down
//

// 
// This class implements XmlEventWriter in order to push (write)
// BDB XML events (obtained via XmlEventReader) to a Xerces-C
// DOMDocument.
//
class DOMEventWriter : public DbXml::XmlEventWriter
{
public:
	DOMEventWriter();
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument() {
		return doc_;
	}
	virtual void close();
	
	// attributes (including namespaces)
	virtual void writeAttribute(const unsigned char *localName,
				    const unsigned char *prefix,
				    const unsigned char *uri,
				    const unsigned char *value,
				    bool isSpecified);

	// text, comments, CDATA, ignorable whitespace
	// length does not include trailing null
	virtual void writeText(DbXml::XmlEventReader::XmlEventType type,
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
	
	// DTD
	virtual void writeDTD(const unsigned char *dtd, size_t length);

	// XML decl and start document
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
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc_;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *currentParent_;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *currentNode_;
};

//
// This class walks a Xerces-C DOMDocument, pushing
// events to XmlEventWriter in order to construct a document
//
class DOMToEventWriter
{
public:
	DOMToEventWriter(DbXml::XmlEventWriter &writer,
			 XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *domDoc);
	~DOMToEventWriter();
	void start();
private:
	void toEvents(
		const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node);
private:
	DbXml::XmlEventWriter &writer_;
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc_;
};


