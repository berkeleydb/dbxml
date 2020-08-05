//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
#include "../DbXmlInternal.hpp"
#include "NsNid.hpp"
#include "NsUpdate.hpp"
#include "NsFormat.hpp"
#include "NsDom.hpp"
#include "NsEventWriter.hpp"
#include "EventReader.hpp"
#include "EventReaderToWriter.hpp"
#include <xqilla/context/DynamicContext.hpp>
#include "../dataItem/DbXmlConfiguration.hpp"
#include <dbxml/XmlManager.hpp>
#include <dbxml/XmlUpdateContext.hpp>
#include "../Document.hpp"
#include "../UpdateContext.hpp"
#include "../TypeConversions.hpp"
#include "../UTF8.hpp"
#include "NsReindexer.hpp"

using namespace DbXml;
using namespace std;

namespace DbXml {

class AutoIndexWriter : public EventWriter {
public:
	AutoIndexWriter() : is_(0) {}
	void setIndexSpecification(IndexSpecification *is) { is_ = is; }
	virtual void close() { delete this; }
	// methods required by EventWriter
	virtual void writeEndElementWithNode(const unsigned char *localName,
					     const unsigned char *prefix,
					     const unsigned char *uri,
					     IndexNodeInfo *ninfo);
	// this one is used only to get the attributes
	virtual void writeStartElementWithAttrs(const unsigned char *localName,
						const unsigned char *prefix,
						const unsigned char *uri,
						int numAttributes,
						NsEventAttrList *attrs,
						IndexNodeInfo *ninfo,
						bool isEmpty);

	// this is a no-op
	virtual void writeTextWithEscape(XmlEventReader::XmlEventType type,
					 const unsigned char *chars,
					 size_t length,
					 bool needsEscape) {}
private:
	IndexSpecification *is_;
};

class NsQName {
public:
	NsQName(ATQNameOrDerived *qname) :
		name_(qname->getName()),
		uri_(qname->getURI()),
		prefix_(qname->getURI() ? qname->getPrefix() : 0) {}
public:
	XMLChToUTF8Null name_;
	XMLChToUTF8Null uri_;
	XMLChToUTF8Null prefix_;
};
}
 
typedef RewriteMap::const_iterator RewriteIter;
typedef UpdateMap::const_iterator MapIter;
typedef pair<MapIter,MapIter> MapRange;
static string makeKey(const NsNid &nid,
		      const DocID &did,
		      const std::string &cname);
static string makeKey(const DbXmlNodeImpl &node);
static void lookupNameIDs(NsQName &qname,
			  const ContainerBase *container,
			  OperationContext &oc,
			  int &uri,
			  int &prefix);

class BetweenNid : public NsNidGen {
public:
        BetweenNid() : firstOrAfter_(false) {}
	virtual ~BetweenNid() {}
	virtual void nextId(NsFullNid *id) {
		NsNid prev(preceding_.nid());
		NsNid next(following_ ? following_.nid() : NsNid());
		getBetweenNid(id, &prev, (next ? &next : 0), firstOrAfter_);
		// reset preceding to be new nid
	 	preceding_ = NsNid(id->getBytes());
		if (!first_)
			first_ = id;
	}
	NsNid getFirstNid() {
		DBXML_ASSERT(first_);
		return first_.nid();
	}
	static void getBetweenNid(NsFullNid *newNid,
				  const NsNid *prev,
				  const NsNid *next,
				  bool firstOrAfter);
public:
	NsFullNidWrap preceding_;
	NsFullNidWrap following_;
	NsFullNidWrap first_;
        bool firstOrAfter_;
};



#if 1 // 1 means use new algorithm

// number of bytes to extend each time; this can
// be changed if it is determined that it is excessive
// or not enough
#define NS_NID_EXTEND_BYTES 4

// midpoint is chosen as half way through 255
#define NS_NID_MIDPOINT 0x90

// used to pad interior bytes -- will never be the
// last byte of a NID
#define NS_PAD_BYTE 1

static bool addOne(NsFullNid &nid)
{
	xmlbyte_t *bytes = nid.getBytesForCopy();
	uint32_t len = nid.getLen();
	DBXML_ASSERT(len >= 2);
	xmlbyte_t *ptr = &(bytes[len-2]);
	if (*ptr == NS_ID_LAST) {
		while (*ptr == NS_ID_LAST) {
			*ptr-- = NS_ID_FIRST;
			if (ptr == bytes) return false;
		}
	}
	++(*ptr);
	return true;
}

//
// a NID may look like this:
// 4 5 2 1 1 1 1  or this:
// 4 5 2 1 1 1 2
// In both cases the result should be:
// 4 5 1 f f f f
// It means treating the NS_ID_FIRST (2) differently when it's
// at the end vs internal
//
static void subtractOne(NsFullNid &nid)
{
	xmlbyte_t *bytes = nid.getBytesForCopy();
	uint32_t len = nid.getLen();
	DBXML_ASSERT(len >= 2);
	xmlbyte_t *ptr = &(bytes[len-2]);
	if (*ptr == NS_ID_FIRST || *ptr == NS_PAD_BYTE) {
		// first time only
		*ptr-- = NS_ID_LAST;
		if (ptr != bytes) {
			// now treat NS_ID_FIRST as any normal value to allow
			// NS_PAD_BYTE as a legitimate internal value
			while (*ptr == NS_PAD_BYTE) {
				*ptr-- = NS_ID_LAST;
				if (ptr == bytes) break;
			}
		}
	}
	--(*ptr);
}

static void padNid(NsFullNid &toPad, uint32_t len,
		   xmlbyte_t padByte = NS_PAD_BYTE)
{
	xmlbyte_t *bytes = (xmlbyte_t *)::malloc(len);
	uint32_t toLen = toPad.getLen();
	memcpy(bytes, toPad.getBytes(), toLen);
	uint32_t i = toLen - 1;
	while (i < (len - 1))
		bytes[i++] = padByte;
	bytes[i] = 0; // null terminate
	toPad.copyNid(bytes, len);
	::free(bytes);
}

static void
extend(NsFullNid *newNid,
       const NsNid *prev,
       const NsNid *next)
{
	// simple algorithm for now:
	// o use "prev" as starting point
	// o pad prev to same length as next if necessary
	// o extend NS_NID_EXTEND_BYTES using a midpoint value
	//
	// TBD -- consider extending using a different value based
	// on type of insertion.  e.g.
	//  insertLast or insertBefore might use a smaller value while
	//  insertFirst/insertAfter might use a larger one
	// Using a mid-point means that the worst-case scenario is not so
	// bad even though 1/2 of the density of the best-case is lost
	
	newNid->copyNid(prev->getBytes(), prev->getLen());
	// pad prev to same length as next if they have
	// the same number of digits before decimal, then
	// extend with a midpoint value.
	if ((newNid->getLen() < next->getLen()) &&
	    (newNid->getBytesBeforeDecimal() ==
	     next->getBytesBeforeDecimal()))
		padNid(*newNid, next->getLen());
	// extend using a mid-point value
	DBXML_ASSERT(newNid->getLen() != 0);
	padNid(*newNid, newNid->getLen() +
	       NS_NID_EXTEND_BYTES, NS_NID_MIDPOINT);
	return;
}

static bool add(NsFullNid &nid, const NsNid *prev, const NsNid *next)
{
	// start with prev and do simple add
	// start non-destructive
	NsFullNidWrap tnidw;
	NsFullNid &tnid = *tnidw;
	tnid.copyNid(prev->getBytes(), prev->getLen());
	// if new value is < next we're good
	if (addOne(tnid) &&
	    (NsFullNid::compare(tnid.getBytes(), next->getBytes()) < 0)) {
		nid.copyNid(&tnid);
		return true;
	}
	// need to start back at old value
	tnid.copyNid(prev->getBytes(), prev->getLen());	
	if ((prev->getLen() < next->getLen()) &&
	     (prev->getBytesBeforeDecimal() ==
	      next->getBytesBeforeDecimal())) {
		
		// try to pad then add and if < also good
		padNid(tnid, next->getLen());
		// need to add because the pad byte is not a valid last byte
		if (addOne(tnid) &&
		    (NsFullNid::compare(tnid.getBytes(),
					next->getBytes()) < 0)) {
			nid.copyNid(&tnid);
			return true;
		}
	}
	return false;
}

static bool sub(NsFullNid &nid, const NsNid *prev, const NsNid *next)
{
	NsFullNidWrap tnidw;
	NsFullNid &tnid = *tnidw;
	tnid.copyNid(next->getBytes(), next->getLen());
	subtractOne(tnid);
	// if new value is > prev we're good
	if (NsFullNid::compare(tnid.getBytes(), prev->getBytes()) > 0) {
		nid.copyNid(&tnid);
		return true;
	}
	// try pad then add and if > also good
	// need to start back at old value.
	tnid.copyNid(next->getBytes(), next->getLen());	
	if ((prev->getLen() > next->getLen()) &&
	    (prev->getBytesBeforeDecimal() ==
	     next->getBytesBeforeDecimal())) {
		// try to pad then subtract and if < also good
		padNid(tnid, prev->getLen());
		subtractOne(tnid);
		if (NsFullNid::compare(tnid.getBytes(), prev->getBytes()) > 0) {
			nid.copyNid(&tnid);
			return true;
		}
	}
	return false;
}

//static
void BetweenNid::getBetweenNid(NsFullNid *newNid,
			       const NsNid *prev,
			       const NsNid *next,
			       bool firstOrAfter)
{
	uint32_t plen = prev->getLen();
	uint32_t nlen = next ? next->getLen() : 0;
	if(plen == 0 && nlen == 0) {
		memset(newNid, 0, sizeof(NsFullNid));
		return;
	}
	// maximum length of the between id is NS_NID_EXTEND_BYTES
	// greater than the maximum length of the
	// prev or next id
	uint32_t maxlen = plen > nlen ? plen : nlen;
	maxlen += NS_NID_EXTEND_BYTES;
	
	if (nlen == 0) {
		// need to free later
		xmlbyte_t *dest = (xmlbyte_t*)
			NsUtil::allocate(maxlen * sizeof(xmlbyte_t) + 1);
		// no next, so we must be appending to the end of
		// the document, just add a next nid to previous
		memcpy(dest, prev->getBytes(), plen);
		dest[maxlen] = 0;

		// eliminate everything after the decimal, and
		// allow for an extra digit
		dest[dest[0]+1] = 0;
		dest[dest[0]+2] = 0;

		// idDigit is 0-based, subtract one from num bytes
		uint32_t tdig = dest[0] - 1;
		_nextId(dest, tdig);
		newNid->copyNid(dest, (uint32_t)::strlen((const char *)dest) + 1);
		NsUtil::deallocate((void*)dest);
		return;
	}
	// need to find a between value
	// treat insertFirst and insertAfter the same
	// tread insertLast and insertBefore the same
	
	// see if a simple add/subtract works out
	if (firstOrAfter) {
		if (sub(*newNid, prev, next)) {
			goto out;
		}
	} else { // lastOrBefore
		// add
		if (add(*newNid, prev, next)) {
			goto out;
		}
	}
	newNid->clear();
	DBXML_ASSERT(NsFullNid::compare(prev->getBytes(), next->getBytes()) < 0);	
	extend(newNid, prev, next);
	
out:
	DBXML_ASSERT(newNid->getBytes()[newNid->getLen()-2] >= NS_ID_FIRST);	
	DBXML_ASSERT(NsFullNid::compare(newNid->getBytes(),
					prev->getBytes()) > 0);
	DBXML_ASSERT(NsFullNid::compare(newNid->getBytes(),
					next->getBytes()) < 0);
	return;
}

#else // old algorithm

static inline xmlbyte_t idbyte2num(xmlbyte_t idbyte)
{
	return (idbyte == 0 || idbyte == NS_ID_ZERO) ? 0 :
		idbyte + 1 - NS_ID_FIRST;
}

static inline xmlbyte_t num2idbyte(xmlbyte_t num)
{
	return num == 0 ? NS_ID_ZERO : num + NS_ID_FIRST - 1;
}

static inline xmlbyte_t nextIdbyteAsNum(const xmlbyte_t *&p)
{
	if(*p == 0) return 0;
	++p;
	return idbyte2num(*p);
}

#define POW_OF_2 3
static inline int avgbyte(int pb, int nb)
{
	int avg = (pb + nb) >> 1;
	for (int i = 0; avg != pb && i < POW_OF_2; i++) {
		avg = ((int)pb + (int)avg) >> 1;
	}
	return avg;
}


// generate id for this object bewteen prev and next
//static
void BetweenNid::getBetweenNid(NsFullNid *newNid,
			       const NsNid *prev,
			       const NsNid *next,
			       bool firstOrAfter)
{
	DBXML_ASSERT(prev);

	newNid->freeNid(); // free any existing ID

	uint32_t plen = prev->getLen();
	uint32_t nlen = next ? next->getLen() : 0;
	if(plen == 0 && nlen == 0) {
		memset(newNid, 0, sizeof(NsFullNid));
		return;
	}

	// maximum length of the between id is one
	// greater than the maximum length of the
	// prev or next id
	uint32_t maxlen = plen > nlen ? plen : nlen;
	maxlen += 1;

	// Allocate memory to build the id in
	xmlbyte_t *dest =
		(xmlbyte_t*) NsUtil::allocate(maxlen * sizeof(xmlbyte_t) + 1);
	if (nlen == 0) {
		// no next, so we must be appending to the end of
		// the document, just add a next nid to previous
		memcpy(dest, prev->getBytes(), plen);
		dest[maxlen] = 0;

		// eliminate everything after the decimal, and
		// allow for an extra digit
		dest[dest[0]+1] = 0;
		dest[dest[0]+2] = 0;

		// idDigit is 0-based, subtract one from num bytes
		uint32_t tdig = dest[0] - 1;
		_nextId(dest, tdig);
		newNid->copyNid(dest, (uint32_t)::strlen((const char *)dest) + 1);
		NsUtil::deallocate((void*)dest);
		return;
	}
	
	xmlbyte_t pb, nb;
	const xmlbyte_t *p = prev->getBytes();
	xmlbyte_t *d = dest;
	const xmlbyte_t *n = next->getBytes();
	
	// copy the indentical prefix of prev and next
	while((pb = *p) == (nb = *n)) {
		// This would mean next and prev were identical!
		DBXML_ASSERT(pb != 0);
		*d = pb;
		++d; ++p; ++n;
	}
	
	DBXML_ASSERT(nb > pb); // As next should be greater than prev
	
	pb = idbyte2num(pb);
	nb = idbyte2num(nb);
	// Special case where nb equals NS_ID_FIRST,
	// and pb equals 0 - in this case, idbyte2num
	// maps them both onto the number 0.
	while(pb == nb) {
		*(d++) = num2idbyte(nb);
		pb = nextIdbyteAsNum(p);
		++n;
		nb = idbyte2num(*n);
	}
	
	// Pick id so that it is half way between next and prev
	// NOTE: need a different algorithm for as first
        // and after to avoid the nids from growing
        // one byte after every insert [#17659]
        xmlbyte_t avg;
        if (firstOrAfter) {
                avg = ((int)pb + (int)nb) >> 1;
        } else {
	        avg = avgbyte((int)pb, (int)nb);
        }
	*(d++) = num2idbyte(avg);
	while(avg == pb) {
		pb = nextIdbyteAsNum(p);
                if (firstOrAfter) {
                        avg = ((int)pb + idbyte2num(NS_ID_LAST) + 1) >> 1;
                } else {
		        avg = avgbyte((int)pb, idbyte2num(NS_ID_LAST) + 1);  //old algorithm
                }
		*(d++) = num2idbyte(avg);
	}
	*(d++) = 0; // null terminate
	
	newNid->copyNid(dest, (uint32_t) (d - dest));
	NsUtil::deallocate(dest);
}


#endif


static void getNidRange(BetweenNid &between,
			NsDomNode *prev,
			NsDomNode *next,
			NsDomElement *parent,
                        bool firstOrAfter)
{
	// preceding is last desc of prev or the parent
	between.preceding_ = (prev ? prev->getLastDescendantNid() :
			      parent->getNodeId());

        // Insert node as first or after requires a different algorithm
        between.firstOrAfter_ = firstOrAfter;

	// following is either next or need to search for it...
	// it'll be empty if appending to the end of the document
	if (next) {
		between.following_ = next->getNodeId(); // set following
	} else {
		// search
		// don't put parent into pref because it's stack-allocated
		NsDomNodeRef pref;
		if (parent->hasElemNext()) {
			pref = parent->getElemNext();
			between.following_ = pref->getNodeId();
			return;
		}
		pref = parent->getNsParentNode();
		
		while (pref && !((NsDomElement*)pref.get())->hasElemNext())
			pref = pref->getNsParentNode();
		if (pref) {
			pref = ((NsDomElement*)pref.get())->getElemNext();
			DBXML_ASSERT(pref);
			between.following_ = pref->getNodeId();
		}
	}
}

//
// o next is optional, and if present, may be text...
// o return "fresh" (newly-fetched) nodes
// o a null return is expected if there is no previous element sibling
// o also find out if a "new" previous sibling is text, and if so, return it
//   for text entry moves
//
static NsDomNodeRef findPreviousElem(const NsDomNode *next,
				     NsDomElement &parent,
				     NsDomNodeRef &prevText,
				     DbWrapper &db, OperationContext &oc)
{
	NsDomNodeRef prevRef;
	if (next)
		prevRef = ((NsDomNode*)next)->getNsPrevSibling();

	if (prevRef && prevRef->getNsNodeType() != nsNodeElement)
		prevText = prevRef; // return prev text sibling
	
	while (prevRef && prevRef->getNsNodeType() != nsNodeElement)
		prevRef = prevRef->getNsPrevSibling();
	if (prevRef || next)
		return prevRef;
	// no next -- it's an append or insert into empty element
	return parent.getElemLastChild();
}

//
// o next is optional, and if present, may be text...
// o return "fresh" (newly-fetched) nodes
// o a null return is expected if there is no next element sibling
//
static NsDomNodeRef findNextElem(const NsDomNode *next,
				 const DocID &did,
				 DbWrapper &db, OperationContext &oc)
{
	NsDomNodeRef nextRef;
	if ((next) && (next->getNsNodeType() == nsNodeElement)) {
		// caller has already fetched new node in this case,
		// just duplicate
		return next->duplicate();
	} else if (next) {
		nextRef = (NsDomNode*)next;
		DBXML_ASSERT(nextRef->isTextType());
	}

	while (nextRef && nextRef->getNsNodeType() != nsNodeElement)
		nextRef = nextRef->getNsNextSibling();
	return nextRef;
}

static void initializeTextEntry(nsTextEntry &newText,
				const DbXmlNodeImpl &node)
{
	if (node.getNodeType() == nsNodePinst) {
		XMLChToUTF8 target(node.getPITarget());
		XMLChToUTF8 data(node.getValue());
		NsNode::createPI(&newText.te_text,
				 (const xmlbyte_t *)target.str(),
				 (const xmlbyte_t *)data.str(),
				 false, 0);
		newText.te_type = NS_PINST;
	} else {
		XMLChToUTF8 value(node.getValue());
		bool hasEnt = false;
		NsNode::createText(&newText.te_text,
				   (const xmlbyte_t *)value.str(),
				   value.len(), false, false,
				   &hasEnt);
		switch (node.getNodeType()) {
		case nsNodeText:
			newText.te_type = NS_TEXT;
			if (hasEnt)
				newText.te_type |= NS_ENTITY_CHK;
			break;
		case nsNodeCDATA:
			newText.te_type = NS_CDATA;
			break;
		case nsNodeComment:
			newText.te_type = NS_COMMENT;
			break;
		default:
			DBXML_ASSERT(false);
			break;
		};
	}
}

//
// public methods
//

NsUpdate::~NsUpdate()
{
	IndexInfoMap::iterator i = indexMap_.begin();
	while (i != indexMap_.end()) {
		delete i->second;
		*i++;
	}
}

/*
 * These next two functions special-case insertion of content into
 * no-content (empty) documents
 */
void NsUpdate::insertContentIntoEmptyDoc(const DbXmlNodeImpl &node,
					 Document &document,
					 OperationContext &oc,
					 DynamicContext *context)
{
        markForUpdate(&document, oc);
	DbWrapper &db = *(document.getDocDb());
	const DocID &did = document.getID();
			
	AutoDelete<EventReader> reader(node.getEventReader(context));
	DBXML_ASSERT(reader.get());
	AutoDelete<NsEventWriter> writer(
		new NsEventWriter(
			&db, document.getDictionaryDB(),
			did,  oc.txn()));
	// auto-indexing will be a no-op if not enabled
	IndexInfo *ii = getIndexInfo(document, oc);
	AutoIndexWriter aiw;
	if (ii) {
		aiw.setIndexSpecification(ii->getAutoIndexSpecification());
		writer->setBaseEventWriter(&aiw);
	}
	// run the events (do not donate objects to r2w)
	EventReaderToWriter r2w(*reader, (NsEventWriter&)*writer, false, false);
	// generate start doc event
	writer.get()->writeStartDocument(0,0,0);
	r2w.start();
	writer.get()->writeEndDocument();
}

bool NsUpdate::insertTextIntoEmptyDoc(const DbXmlNodeImpl &node,
				      Document &document,
				      OperationContext &oc,
				      DynamicContext *context)
{
        markForUpdate(&document, oc);
	bool retVal = true;
	DbWrapper &db = *(document.getDocDb());
	const DocID &did = document.getID();
			
	AutoDelete<NsEventWriter> writer(
		new NsEventWriter(
			&db, document.getDictionaryDB(),
			did,  oc.txn()));
	// Generate all events by hand, starting with startDocument
	writer.get()->writeStartDocument(0,0,0);
	switch (node.getNodeType()) {
	case nsNodePinst:
	{
		XMLChToUTF8 target(node.getPITarget());
		XMLChToUTF8 data(node.getValue());
		writer.get()->writeProcessingInstruction(
			(const unsigned char *)target.str(),
			(const unsigned char *)data.str());
		break;
	}
	case nsNodeText:
	case nsNodeCDATA:
	case nsNodeComment:
	{
		XMLChToUTF8 value(node.getValue());
		writer.get()->writeStartDocument(0,0,0);
		// TBD -- write will not accept this event at this time
		writer.get()->writeText(
			(node.getNodeType() == nsNodeComment ?
			XmlEventReader::Comment :
			 XmlEventReader::Characters),
			(const unsigned char *)value.str(), value.len());
		break;
	}
	default:
		DBXML_ASSERT(false); // can't happen
		break;
	}
	writer.get()->writeEndDocument();
	return retVal;
}

//
// Insert new element content.
// Called from insert before/after/into/as first/as last.
// o into is modeled as append (insert as last) because that's
//   most efficient for BDB XML
// o append/as last will set neither prev nor next
// o only one of next or prev is allowed
//
// Content is added using NsEventReader plugged into NsEventWriter,
// which (now) allows events to start in the middle of an existing
// document.
//
// Text entries and next/previous plumbing are handled separately.
//
// TBD: split up and/or simplify this function.  It's huge and complex...
//
void NsUpdate::insertElement(const DbXmlNodeImpl &node,
			     const DbXmlNodeImpl &parent,
			     const NsDomNode *next,
			     XmlManager &mgr,
			     Document &document,
			     OperationContext &oc,
			     DynamicContext *context,
                             bool firstOrAfter)
{
        markForUpdate(&document, oc);

	DbWrapper &db = *(document.getDocDb());
	const DocID &did = document.getID();

	// reindex.  This is necessary here in case insertion
	// affects indexes on parent nodes -- mixed content.  Second
	// boolean parameter of false means that the element will
	// only be fully generated if there's a value index involved;
	// this prevents unrelated nodes from getting visited.
	NsNidWrap vindexNid;
	removeElementIndexes(parent, document, /* updateStats */ false,
			     false, vindexNid, oc);

	// fetch fresh parent node for this update and turn it
	// into NsDom for convenience (NsDom will own NsNode and cleanup)
	NsNode *parentNode = 0;
	try {
		parentNode = fetchNode(parent, db, oc);
	} catch (XmlException &xe) {
		// if NOTFOUND treat this as an updateDocument to an
		// empty (no-content) document, except you can't do that, so...
		if ((xe.getExceptionCode() == XmlException::DATABASE_ERROR) &&
		    (xe.getDbErrno() == DB_NOTFOUND)) {
			insertContentIntoEmptyDoc(node, document, oc, context);
			// trigger reindexing later -- this is for a new doc
			NsNidWrap ignored;
			removeElementIndexes(parent, document, false, /*fullgen*/true,
					     ignored, oc);
			return;
		} else
			throw; // rethrow
	}
	NsDoc tdoc(oc.txn(), &db, document.getDictionaryDB(),
		   did, document.getContainerID(), 0 /* flags -- TBD */);
	NsDomElement parentDom(parentNode, &tdoc);

	// initialize reader and writer
	AutoDelete<EventReader> reader(node.getEventReader(context));
	DBXML_ASSERT(reader.get());
	AutoDelete<NsEventWriter> writer(
		new NsEventWriter(
			&db, document.getDictionaryDB(),
			did,  oc.txn()));
	// refresh next, if present
	if (next)
		const_cast<NsDomNode*>(next)->refreshNode(oc, true);
	// find next and previous elements, if they exist
	NsDomNodeRef previousText; // will hold prev text node, if exists
	NsDomNodeRef previousElem = findPreviousElem(next, parentDom,
						     previousText, db, oc);
	NsDomNodeRef nextElem = findNextElem(next, did, db, oc);
	
	// Calculate node id range for new nodes
	BetweenNid nidGen;
	getNidRange(nidGen, previousElem.get(),
		    nextElem.get(), &parentDom,
                    firstOrAfter);

	// set writer state for the partial insertion
	writer->setDepth(parentNode->getLevel());
	writer.get()->setNsNidGen(&nidGen);
	writer.get()->setCurrent(parentNode);
	writer.get()->setPrevious(previousElem ?
				  (NsNode*)previousElem->getNsNode() : 0);
	DBXML_ASSERT(!parentNode->getParent());
	// event writer will call NsDocument::completeNode() on
	// parent and previous, so bump the ref count
	parentNode->acquire();
	if (previousElem)
		((NsNode*)previousElem->getNsNode())->acquire();
        if (nextElem) {
                parentNode->setNoNav();
        }

	// auto-indexing will be a no-op if not enabled
	IndexInfo *ii = getIndexInfo(document, oc);
	AutoIndexWriter aiw;
	if (ii) {
		aiw.setIndexSpecification(ii->getAutoIndexSpecification());
		writer->setBaseEventWriter(&aiw);
	}
	
	// run the events (do not donate objects to r2w)
	EventReaderToWriter r2w(*reader, (NsEventWriter&)*writer, false, false);
	r2w.start();
	writer.get()->end(); // force writing/cleanup  (will write parent node)


	// now handle "plumbing" -- NID links and text movement

	// if text entries move, they come from either the parent (child text)
	// or from a "next" element.  Moving text as well as setting next/prev
	// on a nextElem will update both the target and nextElem, so amortize
	// the costs of fetch/write
	int startTextIndex = 0;
	NsNode *ownerNode = 0;
	if ((!previousText) &&
	    (!next && parentNode->hasTextChild())) {
		// appending...
		previousText = parentDom.getNsLastChild();
		DBXML_ASSERT(previousText->getNsNodeType() != nsNodeElement);
		startTextIndex = parentNode->getFirstTextChildIndex();
		ownerNode = parentNode;
	}

	if (nextElem || previousText) {
		NsNode *nextElemNode = (nextElem ?
					(NsNode*)nextElem->getNsNode() : 0);
		NsNodeRef target = fetchNode(nidGen.getFirstNid(), did,
					     db, oc);
		// next/prev links
		if (nextElemNode)
			nextElemNode->setNextPrev(target.get());
		// now, text
		nsTextList_t *oldList = 0;
		bool freeTextList = false;
		if (previousText) {
			if (!ownerNode) {
				// owner is either next element or the parent
				ownerNode = (nextElemNode ? nextElemNode : parentNode);
				DBXML_ASSERT(ownerNode);
			}
			int endTextIndex = previousText->getIndex();
			const std::string &cname = document.getContainerName();
			oldList = coalesceTextNodes(ownerNode,
						    *target,
						    startTextIndex,
						    endTextIndex,
						    false,
						    document);
			// remove text from old node
			ownerNode->removeText(startTextIndex, endTextIndex);
			for (int i = startTextIndex; i <= endTextIndex; i++) {
				textRemoved(i, ownerNode->getNid(),
					    did, cname);
			}
			// NOTE: no need to track text inserted -- it's a new node
			// and can't be part of a further update

			// write node if not going to do it below
			if (ownerNode != nextElemNode)
				putNode(ownerNode, db, did, oc);
			freeTextList = true;
		}

		// write nodes
		if (nextElemNode)
			putNode(nextElemNode, db, did, oc);
		putNode(*target, db, did, oc);
		if (freeTextList)
			target->replaceTextList(oldList,
						true /*free oldlist*/);
	}
	
	// need to reindex new content
	
	markElement(elements_, (vindexNid.isNull() ? nidGen.getFirstNid(): vindexNid),
		    /* updateStats */ true,
		    document, /* attributes */ false,
		    /* fullgen */ true);

	// Update last descendants of parent's parent, but only
	// if this was an append
	if (!nextElem) {
		NsDomNodeRef ppRef = parentDom.getElemParent();
		NsFullNid *lastDescNid =
			(NsFullNid *)parentNode->getLastDescendantNid();
		// only update grandparent if parent does not have a 'next'
		// ppRef will be empty if parent id document element
		if (!parentDom.hasElemNext() && ppRef)
			updateLastDescendants(ppRef.get(),
				      lastDescNid,
					      db, did, oc);
	}
}

void NsUpdate::insertText(const DbXmlNodeImpl &node,
			  const DbXmlNodeImpl &parent,
			  const NsDomNode *next,
			  Document &document,
			  OperationContext &oc,
			  DynamicContext *context)
{
        markForUpdate(&document, oc);
	DbWrapper &db = *(document.getDocDb());
	const DocID &did = document.getID();

	// check for empty value
	const XMLCh *tval = node.getValue();
	if (!tval || !*tval)
		return;

	// reindex; need to update stats because sizes may change
	NsNidWrap vindexNid;
	removeElementIndexes(parent, document, /* updateStats */ true,
			     /*fullgen*/false, vindexNid, oc);
	markElement(elements_,
		    (vindexNid.isNull() ? parent.getNodeID() : vindexNid),
		    /* updateStats */ true,
		    document, /* attributes */ false,
		    /* fullgen */ false);

	// refresh next, if present
	if (next)
		const_cast<NsDomNode*>(next)->refreshNode(oc, true);
	NsDomNodeRef nextElem = (next ?
				 findNextElem(next, did, db, oc) :
				 (NsDomNodeRef)0);

	NsNodeRef targetNode = 0;

	try {
		// target for new text is either nextElem (if present),
		// which means leading text, or the parent -- child text
		targetNode = (nextElem ? nextElem->getNsNode() :
			      fetchNode(parent, db, oc));
	} catch (XmlException &xe) {
		// if NOTFOUND treat this as an updateDocument to an
		// empty (no-content) document, except you can't do that, so...
		if ((xe.getExceptionCode() == XmlException::DATABASE_ERROR) &&
		    (xe.getDbErrno() == DB_NOTFOUND)) {
			if (insertTextIntoEmptyDoc(node, document, oc, context))
				return;
			// else throw
		}
		throw; // rethrow
	}
	
	int index = -1;
	int insertIndex; // track this separately, using original index
	bool asChild = false;
	const std::string &cname = document.getContainerName();
	if (next && next->isTextType()) {
		insertIndex = next->getIndex();
		index = getTextIndex(insertIndex,
				     next->getNodeId(),
				     did,
				     cname);
		if (((NsDomText*)next)->isChildText())
			asChild = true;
	} else if (next) {
		// last leading text...
		insertIndex = index = targetNode->getNumLeadingText();
		
	} else { // trailing child text
		insertIndex = index = targetNode->getNumText();
		asChild = true;
	}

	// insert new text at "index" in target node
	nsTextEntry_t newText;

	initializeTextEntry(newText, node);
	
	nsTextList_t *oldTextList = insertInTextList(&newText,
						     index,
						     *targetNode,
						     false, asChild);
	textInserted(insertIndex, targetNode->getNid(),
		     did, cname);
	
	// Re-write the node, using new text list
	putNode(*targetNode, db, did, oc);

	// note for possible text node coalesce
	if (targetNode->getNumLeadingText() >= 2 ||
	    targetNode->getNumChildText() >= 2)
		markElement(textCoalesce_, targetNode->getNid(),
			    /* updateStats */ false, document, false,
			    /*ignored*/false);

	// Put things back the way they were for cleanup
	(void) targetNode->replaceTextList(oldTextList, true);
}

void NsUpdate::insertAttributes(
	const std::vector<const DbXmlNodeImpl*> &attributes,
	const DbXmlNodeImpl &parent,
	Document &document,
	OperationContext &oc,
	DynamicContext *context)
{
        markForUpdate(&document, oc);

	DbWrapper &db = *(document.getDocDb());
	const DocID &did = document.getID();
	NsDocument *doc =
		const_cast<Document*>(parent.getDocument())->getNsDocument();
	
	// fetch fresh node
	NsNodeRef parentNode = fetchNode(parent, db, oc);

	// reindex
	IndexInfo *ii = getIndexInfo(document, oc);
	NsReindexer reindexer(document, ii,
			      document.getManager().getImplicitTimezone(),
			      /* forDelete */true,
			      /* updateStats */ true);
	if (reindexer.willReindex()) {
		removeAttributeIndexes(reindexer, parentNode, document, oc);
		reindexer.saveStatsNodeSize(parentNode);
	}
	// allocate new list, copy parent's existing attrs, and
	// add new ones
	int numNewAttrs = (int)attributes.size();
	nsAttrList_t *oldList =
		parentNode->copyAttrList(numNewAttrs);

	IndexSpecification *is = (ii ? ii->getAutoIndexSpecification() : 0);
	
	for (int i = 0; i < numNewAttrs; i++) {
		const DbXmlNodeImpl *node = attributes[i];
		int idx = parentNode->addAttr(doc,
					      node->getPrefix(),
					      node->getUri(),
					      node->getLocalName(),
					      node->getValue(),
					      true);
		// Add auto-index entries for this attribute if enabled
		if (is) {
			const char *uri = doc->getStringForID(parentNode->attrUri(idx));
			const char *lname = (const char *)
				(parentNode->getAttrName(idx)->n_text.t_chars);
			Name aname(uri, lname);
			is->enableAutoAttrIndexes(aname.getURIName().c_str());
		}
	}

	// Re-write the node, using new attribute list
	putNode(*parentNode, db, did, oc);
	
	reindexer.updateStatsNodeSize(parentNode); // no-op if not necessary
	
	// Put things back the way they were for cleanup
	(void) parentNode->replaceAttrList(oldList, true);
}


void NsUpdate::removeDocument(const DbXmlNodeImpl &node,
			      Container &container,
			      XmlManager &mgr,
			      OperationContext &oc)
{
	// Remove entire document
	XmlUpdateContext uc = mgr.createUpdateContext();
	container.deleteDocumentInternal(
		oc.txn(),
		(Document&)*node.getDocument(),
		uc);
}

void NsUpdate::removeAttribute(const DbXmlNodeImpl &node,
			       Document &document,
			       OperationContext &oc,
			       DynamicContext *context)
{
        markForUpdate(&document, oc);

	// fetch fresh owner node
	DbWrapper *db = document.getDocDb();
	NsNodeRef parentNode = fetchNode(node, *db, oc);

	//reindex
	IndexInfo *ii = getIndexInfo(document, oc);
	NsReindexer reindexer(document, ii,
			      document.getManager().getImplicitTimezone(),
			      /* forDelete */true,
			      /* updateStats */ true);
	if (reindexer.willReindex()) {
		removeAttributeIndexes(reindexer, parentNode, document, oc);
#ifndef DBXML_DONT_DELETE_STATS
		reindexer.saveStatsNodeSize(parentNode);
#endif
	}
	
	// Find the index of the attribute to remove
	int oldindex = getAttributeIndex(node);

	// Update it
	parentNode->removeAttr(oldindex);

	// Note removal
	attributeRemoved(node);
	
	// Re-write the node
	putNode(*parentNode, *db, node.getDocID(), oc);
#ifndef DBXML_DONT_DELETE_STATS
	reindexer.updateStatsNodeSize(parentNode); // no-op if not necessary
#endif
}

void NsUpdate::removeElement(const DbXmlNodeImpl &node,
			     Document &document,
			     OperationContext &oc,
			     DynamicContext *context)
{
        markForUpdate(&document, oc);

	bool nextModified = false;
	bool prevModified = false;

	// fetch fresh node
	DbWrapper &db = *(document.getDocDb());
	const DocID &did = document.getID();
	NsNodeRef newNode = fetchNode(node, db, oc);
	// Create NsDomElement for navigational methods
	NsDomElement element(newNode.get(),
			     const_cast<Document*>(node.getDocument())->
			     getNsDocument());
	// Fetch nodes that may be modified (to start with)
	NsDomNodeRef parentRef = element.getElemParent();
	NsDomNodeRef nextRef = element.getElemNext();
	NsDomNodeRef prevRef = element.getElemPrev();
	
	// reindex
	NsNidWrap vindexNid;
	removeElementIndexes(node, document, /* updateStats */ true,
			     /*fullgen*/true, vindexNid, oc);
	if (vindexNid.isNull() ||
	    (vindexNid == newNode->getNid()))
	    vindexNid = parentRef->getNodeId();
	    
	markElement(elements_, vindexNid, /* updateStats */ false,
		    document, /* attributes */ false, /*fullgen*/false);
	
	NsNode *nextNode = 0;
	NsNode *prevNode = 0;
	NsNode *parentNode = 0;
	nsTextList_t *oldList = 0;
	bool freeTextList = false;
	NsNode *toNode = 0;
	if (nextRef) {
		nextModified = true;
		nextNode = (NsNode *)nextRef->getNsNode();
		if (prevRef) {
			prevModified = true;
			prevNode = (NsNode*)prevRef->getNsNode();
			nextNode->setNextPrev(prevNode);
		} else
			nextNode->clearPrev();
		// move any leading text
		if (newNode->hasLeadingText()) {
			oldList = coalesceTextNodes(*newNode,
						    nextNode,
						    -1, -1, // all leading text
						    false/* toChild */,
						    document);
			toNode = nextNode;
			freeTextList = true;
		}
	} else {
		parentNode = (NsNode*)parentRef->getNsNode();
		const NsFullNid *lastDescNid = 0;
		// this was last child element...
		if (prevRef) {
			prevModified = true;
			prevNode = (NsNode*)prevRef->getNsNode();
			prevNode->clearNext();
			// new last descendant is that of the previous node
			lastDescNid = prevNode->getLastDescendantNidOrSelf();
		} else {
			// no other children of parent; it's now a leaf
			lastDescNid = parentNode->getFullNid();
		}
		// prevNode of 0 clears parent's lastChild
		parentNode->setLastChild(prevNode);
		if (newNode->hasLeadingText()) {
			oldList = coalesceTextNodes(
				*newNode,
				parentNode,
				-1, -1, // all leading text
				true /* toChild */,
				document);
			toNode = parentNode;
			freeTextList = true;
		}
		// this updates the parent and all of its parents to the root
		updateLastDescendants(parentRef.get(),
				      lastDescNid,
				      db, did, oc);
		// doc node is not modified by updateLastDescendants;
		// handle case where root element is deleted.  Note:
		// document node may have another child element that replaced
		// the one being deleted.
		if (parentNode->isDoc())
			putNode(parentNode, db, did, oc);
	}
	// Remove node and subtree (combine w/indexing?)
	deleteTree(*newNode, db, did, oc);

	// Write prev/next if present and modified
	if (prevModified)
		putNode(prevNode, db, did, oc);
	if (nextModified)
		putNode(nextNode, db, did, oc);
	if (freeTextList) {
		DBXML_ASSERT(toNode);
		toNode->replaceTextList(oldList,
					true /*free oldlist*/);
	}
}

void NsUpdate::removeText(const DbXmlNodeImpl &node,
			  Document &document,
			  OperationContext &oc,
			  DynamicContext *context)
{
        markForUpdate(&document, oc);

	// reindex
	NsNidWrap vindexNid;
	removeElementIndexes(node, document, /* updateStats */ false,
			     /*fullgen*/false, vindexNid, oc);
	markElement(elements_,
		    (vindexNid.isNull() ? node.getNodeID() : vindexNid),
		    /* updateStats */ false,
		    document, /* attributes */ false,
		    /*fullgen*/false);
	
	// fetch fresh owner node
	DbWrapper *db = document.getDocDb();
	NsNodeRef newNode = fetchNode(node, *db, oc);

	// Find the index of the text to remove
	int oldindex = getTextIndex(node);

	// Update it
	newNode->removeText(oldindex, oldindex);

	// Note removal (use original index)
	textRemoved(node);

	// Re-write the node
	putNode(*newNode, *db, node.getDocID(), oc);

	// note for possible text node coalesce
	if (newNode->getNumLeadingText() >= 2 ||
	    newNode->getNumChildText() >= 2)
		markElement(textCoalesce_, newNode->getNid(), false,
			    document, false, /*ignored*/false);
}

void
NsUpdate::renameElement(const DbXmlNodeImpl &node,
			ATQNameOrDerived *qname,
			Document &document,
			OperationContext &oc,
			DynamicContext *context)
{
        markForUpdate(&document, oc);

	// reindex
	NsNidWrap vindexNid;
	removeElementIndexes(node, document, /* updateStats */ true,
			     /*fullgen*/true, vindexNid, oc);
	markElement(elements_,
		    (vindexNid.isNull() ? node.getNodeID() : vindexNid),
		    /* updateStats */ true,
		    document, /*attrs*/false, /*fullgen*/true);

	// fetch fresh node
	DbWrapper *db = document.getDocDb();
	NsNodeRef newNode = fetchNode(node, *db, oc);
	NsQName nqname(qname);

	// lookup uri and prefix if provided
	int uri = NS_NOURI;
	int prefix = NS_NOPREFIX;
	if (nqname.uri_.len())
		lookupNameIDs(nqname, node.getContainer(), oc,
			      uri, prefix);

	// Rename the element
	newNode->renameElement(nqname.name_.str(),
			       nqname.name_.len(),
			       uri, prefix);
	// Re-write the node
	putNode(*newNode, *db, node.getDocID(), oc);
}

//
// private instance methods
//
int NsUpdate::getAttributeIndex(const DbXmlNodeImpl &node) const
{
	int index = node.getIndex();
	int offset = 0;
	string key = makeKey(node);

	MapRange it = attrMap_.equal_range(key);
	for (MapIter i = it.first; i != it.second; i++) {
		if (i->second < index)
			offset++;
	}
	return index - offset;
}

// TBD: GMF -- figure out a better algorithm or data structure
// for handling inserts and deletes
int NsUpdate::getTextIndex(const DbXmlNodeImpl &node) const
{
	return getTextIndex(makeKey(node), node.getIndex());
}

int NsUpdate::getTextIndex(int index, const NsNid &nid,
			   const DocID &did, const std::string &cname)
{
	return getTextIndex(makeKey(nid, did, cname),
			    index);
}

int NsUpdate::getTextIndex(const std::string &key, int index) const
{
	int offset = 0;
	// subtract one for each deleted entry
	MapRange itdel = textDeleteMap_.equal_range(key);
        MapIter i;
	for (i = itdel.first; i != itdel.second; i++) {
		if (i->second < index)
			offset++;
	}

	// add one for each inserted entry
	MapRange itins = textInsertMap_.equal_range(key);
	for (i = itins.first; i != itins.second; i++) {
		if (i->second <= index)
			offset--;
	}
	return index - offset;
}

void NsUpdate::attributeRemoved(const DbXmlNodeImpl &node)
{
	string key = makeKey(node);
	attrMap_.insert(make_pair<const std::string, int>(key,node.getIndex()));
}

void NsUpdate::textRemoved(const DbXmlNodeImpl &node)
{
	string key = makeKey(node);
	textDeleteMap_.insert(make_pair<const std::string, int>(key,node.getIndex()));
}

void NsUpdate::textRemoved(int index, const NsNid &nid,
			   const DocID &did,
			   const std::string &cname)
{
	string key = makeKey(nid, did, cname);
	textDeleteMap_.insert(make_pair<const std::string, int>(key,index));
}

void NsUpdate::textInserted(int index, const DbXmlNodeImpl &node)
{
	string key = makeKey(node);
	textInsertMap_.insert(make_pair<const std::string, int>(key,index));
}

void NsUpdate::textInserted(int index, const NsNid &nid,
			    const DocID &did,
			    const std::string &cname)
{
	string key = makeKey(nid, did, cname);
	textInsertMap_.insert(make_pair<const std::string, int>(key,index));
}

//
// Insert a text entry into the text list for the
// "to" node at the specified index.  Options:
//  o replace -- replace existing entry
//  o asChild -- this is a (new) child entry (ignored
//    if replace is true)
//
nsTextList_t *NsUpdate::insertInTextList(nsTextEntry_t *text,
					 int index,
					 NsNode *to,
					 bool replace,
					 bool asChild)
{
	nsTextEntry_t *toEntries = 0;
	int numToChild = 0;
	int toIndex = 0;
	if (to->hasText()) {
		toEntries = to->getTextEntry(0);
		numToChild = to->getNumChildText();
	}

	int numText = to->getNumText();
	int numInserted = 0;
	if (!replace)
		++numText;
	nsTextList_t *newTextList = NsNode::createTextList(numText);
	try { // clean up on failure
		while (numInserted < numText) {
			if (numInserted == index) {
				// set NS_DONTDELETE to avoid copy.
				// new text is dynamically allocated, and the
				// property of the new text list
				NsNode::addText(
					newTextList,
					text->te_text.t_chars,
					text->te_text.t_len,
					text->te_type | NS_DONTDELETE,
					false /*not donated*/);
				// clear NS_DONTDELETE to ensure deallocation
				newTextList->tl_text[numInserted].te_type = text->te_type;
				numInserted++;
				continue;
			}
			if (toEntries) {
				if ((toIndex != index) || !replace) {
					NsNode::addText(
						newTextList,
						toEntries[toIndex].te_text.t_chars,
						toEntries[toIndex].te_text.t_len,
						toEntries[toIndex].te_type|NS_DONTDELETE,
						false /* ignored in this case */);
					numInserted++;
				}
				++toIndex;
			}
		}
		DBXML_ASSERT((int)newTextList->tl_ntext == numText);
	} catch (XmlException &xe) {
		NsNode::freeTextList(newTextList);
	}
	//
	// resulting from state change:
	// nchild and related flags
        //

	if (asChild && !replace)
		numToChild++;
	if (numToChild) {
		newTextList->tl_nchild = numToChild;
		to->setFlag(NS_HASTEXTCHILD);
	}
	to->setFlag(NS_HASTEXT);
	nsTextList_t *oldList = to->replaceTextList(newTextList);
	return oldList;
}

//
// Helper function to "move" text from one node to another.
// o if toChild, nodes are move to child text
// o if startIndex or endIndex is other than -1, use the value; otherwise
//   use all leading text
//
// 1. create a temporary text list with values from both
// "from" and "to"
// 2. Replace "to's" list with the new one, returning the old
// list to be replaced by the caller when the node has been
// written.
// 
nsTextList_t * NsUpdate::coalesceTextNodes(NsNode *from,
					   NsNode *to,
					   int startIndex,
					   int endIndex,
					   bool toChild,
					   Document &document)
{
	DBXML_ASSERT(from && to);
	if (startIndex == -1)
		startIndex = 0;
	if (endIndex == -1)
		endIndex = from->getNumLeadingText() - 1;
	uint32_t listIndex = 0;
	nsTextEntry_t *toEntries = 0;
	int numToChild = 0;
	if (to->hasText()) {
		toEntries = to->getTextEntry(0);
		numToChild = to->getNumChildText();
		if (toChild)
			listIndex = to->getFirstTextChildIndex();
	}

	int numFrom = endIndex - startIndex + 1;
	int numText = numFrom + to->getNumText();
	// create new text list to hold all the entries
	nsTextList_t *newTextList = NsNode::createTextList(numText);
	int toIndex = 0;
	int lastType = -1;
	try { // clean up on failure
		for (int i = 0; i < numText; i++) {
			if (i == (int) listIndex) {
				// add new entries
				nsTextEntry_t *fromEntries =
					from->getTextEntry(startIndex);
				for (int j = 0; j < numFrom; j++) {
					NsNode::addText(
						newTextList,
						fromEntries[j].te_text.t_chars,
						fromEntries[j].te_text.t_len,
						fromEntries[j].te_type,
						false /*not donated*/);
					textInserted(i + j,
						     to->getNid(),
						     document.getID(),
						     document.getContainerName());
					lastType = fromEntries[j].te_type;
				}
				i += numFrom - 1;
				continue;
			}
			if (toEntries) {
				// first node may be a candidate for coalescing
				if (nsTextType(lastType) == NS_TEXT &&
				    nsTextType(toEntries[toIndex].te_type) == NS_TEXT &&
				    (toChild || (toIndex != to->getFirstTextChildIndex()))) {
					// that's enough to warrant checking later
					markElement(textCoalesce_, to->getNid(), false,
						    document, false, /*ignored*/false);
				}
				lastType = -1;
				NsNode::addText(
					newTextList,
					toEntries[toIndex].te_text.t_chars,
					toEntries[toIndex].te_text.t_len,
					toEntries[toIndex].te_type|NS_DONTDELETE,
					false /* ignored in this case */);
				++toIndex;
			}
		}
		DBXML_ASSERT((int)newTextList->tl_ntext == numText);
	} catch (XmlException &xe) {
		NsNode::freeTextList(newTextList);
	}
	//
	// resulting from state change:
        //
	newTextList->tl_nchild = numToChild;
	if (toChild) {
		newTextList->tl_nchild += numFrom;
		to->setFlag(NS_HASTEXTCHILD);
	}
	to->setFlag(NS_HASTEXT);
	nsTextList_t *oldList = to->replaceTextList(newTextList);
	return oldList;
}

bool NsUpdate::coalesceEntry(nsTextList_t *list, nsTextEntry_t *entries, int &lastType, int index)
{
	bool ret = false;
	if (nsTextType(lastType) == NS_TEXT &&
	    nsTextType(entries[index].te_type) == NS_TEXT) {
		DBXML_ASSERT(index > 0);
		NsNode::coalesceText(list, entries[index].te_text.t_chars,
				     entries[index].te_text.t_len, false);
		// coalescing will always eliminate the NS_DONTDELETE flag
		list->tl_text[list->tl_ntext-1].te_type =
			(entries[index].te_type & ~NS_DONTDELETE);
		ret = true;
	} else {
		NsNode::addText(list, entries[index].te_text.t_chars,
				entries[index].te_text.t_len,
				entries[index].te_type, false /* not donated */);
	}
	lastType = entries[index].te_type;
	return ret;
}

void NsUpdate::doCoalesceText(NsNode *node, Document &doc, OperationContext &oc)
{
	int32_t numLeading = node->getNumLeadingText();
	int32_t numChild = node->getNumChildText();
	DBXML_ASSERT(numLeading >= 2 || numChild >= 2);

	nsTextList_t *newList = NsNode::createTextList(node->getNumText());
	nsTextEntry_t *entries = node->getTextEntry(0);
	
	int i, lastType = -1;
	for (i = 0; i < numLeading; i++)
		(void) coalesceEntry(newList, entries, lastType, i);
	if (numChild) {
		lastType = -1;
		for (i = node->getFirstTextChildIndex(); i < node->getNumText(); i++)
			if (coalesceEntry(newList, entries, lastType, i))
				--numChild; // one fewer child text nodes
	}
	newList->tl_nchild = numChild;
	// replace the list, put the node, put things back for releasing
	nsTextList_t *oldList = node->replaceTextList(newList);
	putNode(node, *doc.getDocDb(), doc.getID(), oc);
	node->replaceTextList(oldList, true /*free oldlist*/);
}

void NsUpdate::coalesceText(OperationContext &oc)
{
	for (NidMap::iterator i = textCoalesce_.begin();
	     i != textCoalesce_.end(); i++) {
		Document *doc = i->second.getDocument();
		try {
			NsNodeRef node =
				fetchNode(NsNid(i->second.getNid()),
					  doc->getID(),
					  *doc->getDocDb(),
					  oc);
			if (node->canCoalesceText())
				doCoalesceText(*node, *doc, oc);
		} catch (XmlException &xe) {
			// ignore DB_NOTFOUND in this path --
			// nodes may have been deleted
			if ((xe.getExceptionCode() == XmlException::DATABASE_ERROR) &&
			    (xe.getDbErrno() == DB_NOTFOUND))
				continue;
			throw;
		}
	}
}

void NsUpdate::completeUpdate(XmlManager &mgr,
			      OperationContext &oc,
			      DynamicContext *context)
{
	coalesceText(oc); // coalesce text nodes
	
	reindex(oc); // reindex elements
	
	for (RewriteIter i = documentUpdates_.begin();
	     i != documentUpdates_.end(); i++) {
		// Update document content without indexing
		Document *doc = i->second;
		ScopedContainer sc(mgr, doc->getContainerID(), true);
		Container *cont = sc.getContainer();
		DBXML_ASSERT(cont);
		// this path can occur if nodes were created via Sequential Scan
		// iterators that did not materialize a Document object
		if (doc->getDefinitiveContent() != Document::DOM) {
			DBXML_ASSERT(doc->getDocDb());
			doc->setContentAsNsDom(doc->getID(),doc->getCacheDatabase());
			doc->setContentModified(true);
		}
					       
		// for wholedoc docs with node indexes, need to totally
		// reindex because node IDs change with partial updates
                // for document level indexing, need to totally reindex
                // because partial reindexing deletes index entries without
                // checking to see if the index entry applies to any other
                // parts of the document that are not deleted
		if (cont->isWholedocContainer()) {
			XmlUpdateContext uc = mgr.createUpdateContext();
			try {
				cont->updateDocumentInternal(oc.txn(), *doc, uc,
							     false /* do not validate */);
			} catch (XmlException &xe) {
				// ignore parser errors.  This is safe because (1) we aren't
				// validating and (2) XQuery Update cannot create non-well-formed
				// documents.  The only possible exception is a no-content document, which
				// should be ignored.
				if (xe.getExceptionCode() != XmlException::INDEXER_PARSER_ERROR)
					throw;
				// else ignore
			}
                } else if (cont->isNodeContainer() && !cont->nodesIndexed()) {
                        bool updateStats = 
                                (cont->getContainerConfig().getStatistics() != 
                                XmlContainerConfig::Off);
                        cont->getDocumentDB()->reindex(*doc, oc, updateStats, false);
                } else {
			DocumentDatabase *db = cont->getDocumentDB();
			DbXmlDbt *dbt = (DbXmlDbt*)doc->getContentAsDbt();
			doc->getID().setDbtFromThis(oc.key());
			db->addContent(oc.txn(), oc.key(), dbt, 0);
		}
	}
	// add auto-generated indexes, if any
	addAutoIndexes();
}

void NsUpdate::markForUpdate(Document *doc, OperationContext &oc)
{
	// only mark if (1) wholedoc and (2) there is a container
	// isWholedoc() handles both.  Transient documents need to
	// be marked as modified and dbt content cleared
	if (doc->getContainerID() == 0 || doc->isWholedoc()) {
		doc->setContentModified(true);
		doc->resetContentAsDbt();
		
	}
        if (doc->isWholedoc() || doc->isDocumentIndex()) {
	        // mark for content to be updated
	        // use docid and container name as key
	        string key = doc->getID().asString();
	        key += doc->getContainerName();
                if (documentUpdates_.find(key) == documentUpdates_.end()) {
	                documentUpdates_.insert(make_pair(key,doc));
                        // Delete the indexes on documents in node containers with
                        // document level indexing.  The document will be reindexed 
                        // when the updates are complete.
                        if (doc->getContainerID() != 0 && !doc->isWholedoc()) {
                                ScopedContainer sc(doc->getManager(), doc->getContainerID(), true);
                                Container *cont = sc.getContainer();
                                bool updateStats = 
                                        (cont->getContainerConfig().getStatistics() != 
                                        XmlContainerConfig::Off);
                                cont->getDocumentDB()->reindex(*doc, oc, updateStats, true);
                        }
                }
        }
}

//
// Reindex-related functions
//
IndexInfo::IndexInfo(Document &document, int cid, OperationContext &oc)
	: autoIs_(0), oc_(oc)
{
	ScopedContainer sc(document.getManager(), cid, true);
	cont_ = sc.getContainer();
	cont_->acquire();
	// read the current IS from the container, but unconditionally set auto
	// indexing OFF for this IS.  It'll never be written back and it's
	// necessary to suppress auto-indexing at this point in the updates
	is_ = new IndexSpecification();
	is_->read(cont_->getConfigurationDB(), oc.txn(), /* lock */ false);
	if (is_->getAutoIndexing()) {
		autoIs_ = new IndexSpecification();
		autoIs_->read(cont_->getConfigurationDB(), oc.txn(), /* lock */ false);
	}
	is_->setAutoIndexing(false);
}

IndexInfo::~IndexInfo()
{
	cont_->release();
	delete is_;
	if (autoIs_)
		delete autoIs_;
}

IndexInfo *NsUpdate::getIndexInfo(Document &document,
				  OperationContext &oc)
{
	int cid = document.getContainerID();
	if (cid == 0)
		return 0;
	IndexInfoMap::iterator i = indexMap_.find(cid);
	if (i != indexMap_.end())
		return i->second;
	IndexInfo *ii = new IndexInfo(document, cid, oc);
	indexMap_[cid] = ii;
	return ii;
}

void NsUpdate::addAutoIndexes()
{
	IndexInfoMap::iterator i = indexMap_.begin();
	while (i != indexMap_.end()) {
		IndexInfo *ii = i->second;
		if (ii->getAutoIndexSpecification()) {
			ii->getContainer()->doAutoIndex(
				ii->getAutoIndexSpecification(),
				*(ii->getIndexSpecification()),
				ii->getOperationContext());
		}
		*i++;
	}
}

void NsUpdate::removeAttributeIndexes(NsReindexer &reindexer,
				      NsNodeRef &parentNode,
				      Document &document,
				      OperationContext &oc)
{
	// remove all attribute indexes for the referenced node (indexes
	// on the node itself remain intact)
	// Node is always reindexed for attributes later
	DBXML_ASSERT(reindexer.willReindex());
	string key = makeKey(parentNode->getNid(), document.getID(),
			     document.getContainerName());
	// avoid doing this twice
	bool fullGen;
	if (!indexesRemoved(key, true, fullGen)) {
		reindexer.indexAttributes(parentNode);
		reindexer.updateIndexes();
		markElement(elements_, key, parentNode->getNid(),
			    false, document, /* attributes */true, false);
	}
}

void NsUpdate::removeElementIndexes(const DbXmlNodeImpl &node,
				    Document &document, bool updateStats,
				    bool targetFullGen, NsNidWrap &vindexNid,
				    OperationContext &oc)
{
	// NOTE on document nodes:  they are handled, but are not
	// "reindexed" here specifically.  They are marked
	// so that NsReindexer code causes them to be indexed.  This
	// deals with the special case of inserting a new root element.
	// You cannot delete a document node, which means that
	// if the node is a document node, the caller is insertElements().
	
	IndexInfo *ii = getIndexInfo(document, oc);
	// remove indexes related to element.  If reindex is true, mark
	// it for reindexing (for update) later
	NsReindexer reindexer(document, ii,
			      document.getManager().getImplicitTimezone(),
			      /* forDelete */true, updateStats);
	if (reindexer.willReindex()) {
		string key = makeKey(node.getNodeID(), document.getID(),
				     document.getContainerName());
		// avoid doing this twice
		bool fullGen = false;
		if (!indexesRemoved(key, false, fullGen)) {
			// if this is the document node, just mark it, below
			if (node.getNodeType() != nsNodeDocument) {
				NsNodeRef nodeRef = node.getNsDomNode()->getNsNode();
				reindexer.indexElement(nodeRef, targetFullGen, vindexNid);
				reindexer.updateIndexes();
			} else {
				// document needs reindexing unconditionally
				markElement(elements_, key, node.getNodeID(), true,
					    document, /* attributes */false, true);
			}
		} else if (fullGen)
			vindexNid.copy(node.getNodeID());
	}
}

void NsUpdate::markElement(NidMap &nidmap, const NsNid &nid, bool updateStats,
			   Document &doc, bool attributes, bool fullGen)
{
	markElement(nidmap, makeKey(nid, doc.getID(),
                                    doc.getContainerName()),
		    nid, updateStats, doc,
		    attributes, fullGen);
}

void NsUpdate::markElement(NidMap &nidmap, const std::string &key,
			   const NsNid &nid, bool updateStats,
			   Document &doc, bool attributes, bool fullGen)
{
	NidMap::iterator i = nidmap.find(key);
	if (i != nidmap.end()) {
		i->second.setAttributes(attributes);
	} else {
		nidmap.insert(
			make_pair(key,
				  NidMarker(&doc, nid, updateStats, attributes, fullGen)));
	}
}

// is the element specified by key already on the list?
bool NsUpdate::indexesRemoved(const std::string &key,
			      bool attributes,
			      bool &fullGen) const
{
	NidMap::const_iterator i = elements_.find(key);
	if (i == elements_.end())
		return false;
	// if the previous removal was only attributes
	// and this one is not, it must proceed
	if (i->second.attributesOnly() && !attributes)
		return false;
	fullGen = i->second.fullGeneration();
	return true;
}

void NsUpdate::reindex(OperationContext &oc)
{
	for (NidMap::iterator i = elements_.begin();
	     i != elements_.end(); i++) {
		Document *doc = i->second.getDocument();
		IndexInfo *ii = getIndexInfo(*doc, oc);
		NsReindexer reindexer(
			*doc, ii, doc->getManager().getImplicitTimezone(),
			/* forDelete */false,
			i->second.getUpdateStats());
		if (reindexer.willReindex()) {
			try {
				// TBD: can probably avoid this fetch
				// in some instances (e.g. attributesOnly
				// and attrs aren't indexed)
				NsNodeRef node =
					fetchNode(NsNid(i->second.getNid()),
						  doc->getID(),
						  *doc->getDocDb(),
						  oc);
				
				if (i->second.attributesOnly()) {
					reindexer.indexAttributes(node);
				} else {
					NsNidWrap ignored;
					reindexer.indexElement(
						node,
						i->second.fullGeneration(),
						ignored);
				}
				reindexer.updateIndexes();
			} catch (XmlException &xe) {
				// ignore DB_NOTFOUND in this path --
				// nodes may have been deleted
				if ((xe.getExceptionCode() == XmlException::DATABASE_ERROR) &&
				    (xe.getDbErrno() == DB_NOTFOUND))
					continue;
				throw;
			}
		}
	}
}
	
//
// utility functions
//

//static
NsNode *NsUpdate::fetchNode(const DbXmlNodeImpl &node,
			    DbWrapper &db,
			    OperationContext &oc)
{
	return fetchNode(node.getNodeID(), node.getDocID(), db, oc);
}

//static
NsNode *NsUpdate::fetchNode(const NsNid &nid,
			    const DocID &did,
			    DbWrapper &db,
			    OperationContext &oc)
{
	return NsFormat::fetchNode(nid, did, db, oc, true);
}

// static
void NsUpdate::putNode(const NsNode *node,
		       DbWrapper &db,
		       const DocID &did,
		       OperationContext &oc)
{
	int err = NsFormat::putNodeRecord(db, oc, did, node,
					  false /* not adding */);

	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG))
		NsFormat::logNodeOperation(db, did, node,
					   const_cast<NsNode*>(node)->getNid(),
					   NULL, // don't pass data
					   "updating", err);
	if (err)
		throw XmlException(err);
}

// static
void NsUpdate::deleteTree(NsNode *node,
			  DbWrapper &db,
			  const DocID &did,
			  OperationContext &oc)
{
	// delete the range of records from current to
	// last descendant
	Cursor cursor(db, oc.txn(), CURSOR_WRITE, 0);
	if (cursor.error() != 0)
		throw XmlException(cursor.error());
	DbtOut data;
	data.set_flags(DB_DBT_PARTIAL); // only need keys

	// Find last node (maybe the first one)
	DbtOut tmp;
	NsNid lastNid(node->getLastDescendantNidOrSelf());
	NsFormat::marshalNodeKey(did, lastNid, tmp);
	void *lastData = tmp.data;
	u_int32_t keySize = tmp.size;

	// First nid (this one)
	NsFormat::marshalNodeKey(did, node->getNid(), oc.key());
	int err = cursor.get(oc.key(), data, DB_SET_RANGE);

	while (err == 0) {
		try {
			err = cursor.del(0);
			if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
				unsigned char *ptr = (unsigned char *)oc.key().data;
				DocID tdid;
				ptr += tdid.unmarshal(ptr);
				NsNid nid(ptr);
				NsFormat::logNodeOperation(db, did, 0, nid,
							   0, "deleted", err);
			}
			if (err == 0 &&
			    oc.key().size == keySize &&
			    memcmp(oc.key().data, lastData, keySize) == 0)
				break;
			if (err == 0)
				err = cursor.get(oc.key(), data, DB_NEXT);
		} catch (...) {
			cursor.close();
			throw;
		}
	}
	cursor.close();
}


// NOTE: the first node in the call *must* require modification.
// After that, it depends on if the last modified element has a 'next'
// static
void NsUpdate::updateLastDescendants(NsDomNode *node,
				     const NsFullNid *nid,
				     DbWrapper &db,
				     const DocID &did,
				     OperationContext &oc)
{
	// stop when either (1) at document node or (2) current
	// node has a next element
	NsDomNodeRef nodeRef(node);
	while (nodeRef) {
		NsNode *nsNode = (NsNode*)nodeRef->getNsNode();
		if (nsNode->isDoc())
			break;
		nsNode->setLastDescendantNid(nid);
		putNode(nsNode, db, did, oc);
		if (nsNode->hasNext())
			break;
		nodeRef = nodeRef->getNsParentNode();
	}
}

//
// Note on insert/delete of attributes and text
//
// Insertion and deletion of attribute and text entries affects both
// the "real" index databases and the index values associated with a given
// DbXmlNodeImpl object.  Those index values indicate the index of the
// attribute or text within the owning node.
//
// As attribute or text are added or deleted, the indexes of any
// affected objects need to be adjusted.  In order to do that,
// a map of node->index added/removed is maintained and used.
//

// TBD: GMF -- more efficient way to make a key
// It needs to encapsulate:
//  node ID, doc ID and container name
//
static string makeKey(const NsNid &nid,
		      const DocID &did,
		      const std::string &cname)
{
	string ret = (const char *)nid.getBytes();
	ret += did.asString();
	ret += cname;
	return ret;
}

static string makeKey(const DbXmlNodeImpl &node)
{
	return makeKey(node.getNodeID(),
		       node.getDocID(),
		       node.getContainer()->getName());
}

// Lookup ID from name in dictionary, but be prepared
// to define a new mapping in the event the new
// content has a new URI and/or prefix
static void lookupNameIDs(NsQName &qname,
			  const ContainerBase *container,
			  OperationContext &oc,
			  int &uri,
			  int &prefix)
{
	NameID uriID;
	DictionaryDatabase *ddb = container->getDictionaryDatabase();
	DBXML_ASSERT(ddb);
	(void) ddb->lookupIDFromStringName(oc, qname.uri_.str(),
					   qname.uri_.len(), uriID,
					   /* define */ true);
	uri = (int)uriID.raw();
	if (qname.prefix_.len()) {
		NameID prefixID;
		(void) ddb->lookupIDFromStringName(oc, qname.prefix_.str(),
						   qname.prefix_.len(), prefixID,
						   /* define */ true);
		prefix = (int) prefixID.raw();
	}
}

//
// AutoIndexWriter implementation
//

void AutoIndexWriter::writeEndElementWithNode(const unsigned char *localName,
					      const unsigned char *prefix,
					      const unsigned char *uri,
					      IndexNodeInfo *ninfo)
{
	if (!is_)
		return;
	const NsNode *node = ninfo->getNode();
	DBXML_ASSERT(node);
	if (!node->hasChildElem()) {
		// a leaf, add it
		Name nm((const char *)uri, (const char *)localName);
		is_->enableAutoElementIndexes(nm.getURIName().c_str());
	}
}

void AutoIndexWriter::writeStartElementWithAttrs(const unsigned char *localName,
						 const unsigned char *prefix,
						 const unsigned char *uri,
						 int numAttributes,
						 NsEventAttrList *attrs,
						 IndexNodeInfo *ninfo,
						 bool isEmpty)
{
	if (!is_)
		return;
	for (int i = 0; i < numAttributes; i++) {
		Name nm((const char *)attrs->uri(i),
			(const char *)attrs->localName(i));
		is_->enableAutoAttrIndexes(nm.getURIName().c_str());
	}
	if (isEmpty)
		writeEndElementWithNode(localName, prefix, uri, ninfo);
}

