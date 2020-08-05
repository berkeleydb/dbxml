//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSXERCESINDEXER_HPP
#define __DBXMLNSXERCESINDEXER_HPP
	
#include "NsEvent.hpp"
#include "NsHandlerBase.hpp"

namespace DbXml {

class EventWriter;

/**
 * NsXercesIndexer
 *
 * Implements NsEventTranslator, to 
 * handle parser events and transcodes to UTF-8,
 * and write to the BDB XML indexer.
 *
 * This class only exists to streamline
 * indexing of document stored as whole-doc storage.
 * It bypasses creation of nsNode_t objects.
 *
 * It also does not implement interfaces known not to be of
 * interest to the indexer.
 *
 * Indexer only needs start/end element, and character events
 * involving text (no comments, no processing instructions)
 */
class NsXercesIndexer : public NsEventTranslator {
public:
	NsXercesIndexer(EventWriter *writer = 0);
	virtual ~NsXercesIndexer();
	virtual void startDocument(const xmlch_t *sniffedEncoding);
	virtual void endDocument();
	virtual void startElement(const xmlch_t *localName,
				  const xmlch_t *prefix,
				  const xmlch_t *uri,
				  NsEventAttrList16 *attrs,
				  const uint32_t attrCount,
				  bool isEmpty);
	virtual void endElement(const xmlch_t *localName,
				const xmlch_t *prefix,
				const xmlch_t *uri);
	virtual void characters(const xmlch_t *characters,
				size_t len, bool isCDATA,
				bool needsEscape);
	virtual void comment(const xmlch_t *comment, size_t len) {}
	virtual void processingInstruction(const xmlch_t *target,
					   const xmlch_t *data) {}
	virtual EventWriter *getEventWriter(void) const {
		return _writer;
	}
	virtual void setEventWriter(EventWriter *writer) {
		_writer = writer;
	}
private:
	EventWriter *_writer;
};

/**
 * hold transcoded attribute strings, for deallocation
 */	
struct attrString {
	struct attrString *next;
	xmlbyte_t s[1];
};

/**
 * A Special, extended attribute list handler, with
 * methods tailored for the DB XML Indexer
 */
class NsEventAttrListIndexer : public NsEventAttrList {
public:
	// Indexer does not use most of these.
	NsEventAttrListIndexer(NsEventAttrList16 *alist,
			       const xmlch_t *elemName = 0,
			       const xmlch_t *elemUri = 0)
		: _alist(alist), _toFree(0), _elemName(elemName),
		  _elemUri(elemUri) {}
	virtual ~NsEventAttrListIndexer();
	virtual bool isEmpty(void) const { return true; } // unused
	virtual int numAttributes(void) const { return 0; } // unused
	virtual const xmlbyte_t *prefix(int index) const;
	virtual const xmlbyte_t *localName(int index) const;
	virtual const xmlbyte_t *value(int index) const;
	virtual const xmlbyte_t *uri(int index) const;
#ifdef NS_USE_SCHEMATYPES
	virtual bool hasTypeInfo(int index) const {
		return false; // unused
	}
	virtual const xmlbyte_t *typeName(int index) const {
		return 0; //unused
	}
	virtual const xmlbyte_t *typeUri(int index) const {
		return 0; //unused
	}
#endif
	virtual bool needsEscape(int index) const {
		return true; // unused
	}
	virtual bool isSpecified(int index) const {
		return true; // unused
	}
private:
	struct attrString *_allocString(const xmlch_t *str,
					size_t &nchars) const;
private:
	NsEventAttrList16 *_alist;
	mutable struct attrString *_toFree;
	const xmlch_t *_elemName;
	const xmlch_t *_elemUri;
};

}

#endif
