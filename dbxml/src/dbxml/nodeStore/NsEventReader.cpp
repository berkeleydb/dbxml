//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "NsEventReader.hpp"
#include "NsDocument.hpp"
#include "NsUtil.hpp"
#include "NsConstants.hpp"
#include "NsFormat.hpp"
#include "../DbWrapper.hpp"
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xercesc/framework/MemoryManager.hpp>

/*
 * NsEventReader
 *
 * A pull interface to walk node storage documents.
 * This is based on the original NsEventGenerator code,
 * and will replace it when working, and necessary
 * adapter classes are built.
 */

#define MAKEBOOL(x) ((x)? true : false)

namespace DbXml {

#define FORMAT_DEBUG 1
#ifdef FORMAT_DEBUG
// see code in NsFormat.cpp
extern int _dumpFormatNodes;
#endif

/*
 * NsEventReaderBuf
 *
 * Helper class that encapsulates buffer mgmt for the DB bulk
 * retrieval API.  The only tricky part is knowing when all of the
 * DBTs in a multi-buffer are out of scope.  This occurs after child
 * processing is complete.
 */
class NsEventReaderBuf
{
public:
	NsEventReaderBuf();
	// data
	NsEventReaderBuf *reuseNext;
	NsEventReaderBuf *freeNext;
	uint32_t nNodes;
	uint32_t nComplete;
	uint32_t nSize;
	bool usedUp;
	void *p;
	DbXmlDbt dbt;
};

enum state {
	TEXT,
	START_ELEMENT,
	CHILD_ELEMENT,
	END_ELEMENT,
	POP
};
	
class NsEventReaderNodeList
{
public:
	NsEventReaderNodeList(NsEventReaderNodeList *parent,
			      NsEventReaderBuf *currentBuf)
		: buffer(currentBuf),
		  textDone(0), state(0), started(false), parent(parent) {
		memset(&curText, 0, sizeof(curText));
	}

	int getTextIndex() const { return textDone; }
	nsTextEntry_t *getTextEntry() { return &curText; }
	void incrText() {
		// transition state when done with either
		// leading text or child text
		++textDone;
		int ntext = node.getNumText();
		if (!node.hasTextChild()) {
			if (textDone == ntext)
				state = START_ELEMENT;
		} else {
			if (textDone == ntext)
				state = END_ELEMENT;
			else if (textDone ==
				 (ntext - (int)node.getNumChildText()))
				state = START_ELEMENT;
		}
	}
	void setStartElemDone() {
		started = true;
		if (node.hasElemChild())
			state = CHILD_ELEMENT;
		else if (node.hasTextChild())
			state = TEXT;
		else
			state = END_ELEMENT;
	}
	void setEndElemDone() {
		state = POP;
	}
	void setChildElemDone() {
		if (node.hasTextChild())
			state = TEXT;
		else
			state = END_ELEMENT;
	}
	bool hasSibling() {
		return (node.hasNext());
	}
	int getState() const { return state; }
	void setState(bool init);
	bool getStarted() const { return started; }
public:
	NsRawNode node;
	NsEventReaderBuf *buffer;
	int textDone;
	nsTextEntry_t curText;
	int state;
	bool started;
	NsEventReaderNodeList *parent;
};

void NsEventReaderNodeList::setState(bool init)
{
	if (node.hasText()) {
		if (init) {
			// skip leading text if this is
			// the starting node
			textDone +=
				(node.getNumText() -
				 node.getNumChildText());
		} else {
			if (node.getNumText() >
			    node.getNumChildText()) {
				state = TEXT;
				return;
			}
		}
	}
	state = START_ELEMENT; 
}
	
}

using namespace DbXml;
using namespace std;

NsEventReader::NsEventReader(
	Transaction *txn, DbWrapper *db, DictionaryDatabase *ddb,
	const DocID &docId,
	int cid, u_int32_t flags, uint32_t bufferSize,
	const NsNid *startId, CacheDatabase *cdb)
	: localName_(0),
	  emptyElement_(false),
	  document_(0),
	  doInit_(true),
	  popElement_(false),
	  entityCount_(0),
	  current_(NULL),
	  currentBuffer_(NULL),
	  cursor_(*db, txn, CURSOR_READ, "NsEventReader", flags),
	  cursorFlags_(DB_CURSOR_GET_MASK(*db,flags)),
	  reuseList_(NULL),
	  freeList_(NULL),
	  docdb_(cdb),
	  bufferSize_(bufferSize)
{
	DBXML_ASSERT(db);

	if(db->getPageSize() > bufferSize_) {
		bufferSize_ = db->getPageSize();
	}

	document_.initDoc(txn, db, ddb, docId, cid, flags);

	// set starting NID.  If NULL, start at beginning
	startId_.clear();
	if (startId) {
		startId_.copyNid(startId->getBytes(),
				 startId->getLen());
	} else {
		startId_.setDocRootNid();
	}

	// do the first event separately
	getNode(current_);
	if (hasNext_) // handle no-content docs
		(void) doElement(true /*start*/);
}

NsEventReader::NsEventReader(NsDoc &nsDoc, uint32_t bufferSize,
	const NsNid *startId, CacheDatabase *cdb)
	: localName_(0),
	  emptyElement_(false),
	  document_(0),
	  doInit_(true),
	  popElement_(false),
	  entityCount_(0),
	  current_(NULL),
	  currentBuffer_(NULL),
	  cursor_(*(nsDoc.getDocDb()), nsDoc.getTxn(),
		  CURSOR_READ, "NsEventReader", nsDoc.getFlags()),
	  cursorFlags_(DB_CURSOR_GET_MASK(*(nsDoc.getDocDb()),nsDoc.getFlags())),
	  reuseList_(NULL),
	  freeList_(NULL),
	  docdb_(cdb),
	  bufferSize_(bufferSize)
{
	if(nsDoc.getDocDb()->getPageSize() > bufferSize_) {
		bufferSize_ = nsDoc.getDocDb()->getPageSize();
	}

	document_.initDoc(nsDoc.getTxn(),
			  nsDoc.getDocDb(),
			  nsDoc.getDictionaryDatabase(),
			  nsDoc.getDocID(),
			  nsDoc.getContainerID(),
			  nsDoc.getFlags());

	// set starting NID.  If NULL, start at beginning
	startId_.clear();
	if (startId) {
		startId_.copyNid(startId->getBytes(),
				 startId->getLen());
	} else {
		startId_.setDocRootNid();
	}

	try {
		// do the first event separately
		getNode(current_);
		if (hasNext_) // handle no-content docs
			(void) doElement(true /*start*/);
	} catch (...) {
		cleanup();
		throw;
	}
}

NsEventReader::~NsEventReader()
{
	cleanup();
}

void NsEventReader::close()
{
	delete this;
}

void NsEventReader::cleanup()
{
	while(current_ != 0) {
		NsEventReaderNodeList *tmp = current_;
		current_ = tmp->parent;

		if(tmp->buffer)
			releaseNode(tmp->buffer);
		delete tmp;
	}

	while (freeList_) {
		NsEventReaderBuf *cur = freeList_;
		freeList_ = cur->freeNext;
		NsUtil::deallocate(cur);
	}
	startId_.freeNid();
	// make sure the cursor is closed before the CacheDatabase (if present)
	// is closed...
	cursor_.close();
}

//
// XmlEventReader methods
//

//
// generic methods
//

//
// names and values
//
// Depend on type:
//  o names of attributes and elements are what is expected
//  o value is generally what is expected.
//  o Special cases:
//      ProcessingInstruction -- local name is target, value is data
//    

const unsigned char *
NsEventReader::getNamespaceURI() const
{
	if (!localName_ || type_ == ProcessingInstruction)
		return 0;
	DBXML_ASSERT(getNode() && getNode() == &current_->node);


	if (!getNode()->hasURI())
		return 0;

	return (const unsigned char *)
		document_.getStringForID(getNode()->getURIIndex());
}

const unsigned char *
NsEventReader::getLocalName() const
{
	return localName_;
}

const unsigned char *
NsEventReader::getPrefix() const
{
	if (!localName_)
		return 0;
	DBXML_ASSERT(getNode() && getNode() == &current_->node);
	int32_t prefix = getNode()->getPrefixIndex();
	if (prefix == NS_NOPREFIX)
		return 0;
	return 	(const unsigned char *) document_.getStringForID(prefix);
}
	
const unsigned char *
NsEventReader::getValue(size_t &len) const
{
	if (type_ == ProcessingInstruction) {
		if (value_)
			len = ::strlen((const char *)value_);
		else
			len = 0;
		return value_;
	} else {
		if (!value_)
			throwIllegalOperation(type_, "getValue");
		len = valueLen_;
		return value_;
	}
}

// This method works for text strings and attributes values
bool
NsEventReader::needsEntityEscape(int index) const
{
	if (type_ == Characters) {
		return MAKEBOOL(textType_ & NS_ENTITY_CHK);
	} else if (type_ == StartElement) {
		ensureAttributes(index, "needsEntityEscape");
		fetchAttributes(index);
		return ((attr_.a_flags & NS_ATTR_ENT) != 0);
	} else if (type_ == CDATA)
		return false;
	throwIllegalOperation(type_, "needsEntityEscape");
	return true;
}

bool
NsEventReader::isEmptyElement() const {
	ensureType(StartElement, "isEmptyElement");
	return emptyElement_;
}

const unsigned char *
NsEventReader::getVersion() const
{
	ensureType(StartDocument, "getVersion");
	return (const unsigned char*) document_.getXmlDecl();
}

const unsigned char *
NsEventReader::getEncoding() const
{
	ensureType(StartDocument, "getEncoding");
	return document_.getEncodingStr();
}

bool
NsEventReader::isStandalone() const
{
	// if there is a standalone string, it's easy,
	// but if not, there is no way to know.  The document
	// may or may not have an external DTD ref, so
	// default to 'no'
	// TBD: think about keeping standalone state during
	// initial parse, as a document-level flag.
	ensureType(StartDocument, "isStandalone");
	const xmlbyte_t *sa = document_.getStandaloneStr();
	if (sa && (::memcmp(sa, "yes", 3) == 0))
		return true;
	return false;
}

bool
NsEventReader::standaloneSet() const
{
	ensureType(StartDocument, "standaloneSet");
	if (document_.getStandaloneStr())
		return true;
	return false;
}

bool
NsEventReader::encodingSet() const
{
	ensureType(StartDocument, "encodingSet");
	if (document_.getEncodingStr())
		return true;
	return false;
}

//
// attribute access (includes namespace attributes)
// node_ will be 0 if no attributes are present
//

int
NsEventReader::getAttributeCount() const
{
	ensureType(StartElement, "getAttributeCount");
	return nattrs_;
}

void
NsEventReader::fetchAttributes(int index) const
{
	if (index != attrIndex_) {
		curAttrName_ = getNode()->getNextAttr(curAttrName_,
						  &attr_, index);
		attrIndex_ = index;
	}
}

bool
NsEventReader::isAttributeSpecified(int index) const
{
	ensureAttributes(index, "isAttributeSpecified");
	fetchAttributes(index);
	bool notSpecified = (attr_.a_flags & NS_ATTR_NOT_SPECIFIED) != 0;
	return (!notSpecified);
}

const unsigned char *
NsEventReader::getAttributeLocalName(int index) const
{
	ensureAttributes(index, "getAttributeLocalName");
	fetchAttributes(index);
	return curAttrName_;
}

const unsigned char *
NsEventReader::getAttributeNamespaceURI(int index) const
{
	ensureAttributes(index, "getAttributeNamespaceURI");
	fetchAttributes(index);
	int32_t uri = attr_.a_uri;
	if (uri == NS_NOURI)
		return 0;
	return 	(const unsigned char *) document_.getStringForID(uri);
}

const unsigned char *
NsEventReader::getAttributePrefix(int index) const
{
	ensureAttributes(index, "getAttributePrefix");
	fetchAttributes(index);
	int32_t prefix = attr_.a_name.n_prefix;
	if (prefix == NS_NOPREFIX)
		return 0;
	return (const unsigned char *) document_.getStringForID(prefix);
}

const unsigned char *
NsEventReader::getAttributeValue(int index) const
{
	ensureAttributes(index, "getAttributeValue");
	fetchAttributes(index);
	return (const unsigned char *)attr_.a_value;
}

XmlEventReader::XmlEventType
NsEventReader::next()
{
	if (!hasNext_)
		throw XmlException(
			XmlException::EVENT_ERROR,
			"XmlEventReader::next() called when hasNext() is false");
	if(doInit_) {
		// first event is done...
		doInit_ = false;
		if ((current_ == NULL)
		    || ((current_->parent == NULL) &&
			(popElement_ || emptyElement_))
			) {
			hasNext_ = false;
			cursor_.close();
		}
		current_->setStartElemDone();
		return type_;
	} else if (popElement_) {
		// this logic keeps the current element info
		// available through the EndElement event
		endElement();
		if (current_ == NULL) {
			// there is no next...
			throw XmlException(
				XmlException::EVENT_ERROR,
				"XmlEventReader::next() called when hasNext() is false");
		}
	}

	// Basic node loop:
	// 1.  leading text nodes (siblings)
	// 2.  start this element
	// 3.  if has child elements, get next node and process
	// 4.  do child text
	// 5.  end this element
	// 6.  if nextSibling, get next node, else pop to parent
	// 7.  release this node
	//
	while (current_ != NULL) {
		switch (current_->getState()) {
		case TEXT:
		{
			if (!doText())
				continue; // skip
			break;
		}
		case START_ELEMENT:
		{
			// if the current node has been started, need another
			if (current_->getStarted())
				getNode(current_);
			(void) doElement(/* start */true);
			current_->setStartElemDone();
			if (entityCount_ != 0)
				continue; // eat this event
			break;
		}
		case CHILD_ELEMENT:
		{
			current_->setChildElemDone();
			getNode(current_);
			continue;
		}
		case END_ELEMENT:
		{
			current_->setEndElemDone();
			bool validEvent = doElement(/* start */false);
			if (validEvent)
				return type_;
			// else fall through
		}
		case POP:
		default:
		{
			// pop to parent or next sibling.
			endElement();
			continue;
		}
		}
		return type_;
	}
	return type_;
}

// initialize state for a text event.
// Type may be: Characters, CDATA, Comment, Whitespace,
//   EntityReference, ProcessingInstruction, or DTD
// NOTES:
//  o Whitespace means ignorable whitespace
//  o CDATA will never be ignorable white space
// Returns true on "success" and false if the event
// needs to be skipped (e.g. start/end entity info
// that is not being reported)
bool
NsEventReader::doText()
{
	const nsTextEntry_t *entry =
		current_->node.getTextEntry(current_->getTextEntry(),
					    current_->getTextIndex());
	current_->incrText();

	// initialize text event state
	localName_ = 0;  // not valid for text
	value_ = (unsigned char*)entry->te_text.t_chars;
	valueLen_ = entry->te_text.t_len;
	textType_ = entry->te_type;

	switch (nsTextType(textType_)) {
	case NS_TEXT:
		if(textType_ & NS_IGNORABLE)
			type_ = Whitespace;
		else
			type_ = Characters;
		break;
	case NS_CDATA:
		DBXML_ASSERT(!(textType_ & NS_IGNORABLE));
		type_ = CDATA;
		break;
	case NS_COMMENT:
		type_ = Comment;
		break;
	case NS_PINST:
		// PI is encoding as name0data
		// return target in localName_, data in value_
		type_ = ProcessingInstruction;
		localName_ = value_;
		while (*value_++); // get past NULL
		break;
	case NS_SUBSET:
		type_ = DTD;
		break;
	case NS_ENTSTART:
		if (!(expandEntities_ || reportEntityInfo_))
			throw XmlException(
				XmlException::EVENT_ERROR,
				"Configuration error: must either expand or report entity information");
		if (!expandEntities_)
			++entityCount_;
		if (reportEntityInfo_) {
			type_ = StartEntityReference;
			return true;
		} else
			return false; // skip
		break;
	case NS_ENTEND:
		if (!expandEntities_)
			--entityCount_;
		if (reportEntityInfo_) {
			type_ = EndEntityReference;
			return true;
		} else
			return false; // skip
		break;
	default:
		DBXML_ASSERT(0); // bad
	}
	// if entityCount is 0, we're not eating events,
	// so return true
	return (entityCount_ == 0);
}

void
NsEventReader::getNode(NsEventReaderNodeList *parent)
{
	DbXmlDbt key;
	DbXmlDbt data;
	current_ = new NsEventReaderNodeList(parent, currentBuffer_);
	try {
		if(doInit_) {
			nextNode(&current_->buffer, key, data, &startId_);
			if(current_->buffer == 0) {
				hasNext_ = false; // no-content doc
				return;
			}
		} else {
			nextNode(&current_->buffer, key, data);
			DBXML_ASSERT(current_->buffer != 0);
			DBXML_ASSERT(data.data);
		}
		currentBuffer_ = current_->buffer;
		
		current_->node.setNode(key, data);
		current_->setState(doInit_);
		DBXML_ASSERT(current_->node.getDocID() == document_.getDocID());
	} catch (...) {
		delete current_;
		current_ = 0;
		throw;
	}

	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		NsRawNode *n = &current_->node;
		// raw node so cannot log the actual node state
		NsFormat::logNodeOperation(*document_.getDocDb(),
					   n->getDocID(), 0,
					   n->getNodeID(), &data,
					   "EventReader");
	}
}

// returns false either on end of document, or after
// end of empty element, which triggers the caller to skip
// the event
bool
NsEventReader::doElement(bool start)
{
	// initialize state
	value_ = 0;
	setNode(0);
	attrIndex_ = -1;
	curAttrName_ = 0;
	// current_ is set by caller
	uint32_t flags = current_->node.getFlags();
	if (flags & NS_ISDOCUMENT) {
		localName_ = 0;
		if(start) {
			// TBD: not using sniffed encoding
			type_ = StartDocument;
		} else {
			type_ = EndDocument;
			popElement_ = true;
		}
	} else {
		emptyElement_ =
			!((flags & NS_HASCHILD) || (flags & NS_HASTEXTCHILD));
		// skip EndElement for empty elements
		if (emptyElement_ && !start)
			return false;

		// even for EndElement, reassign names
		setNode(&current_->node);
		localName_ = getNode()->getNodeName();
		if(start) {
			type_ = StartElement;
			nattrs_ = getNode()->getNumAttrs();
		} else {
			type_ = EndElement;
			popElement_ = true;
		}
	}
	if (entityCount_ != 0)
		return false; // skip this
	// set hasNext_
	if (!doInit_) {
		if ((current_ == NULL)
		    || ((current_->parent == NULL) &&
			(popElement_ || emptyElement_))
			) {
			hasNext_ = false;
			cursor_.close();
		}
	}
	return true;
}
//
// if startId is non-null, it's the first time, so
// do a GET vs NEXT cursor op.  *bufp will be NULL.
//
// No-content documents: it's possible to get here with a document
// ID that is not in the database, so silently handle such failures.
//

void
NsEventReader::nextNode(NsEventReaderBuf **bufp, DbXmlDbt &key, DbXmlDbt &data, NsFullNid *startId)
{
	NsEventReaderBuf *buf = *bufp;
	DBXML_ASSERT(buf || startId);
	uint32_t bufSize = bufferSize_;
	// are there any items left?
	if(buf) {
		DB_MULTIPLE_KEY_NEXT(buf->p, &buf->dbt, key.data, key.size, data.data, data.size);
		if (buf->p == 0) {
			buf->usedUp = true;
			if (buf->nComplete == buf->nNodes) {
				buf->reuseNext = reuseList_;
				reuseList_ = buf;
			}
			buf = 0;
		}
	}
	
	while (!buf) {
		// "construct" the object plus data
		// pull off free list if available
		if (reuseList_ && (reuseList_->nSize >= bufSize)) {
			buf = reuseList_;
			reuseList_ = buf->reuseNext;
		} else {
			buf = (NsEventReaderBuf *)
				NsUtil::allocate(bufSize +
						 sizeof(NsEventReaderBuf));
			buf->nSize = bufSize;
			buf->freeNext = freeList_;
			freeList_ = buf;
			// placement new to initialize DbXmlDbt
			(void) new (&(buf->dbt)) DbXmlDbt();
		}
		buf->reuseNext = 0;
		buf->nNodes = 0;
		buf->nComplete = 0;
		buf->usedUp = false;

		buf->dbt.set_flags(DB_DBT_USERMEM);
		buf->dbt.ulen = buf->nSize;
		buf->dbt.data = buf + 1;
		int ret = 0;
		try {
			if (startId) {
				NsFormat::marshalNodeKey(document_.getDocID(),
							 NsNid(startId), docKey_);
				ret = cursor_.get(
					docKey_,
					buf->dbt,
					cursorFlags_|DB_SET|DB_MULTIPLE_KEY);
			} else {
				ret = cursor_.get(
					docKey_,
					buf->dbt,
					cursorFlags_|DB_NEXT|DB_MULTIPLE_KEY);
			}
		} catch (XmlException &xe) {
			ret = xe.getDbErrno();
		}
		if (ret != 0) {
			u_int32_t dbt_size = buf->dbt.size;
			// if buf was just allocated, free it; otherwise,
			// put it back on the reuse list
			if (freeList_ == buf) {
				freeList_ = buf->freeNext;
				NsUtil::deallocate(buf);
			} else {
				buf->reuseNext = reuseList_;
				reuseList_ = buf;
			}
			if (ret == DB_BUFFER_SMALL) {
				// necessary size is in buf->dbt.size
				while (bufSize < dbt_size)
					bufSize <<= 1;
				buf = 0;
				continue;
			} else if(ret != DB_NOTFOUND) {
				throw XmlException(ret, __FILE__, __LINE__);
			}

			if(startId) {
				// no-content document
				buf = 0;
			}
			*bufp = buf; // 0
			return;
		}
		// initialize multiple iter
		DB_MULTIPLE_INIT(buf->p, &buf->dbt);
		DB_MULTIPLE_KEY_NEXT(buf->p, &buf->dbt, key.data, key.size, data.data, data.size);
		if (buf->p == 0) {
			NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
						 "Failed to find node.",
						 __FILE__, __LINE__);
			return;
		}
		*bufp = buf;
	}
#ifdef FORMAT_DEBUG
	if (_dumpFormatNodes) {
		unsigned char *datap = (unsigned char *)data.data;
		const NsFormat &fmt = NsFormat::getFormat((int)(*datap));
		NsNode *nsNode = fmt.unmarshalNodeData(
			datap, /*adoptBuffer*/false);
		nsNode->acquire();
		nsNode->release();
	}
#endif

	buf->nNodes++;
}

// reset current_ to either the parent of the current node, or
// get the next sibling, remembering to set the appropriate parent
// return true if popping to parent, false if moving to sibling.
void
NsEventReader::endElement()
{
	popElement_ = false;
	NsEventReaderNodeList *tmp = current_;

	if (current_->hasSibling()) {
		getNode(tmp->parent);
	} else
		current_ = tmp->parent;
	releaseNode(tmp->buffer);
	delete tmp;
}
	
void
NsEventReader::releaseNode(NsEventReaderBuf *buf)
{
	buf->nComplete++;
	// it's not available until 1) all records have
	// been used up, and 2) all nodes it includes are
	// done processing
	if (buf->usedUp && (buf->nComplete == buf->nNodes)) {
		// done -- put it on free list
		buf->reuseNext = reuseList_;
		reuseList_ = buf;
	}
}

