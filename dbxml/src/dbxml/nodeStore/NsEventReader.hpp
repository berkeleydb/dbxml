//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSEVENTREADER_HPP
#define __DBXMLNSEVENTREADER_HPP
	
#include "EventReader.hpp"
#include "NsNid.hpp"
#include "NsRawNode.hpp"
#include "NsEvent.hpp"
#include "NsDocument.hpp"
#include "../ScopedDbt.hpp"
#include "../Cursor.hpp"

// use 1K buffer for getting the string value of an element
// It will be rounded up to container page size if page size is > 1k
#define NS_EVENT_VALUE_BUFSIZE (1024)

// use 256K buffer for fetching whole documents or large subtrees
#define NS_EVENT_BULK_BUFSIZE (256 * 1024)

namespace DbXml
{

// forwards
class NsEventReaderBuf;
class NsEventReaderNodeList;
class DbWrapper;

/**
 * NsEventReader
 *
 * This class implements pull methods to walk all or part of a
 * node storage document, using the XmlEventReader interface,
 * which it implements.
 *
 * There is no reset method to restart the cursor iteration;
 * it's cheap to recreate the object.
 */
class NsEventReader : public EventReader {
public:
	NsEventReader(Transaction *txn, DbWrapper *db,
		DictionaryDatabase *ddb, const DocID &docId,
		int cid, u_int32_t flags, uint32_t bufferSize,
		const NsNid *startId = 0, CacheDatabase *cdb = 0);
	NsEventReader(NsDoc &nsDoc, uint32_t bufferSize,
		const NsNid *startId = 0, CacheDatabase *cdb = 0);
	virtual ~NsEventReader();
	virtual void close();
	// the public XmlEventReader interface

	// iterators
	virtual XmlEventType next();

	// naming
	virtual const unsigned char *getNamespaceURI() const;
	// getlocalName returns processing instruction target
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

	// start_document only -- see EventReader

	// BDB XML "extensions" in XmlEventReader
	virtual bool needsEntityEscape(int index = 0) const;
	virtual bool isEmptyElement() const;

	// required by EventReader
	IndexNodeInfo *getIndexNodeInfo() const { return &ninfo_; }
	const unsigned char *getVersion() const;
	const unsigned char *getEncoding() const;
	bool standaloneSet() const;
	bool encodingSet() const;
	bool isStandalone() const;

private:
	void cleanup();
	bool doText();
	bool doElement(bool start);
	void endElement();
	void nextNode(NsEventReaderBuf **bufp,
		      DbXmlDbt &key, DbXmlDbt &data, NsFullNid *startId = 0);
	void releaseNode(NsEventReaderBuf *buf);
	void getNode(NsEventReaderNodeList *parent);
	void fetchAttributes(int index) const;
	void setNode(NsRawNode *node) { ninfo_.node_ = node; }
	NsRawNode *getNode() const {
		return const_cast<NsRawNode*>(ninfo_.node_);
	}
	
	// name info
	const unsigned char *localName_;
	
	// text event info
	uint32_t textType_;

	// element/attribute info
	mutable NsRawNodeIndexNodeInfo ninfo_;
	mutable nsAttr_t attr_;
	mutable const unsigned char *curAttrName_;
	mutable int attrIndex_;
	bool emptyElement_;
	int nattrs_;

	// data
	mutable NsDocument document_;
	bool doInit_;
	bool popElement_;
	int entityCount_;

	mutable NsEventReaderNodeList *current_;
	NsEventReaderBuf *currentBuffer_;

	DbtOut docKey_;
        Cursor cursor_;
	u_int32_t cursorFlags_;
	NsFullNid startId_;
	// memory mgmt
	NsEventReaderBuf *reuseList_;
	NsEventReaderBuf *freeList_;

	CacheDatabaseHandle docdb_;

	uint32_t bufferSize_;
};

}

#endif
