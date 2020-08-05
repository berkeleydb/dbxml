//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __NSEVENTWRITER_HPP
#define __NSEVENTWRITER_HPP

#include <dbxml/XmlEventWriter.hpp>
#include "NsHandlerBase.hpp"
#include "NsDocument.hpp"
#include <string>

/*
 * NsEventWriter
 *
 * Implementation of XmlEventWriter that will push events
 * to one, and optionally two EventWriter objects for
 * indexing and (optionally) writing.
 *
 * Implements XmlEventWriter and inherits from NsHandlerBase in
 * order to create node storage documents
 */
 
namespace DbXml
{

class NsDocument;
class NsDocumentDatabase;
class NsWriter;
class BufferNsStream;
class EventWriter;
class NsEventNodeAttrList;
class Container;
class UpdateContext;
class Document;
class XmlException;
	
//
// NsEventWriter
//
class NsEventWriter : public XmlEventWriter, public NsHandlerBase
{
public:
	// Construction
	NsEventWriter(Container *container, UpdateContext *uc,
		      Document *document, u_int32_t flags);
	// stripped-down version, only used by upgrade (for now)
	NsEventWriter(DbWrapper *db,
		      DictionaryDatabase *dict,
		      const DocID &id,
		      Transaction *txn = 0);
	virtual ~NsEventWriter();

	virtual void close();
	
	void createNsWriter();

	// configuration to allow writing into the middle of existing
	// documents
	void setNsNidGen(NsNidGen *gen) {
		nidGen_ = gen;
	}
	void setDepth(uint32_t depth) {
		_depth = depth;
	}
	void setCurrent(NsNode *current) {
		_current = current;
	}
	NsNode *getCurrent() const {
		return _current;
	}
	void setPrevious(NsNode *previous) {
		_previous = previous;
	}
	
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

	//
	// Methods used by internal consumers of NsEventWriter
	// These do little/no error checking
	//
	void writeStartElementInternal(const unsigned char *localName,
				       const unsigned char *prefix,
				       const unsigned char *uri,
				       NsFullNid *nid, bool setIsRoot);
	void writeEndElementInternal();
	void writeAttributeInternal(const unsigned char *localName,
				    const unsigned char *prefix,
				    const unsigned char *uri,
				    const unsigned char *value,
				    bool isSpecified);
	
	void writeStartDocumentInternal(const unsigned char *version,
					const unsigned char *encoding,
					const unsigned char *standalone,
					NsFullNid *nid);
	void writeEndDocumentInternal();
	void closeInternal();
private:
	void doStartElem(NsEventNodeAttrList *attrs = 0);
	void throwBadWrite(const char *msg);
	void exceptionNotify(XmlException &xe);
	void completeDoc();
	void getNextNid(NsFullNid *nid);
private:
	NsDocument nsdoc_;
	UpdateContext *uc_;
	Container *cont_;
	XmlDocument doc_;
	DbWrapper *db_;
	DocID docId_;
	NsWriter *writer_;
	BufferNsStream *writerStream_;
	bool mustBeEnd_;
	bool success_;
	bool needsStartElement_;
	bool needsEndDocument_;
	bool isEmpty_;
	int attrsToGo_;
	NsNidGen *nidGen_;
};
}
	
#endif

