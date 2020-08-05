//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "NsUpgrade.hpp"
#include "NsDocument.hpp"
#include "NsUtil.hpp"
#include "NsConstants.hpp"
#include "NsFormat.hpp"
#include "NsDocumentDatabase.hpp"

#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

static const xmlch_t *_xmlUri16 = XMLUni::fgXMLURIName;
static const char *_dbxmlUri8 = "http://www.sleepycat.com/2002/dbxml";
static const xmlch_t _dbxmlUri16[] =
{
	chLatin_h, chLatin_t, chLatin_t, chLatin_p, chColon, chForwardSlash,
	chForwardSlash, chLatin_w, chLatin_w, chLatin_w, chPeriod,
	chLatin_s, chLatin_l, chLatin_e, chLatin_e, chLatin_p, chLatin_y,
	chLatin_c, chLatin_a, chLatin_t, chPeriod, chLatin_c, chLatin_o,
	chLatin_m, chForwardSlash, chDigit_2, chDigit_0, chDigit_0, chDigit_2,
	chForwardSlash,	chLatin_d, chLatin_b, chLatin_x, chLatin_m,
	chLatin_l, chNull
};


/*
 * NsUpgradeReader
 *
 * See comments in NsUpgrade.hpp.  This is upgrade only.
 */

#define MAKEBOOL(x) ((x)? true : false)

// use 256K buffer for DB_MULTIPLE gets
#define NS_EVENT_DB_BUFSIZE (256 * 1024)

using namespace std;

#define NS_NAMESPACE_ID 3 // hardcoded -- namespace info

namespace DbXml {
	
//
// NsNamespaceInfo
//
// As of 2.3, only used for upgrading pre-2.3 containers, so
// this class is moved to NsUpgrade.cpp
//
// Encapsulates per-document namespace information, populated
// when a document is in use (parse time, serialized, and DOM):
//  o list of namespace URIs, including URIs for schema types
//  o list of prefix mappings, and schema name mappings
//
// TBD: put the URIs in the name
// dictionary, for sharing across documents.
///
#define NS_DEFAULT_NURI 20
#define NS_DEFAULT_NPREFIX 20
#define NS_NSMAP_KNOWN 3
	
class NsPrefixMapEntry {
public:
	NsPrefixMapEntry() : prefix8(0), prefix16(0),
			     uriIndex(-1), plen(0) {}
	// no destructor, NsNamespaceInfo manages memory
	const xmlbyte_t *prefix8;
	const xmlch_t *prefix16;
	int32_t uriIndex;
	size_t plen; // number of utf-8 bytes, incl. null.
};

class NsUriMapEntry {
public:
	NsUriMapEntry() : uri8(0), uri16(0), ulen(0) {}
	// no destructor, NsNamespaceInfo manages memory
	const xmlbyte_t *uri8;
	const xmlch_t *uri16;
	size_t ulen; // number of utf-8 bytes, incl. null.
};

typedef std::vector<NsPrefixMapEntry> NsPrefixMap;
typedef std::vector<NsUriMapEntry> NsUriMap;
typedef std::vector<int32_t> NsUriParserMap;

//
// pre-2.3 NsNamespaceInfo class, holding per-document
// URIs and prefix mappings.  It's been stripped down
// to remove code that inserts new objects.  It just needs
// to be able to load from an existing node storage doc.
//
class NsNamespaceInfo {
public:
	NsNamespaceInfo();
	~NsNamespaceInfo();

	void initialize(void);
	//
	// load namespace info from DB
	//
	void load(const char *buffer);

	// utf-8 and utf-16 accessors
	const xmlbyte_t * getUri8(int32_t uriIndex);
	const xmlbyte_t * getPrefix8(int32_t prefIndex);
	const xmlch_t * getUri(int32_t uriIndex);
	const xmlch_t * getPrefix(int32_t prefIndex);

	int32_t getNUri() const { return _nURI; }
	int32_t getNPrefix() const { return _nPrefix; }
private:
	// lists
	NsPrefixMap _prefixMap;
	NsUriMap _uriMap;
	NsUriParserMap _uriParserMap;
	int32_t _nURI;
	int32_t _nPrefix;
	int32_t _nPUri;
};

class NsUpgradeReaderBuf
{
public:
	NsUpgradeReaderBuf();
	// data
	NsUpgradeReaderBuf *reuseNext;
	NsUpgradeReaderBuf *freeNext;
	uint32_t nNodes;
	uint32_t nComplete;
	size_t nSize;
	bool usedUp;
	void *p;
	DbXmlDbt dbt;
};

class NsUpgradeReaderNodeList
{
public:
	NsUpgradeReaderNodeList(NsUpgradeReaderNodeList *parent,
			      NsUpgradeReaderBuf *currentBuf)
		: node(NULL), buffer(currentBuf),
		childrenDone(0), textDone(0), parent(parent) {}
	OldNsNode *node;
	NsUpgradeReaderBuf *buffer;
	int childrenDone;
	int textDone;
	NsUpgradeReaderNodeList *parent;
};
	
}
using namespace DbXml;

//
// NsNamespaceInfo implementation
//
NsNamespaceInfo::NsNamespaceInfo() :
	_prefixMap(0), _uriMap(0), _uriParserMap(0),
	_nURI(0), _nPrefix(0), _nPUri(0)
{
}

void
NsNamespaceInfo::initialize(void)
{
	_nURI = _nPrefix = _nPUri = NS_NSMAP_KNOWN;
	_prefixMap.resize(NS_DEFAULT_NPREFIX);
	_uriMap.resize(NS_DEFAULT_NURI);
	_uriParserMap.resize(NS_DEFAULT_NURI, -1);

	// well-known uri(s) and prefix(es)

	// xml:
	_uriMap[0].uri16 = (const xmlch_t *) _xmlUri16;
	_uriMap[0].uri8 = (const xmlbyte_t *)_xmlUri8;
	_prefixMap[0].prefix8 = (const xmlbyte_t *)_xmlPrefix8;
	_prefixMap[0].prefix16 = (const xmlch_t *) _xmlPrefix16;
	_prefixMap[0].uriIndex = 0;

	// xmlns:
	_uriMap[1].uri16 = (const xmlch_t *) _xmlnsUri16;
	_uriMap[1].uri8 = (const xmlbyte_t *)_xmlnsUri8;
	_prefixMap[1].prefix8 = (const xmlbyte_t *)_xmlnsPrefix8;
	_prefixMap[1].prefix16 = (const xmlch_t *) _xmlnsPrefix16;
	_prefixMap[1].uriIndex = 1;

	// dbxml:
	_uriMap[2].uri16 = (const xmlch_t *) _dbxmlUri16;
	_uriMap[2].uri8 = (const xmlbyte_t *)_dbxmlUri8;
	_prefixMap[2].prefix8 = (const xmlbyte_t *)_dbxmlPrefix8;
	_prefixMap[2].prefix16 = (const xmlch_t *) _dbxmlPrefix16;
	_prefixMap[2].uriIndex = 2;
}

NsNamespaceInfo::~NsNamespaceInfo()
{
	int i;
	// start past static string(s)
	for (i = NS_NSMAP_KNOWN; i < _nURI; i++) {
		if (_uriMap[i].uri8)
			NsUtil::deallocate((void*)_uriMap[i].uri8);
		if (_uriMap[i].uri16)
			NsUtil::deallocate((void*)_uriMap[i].uri16);
		_uriMap[i].uri8 = 0;
		_uriMap[i].uri16 = 0;
	}
	for (i = NS_NSMAP_KNOWN; i < _nPrefix; i++) {
		if (_prefixMap[i].prefix8)
			NsUtil::deallocate((void*)_prefixMap[i].prefix8);
		if (_prefixMap[i].prefix16)
			NsUtil::deallocate((void*)_prefixMap[i].prefix16);
		_prefixMap[i].prefix8 = 0;
		_prefixMap[i].prefix16 = 0;
	}
	_nURI = NS_NSMAP_KNOWN;
	_nPrefix = NS_NSMAP_KNOWN;
	_nPUri = 0;
}

//
// load marshal/unmarshal namespace info from
// a DB record (dump is no longer required).  The format is:
//   nUri -- integer number of uris
//   uri10uri20...
//   nPrefixes -- integer number of prefixes/mappings
//   [uriIndex1,prefix10]... (uriIndex is an integer)
//
//  The order is important -- the order implies index
//  numbers once loaded.  Actual uri and prefix strings
//  start at NS_NSMAP_KNOWN, since indexes less than that
//  are statically constructed (see class ctor).
//
//  The namespace info node ID comes from the document object.
//

void
NsNamespaceInfo::load(const char *buf)
{
	int i;
	// caller got past version and id
	const xmlbyte_t *ptr = (const xmlbyte_t *) buf;
	ptr += NsFormat::unmarshalInt(ptr, &_nURI);
	_nURI += NS_NSMAP_KNOWN;
	if ((uint32_t)_nURI > _uriMap.size())
		_uriMap.resize(_nURI);
	for (i = NS_NSMAP_KNOWN; i < _nURI; i++) {
		size_t len = 0;
		_uriMap[i].uri8 = NsUtil::nsStringDup(ptr, &len);
		_uriMap[i].ulen = len;
		ptr += len;
	}
	ptr += NsFormat::unmarshalInt(ptr, &_nPrefix);
	_nPrefix += NS_NSMAP_KNOWN;
	if ((uint32_t)_nPrefix > _prefixMap.size())
		_prefixMap.resize(_nPrefix);

	for (i = NS_NSMAP_KNOWN; i < _nPrefix; i++) {
		size_t len = 0;
		ptr += NsFormat::unmarshalInt(ptr, &(_prefixMap[i].uriIndex));
		_prefixMap[i].prefix8 = NsUtil::nsStringDup(ptr, &len);
		_prefixMap[i].plen = len;
		ptr += len;
	}
}

const xmlch_t *
NsNamespaceInfo::getUri(int32_t uriIndex)
{
	DBXML_ASSERT(uriIndex != NS_NOPREFIX);
	const xmlch_t *uri = _uriMap[uriIndex].uri16;
	if (!uri) {
		xmlch_t *newuri = 0;
		const xmlbyte_t *uri8 = _uriMap[uriIndex].uri8;
		if (uri8)
			NsUtil::nsFromUTF8(&newuri, uri8,
					   NsUtil::nsStringLen(uri8) + 1, 0);
		_uriMap[uriIndex].uri16 = uri = newuri;
	}
	return  uri;
}

const xmlbyte_t *
NsNamespaceInfo::getUri8(int32_t uriIndex)
{
	DBXML_ASSERT(uriIndex != NS_NOPREFIX);
	const xmlbyte_t *uri8 = _uriMap[uriIndex].uri8;
	if (!uri8) {
		xmlbyte_t *newuri = 0;
		const xmlch_t *uri16 = _uriMap[uriIndex].uri16;
		if (uri16)
			_uriMap[uriIndex].ulen =
				NsUtil::nsToUTF8(&newuri, uri16,
						 NsUtil::nsStringLen(uri16) +1,
						 0);
		_uriMap[uriIndex].uri8 = uri8 = newuri;
	}
	return  uri8;
}

const xmlch_t *
NsNamespaceInfo::getPrefix(int32_t prefIndex)
{
	DBXML_ASSERT(prefIndex != NS_NOPREFIX);
	const xmlch_t *pref = _prefixMap[prefIndex].prefix16;
	if (!pref) {
		xmlch_t *newpref = 0;
		const xmlbyte_t *pref8 = _prefixMap[prefIndex].prefix8;
		if (pref8)
			NsUtil::nsFromUTF8(&newpref, pref8,
					   NsUtil::nsStringLen(pref8) + 1, 0);
		_prefixMap[prefIndex].prefix16 = pref = newpref;
	}
	return  pref;
}

const xmlbyte_t *
NsNamespaceInfo::getPrefix8(int32_t prefIndex)
{
	DBXML_ASSERT(prefIndex != NS_NOPREFIX);
	const xmlbyte_t *pref8 = _prefixMap[prefIndex].prefix8;
	if (!pref8) {
		xmlbyte_t *newpref = 0;
		const xmlch_t *pref16 = _prefixMap[prefIndex].prefix16;
		if (pref16)
			_prefixMap[prefIndex].plen =
				NsUtil::nsToUTF8(&newpref, pref16,
						 NsUtil::nsStringLen(pref16)+1,
						 0);
		_prefixMap[prefIndex].prefix8 = pref8 = newpref;
	}
	return  pref8;
}

//
// Upgrade implementation
//
// NsUpgradeReader
//
NsUpgradeReader::NsUpgradeReader(
	DbWrapper &db, const DocID &docId)
	: emptyElement_(false),
	  db_(db),
	  encStr_(0),
	  standStr_(0),
	  sniffStr_(0),
	  xmlDecl_(-1),
	  nsInfo_(0),
	  id_(docId),
	  doInit_(true),
	  popElement_(false),
	  entityCount_(0),
	  current_(NULL),
	  currentBuffer_(NULL),
	  cursor_(db, 0, CURSOR_READ, 0),
	  cursorFlags_(0),
	  reuseList_(NULL),
	  freeList_(NULL)
{
	docId.setDbtFromThis(docKey_);

	initDocInfo();

	// Old root nid format -- node ID 4
	startBuf_[0] = 0x04;
	startBuf_[1] = 0;	
	doElement(true /*start*/);
}

NsUpgradeReader::~NsUpgradeReader()
{
	while(current_ != 0) {
		NsUpgradeReaderNodeList *tmp = current_;
		current_ = tmp->parent;

		if(tmp->buffer)
			releaseNode(tmp->buffer);
		if(tmp->node)
			NsUtil::deallocate(tmp->node);
		delete tmp;
	}

	while (freeList_) {
		NsUpgradeReaderBuf *cur = freeList_;
		freeList_ = cur->freeNext;
		::free(cur);
	}
	if (nsInfo_)
		delete nsInfo_;
	if (encStr_)
		NsUtil::deallocate((void*)encStr_);
	if (sniffStr_)
		NsUtil::deallocate((void*)sniffStr_);
}

void NsUpgradeReader::close()
{
	delete this;
}

const xmlbyte_t *_standYes=(const xmlbyte_t*) "yes";
const xmlbyte_t *_standNo=(const xmlbyte_t*) "no";

// these flags are reproduced from NsDocument.cpp as of
// 2.2.  They should not change here, even if they
// change in NsDocument.
enum NSDOC_FLAGS {
	NSDOC_STANDYES =  0x0001,
	NSDOC_STANDNO =   0x0002,
	NSDOC_HASENCODE = 0x0004,
	NSDOC_NAMESPACE = 0x0008, /**< do namespaces */
	NSDOC_HASNSNODE = 0x0010, /**< has namespace node */
	NSDOC_HASDECL   = 0x0020, /**< has xml decl */
	NSDOC_HASSNIFF  = 0x0040  /**< has sniffed encoding */
};

void
NsUpgradeReader::initDocInfo()
{
	// read doc node
	// read namespace info
	OperationContext oc;
	oc.set(NULL);
	const char metadataId[] =
		{ NS_PROTOCOL_VERSION_COMPAT, NS_METADATA_ID, 0 };
	DbtOut data((void *)metadataId, strlen(metadataId) + 1);
	// reproduce old NsDocumentDatabase::getNodeRecord(), but
	// cannot deadlock -- no transaction
	DBXML_ASSERT(!oc.txn());
	id_.setDbtFromThis(oc.key());
	int ret = db_.get(oc.txn(), &oc.key(), &data, DB_GET_BOTH);
	uint32_t flags = 0;
	if (ret == 0) {
		const xmlbyte_t *ptr = (const xmlbyte_t *) data.data;
		ptr += 3;  // past version and id
		size_t len;
		ptr += NsFormat::unmarshalInt(ptr, &flags);
		if (flags & NSDOC_HASDECL) {
			ptr += NsFormat::unmarshalInt(ptr, &xmlDecl_);
			DBXML_ASSERT(xmlDecl_ == 0 || xmlDecl_ == 1);
		}
		if (flags & NSDOC_HASENCODE) {
			DBXML_ASSERT(!encStr_);
			encStr_ = NsUtil::nsStringDup(ptr, &len);
			ptr += len;
		}
		if (flags & NSDOC_HASSNIFF) {
			sniffStr_ = NsUtil::nsStringDup(ptr, &len);
			ptr += len;
		}
		if (flags & NSDOC_STANDYES)
			standStr_ = _standYes;
		if (flags & NSDOC_STANDNO)
			standStr_ = _standNo;
	}
	if (flags & NSDOC_NAMESPACE) {
		nsInfo_ = new NsNamespaceInfo();
		if (!nsInfo_)
			NsUtil::nsThrowException(XmlException::NO_MEMORY_ERROR,
				 "new failed to allocate memory",
				 __FILE__, __LINE__);
		nsInfo_->initialize();
		const char namespaceId[] =
			{ NS_PROTOCOL_VERSION_COMPAT, NS_NAMESPACE_ID, 0 };

		DbtOut ndata((void *)namespaceId,
			     strlen(namespaceId) + 1);

		id_.setDbtFromThis(oc.key());
		ret = db_.get(oc.txn(), &oc.key(), &ndata, DB_GET_BOTH);
		if (ret == 0) {
			nsInfo_->load((const char*)
				      ((const char *)ndata.data)+3);
		}
	}
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
NsUpgradeReader::getNamespaceURI() const
{
	if (!localName_)
		throwIllegalOperation(type_, "getNamespaceURI");
	return uri_;
}

const unsigned char *
NsUpgradeReader::getLocalName() const
{
	if (!localName_)
		throwIllegalOperation(type_, "getLocalName");
	return localName_;
}

const unsigned char *
NsUpgradeReader::getPrefix() const
{
	if (!localName_)
		throwIllegalOperation(type_, "getPrefix");
	return prefix_;
}
	
const unsigned char *
NsUpgradeReader::getValue(size_t &len) const
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
NsUpgradeReader::needsEntityEscape(int index) const
{
	if (type_ == Characters) {
		return MAKEBOOL(textType_ & NS_ENTITY_CHK);
	} else if (type_ == StartElement) {
		ensureAttributes(index, "needsEntityEscape");
		if (node_->ond_attrs->al_attrs[index].a_flags & NS_ATTR_ENT)
			return true;
		return false;
	} else if (type_ == CDATA)
		return false;
	throwIllegalOperation(type_, "needsEntityEscape");
	return true;
}

bool
NsUpgradeReader::isEmptyElement() const {
	ensureType(StartElement, "isEmptyElement");
	return emptyElement_;
}

// EventReader extensions
IndexNodeInfo *
NsUpgradeReader::getIndexNodeInfo() const
{
	DBXML_ASSERT(0);
	return 0;
}

const unsigned char *
NsUpgradeReader::getVersion() const
{
	ensureType(StartDocument, "getVersion");
	if (xmlDecl_ >= 0)
		return _NsDecl8[xmlDecl_];
	return 0;
}

const unsigned char *
NsUpgradeReader::getEncoding() const
{
	ensureType(StartDocument, "getEncoding");
	return encStr_;
}

bool
NsUpgradeReader::isStandalone() const
{
	if (standStr_ && (::memcmp(standStr_, "yes", 3) == 0))
		return true;
	return false;
}

bool
NsUpgradeReader::standaloneSet() const
{
	if (standStr_)
		return true;
	return false;
}

bool
NsUpgradeReader::encodingSet() const
{
	if (encStr_)
		return true;
	return false;
}

//
// attribute access (includes namespace attributes)
// node_ will be 0 if no attributes are present
//

int
NsUpgradeReader::getAttributeCount() const
{
	ensureType(StartElement, "getAttributeCount");
	return nattrs_;
}

bool
NsUpgradeReader::isAttributeSpecified(int index) const
{
	ensureAttributes(index, "isAttributeSpecified");
	if (node_->ond_attrs->al_attrs[index].a_flags & NS_ATTR_NOT_SPECIFIED)
		return false;
	return true;
}

const unsigned char *
NsUpgradeReader::getAttributeLocalName(int index) const
{
	ensureAttributes(index, "getAttributeLocalName");
	nsText_t &text = node_->ond_attrs->al_attrs[index].a_name.n_text;
	return (const unsigned char *)text.t_chars;
}

const unsigned char *
NsUpgradeReader::getAttributeNamespaceURI(int index) const
{
	ensureAttributes(index, "getAttributeNamespaceURI");
	int32_t uri = NS_NOURI;
	nsAttr_t *attr = node_->getAttr(index);
	if (attr->a_flags & NS_ATTR_URI)
		uri = attr->a_uri;
	if (uri == NS_NOURI)
		return 0;
	if ((uri != NS_NOPREFIX) && nsInfo_)
		return nsInfo_->getUri8(uri);
	return 0;
}

const unsigned char *
NsUpgradeReader::getAttributePrefix(int index) const
{
	ensureAttributes(index, "getAttributePrefix");
	int32_t prefix = node_->getAttr(index)->a_name.n_prefix;
	if (prefix == NS_NOPREFIX)
		return 0;
	if ((prefix != NS_NOPREFIX) && nsInfo_)
		return nsInfo_->getPrefix8(prefix);
	return 0;
}

const unsigned char *
NsUpgradeReader::getAttributeValue(int index) const
{
	ensureAttributes(index, "getAttributeValue");
	return (const unsigned char *) 
		node_->getAttr(index)->a_value;
}

XmlEventReader::XmlEventType
NsUpgradeReader::next()
{
	// if this is called and there is no next, bad things
	// happen.
	DBXML_ASSERT(hasNext());
	if(doInit_) {
		// first event is done...
		doInit_ = false;
		if ((current_ == NULL)
		    || ((current_->parent == NULL) &&
			(popElement_ || emptyElement_))
			)
			hasNext_ = false;
		return type_;
	} else if (popElement_) {
		// this logic keeps the current element info
		// available through the EndElement event
		popElement();
		if (current_ == NULL) {
			// there is no next...
			throw XmlException(
				XmlException::EVENT_ERROR,
				"XmlEventReader::next() called when hasNext() is false");
		}
	}

	while (current_ != NULL) {
		if((u_int32_t)current_->childrenDone <
		   current_->node->numChildElem()) {
			if(current_->node->hasText() &&
			   (u_int32_t)current_->textDone <
			   current_->node->childTextIndex(
				   current_->childrenDone)) {
				if (!doText())
					continue; // skip
				return type_;
			}

			++current_->childrenDone;
			(void) doElement(/* start */true);
			if (entityCount_ != 0)
				continue; // eat this event
			return type_;
		}

		if(current_->node->hasText() &&
		   current_->textDone < current_->node->numText()) {
			if (!doText())
				continue; // skip
			return type_;
		}

		// Do the endElement event -- it may return false,
		// in which case, it was an empty element, and we skip the
		// EndElement event
		bool validEvent = doElement(/* start */false);

//		if (validEvent && (entityCount_ == 0)) {
//			popElement_ = true;
//			break;
//		}
		if (validEvent)
			break;
		// else pop to parent and continue
		popElement();
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
NsUpgradeReader::doText()
{
	const nsTextEntry_t *entry =
		current_->node->getTextEntry(current_->textDone);
	++current_->textDone;

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

// returns false either on end of document, or after
// end of empty element, which triggers the caller to skip
// the event
bool
NsUpgradeReader::doElement(bool start)
{
	// initialize state
	value_ = 0;
	node_ = 0;
	// If starting, fetch the next node; otherwise, it's been
	// "popped" back into current_
	if(start) {
		DbXmlDbt data;
		current_ = new NsUpgradeReaderNodeList(current_, currentBuffer_);
		if(doInit_) {
			nextNode(&current_->buffer, data, startBuf_);
			if(current_->buffer == 0) {
				hasNext_ = false; // no-content doc
				return false;
			}
		} else {
			nextNode(&current_->buffer, data);
			DBXML_ASSERT(current_->buffer != 0);
		}
		currentBuffer_ = current_->buffer;

		// Because the node buffer will stick around until we've
		// finished, we can set copyStrings to false, meaning that
		// the OldNsNode strings point directly into the node buffer.
		unsigned char *datap = (unsigned char *)data.data;
		current_->node = OldNsNode::unmarshalOldNode(
			datap, data.size,
			/*adoptBuffer*/false);
	}
	//
	// We have an unmarshaled node in current_
	//
	uint32_t flags = current_->node->getFlags();
	if (flags & OLD_NS_ISDOCUMENT) {
		if(start) {
			// TBD: not using sniffed encoding
			type_ = StartDocument;
		} else {
			type_ = EndDocument;
			popElement_ = true;
		}
	} else {
		emptyElement_ =
			!((flags & OLD_NS_HASCHILD) ||
			  (flags & OLD_NS_HASTEXT));
		// skip EndElement for empty elements
		if (emptyElement_ && !start)
			return false;

		// even for EndElement, reassign names
		node_ = current_->node;
		localName_ = (const unsigned char*)
			node_->ond_header.nh_name.n_text.t_chars;
		if (nsInfo_ && node_->namePrefix() != NS_NOPREFIX)
			prefix_ = (const unsigned char *)
				nsInfo_->getPrefix8(node_->namePrefix());
		else
			prefix_ = 0;
		if (nsInfo_ && node_->hasUri())
			uri_ = (const unsigned char *)
				nsInfo_->getUri8(node_->uriIndex());
		else
			uri_ = 0;
		if(start) {
			type_ = StartElement;
			nattrs_ = node_->numAttrs();
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
//		    || (emptyElement_ && (current_->parent->parent == NULL))
			)
			hasNext_ = false;
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
NsUpgradeReader::nextNode(NsUpgradeReaderBuf **bufp, DbXmlDbt &data,
			  xmlbyte_t *startId)
{
	NsUpgradeReaderBuf *buf = *bufp;
	DBXML_ASSERT(buf || startId);
	size_t bufSize = NS_EVENT_DB_BUFSIZE;
	// are there any items left?
	if(buf) {
		DB_MULTIPLE_NEXT(buf->p, &buf->dbt, data.data, data.size);
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
			buf = (NsUpgradeReaderBuf *)
				NsUtil::allocate(bufSize +
						 sizeof(NsUpgradeReaderBuf));
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
		buf->dbt.ulen = ((u_int32_t)buf->nSize);
		buf->dbt.data = (buf + 1);
		int ret = 0;
		if (startId) {
			xmlbyte_t *dptr = (xmlbyte_t *)
				buf->dbt.data;
			*dptr++ = NS_PROTOCOL_VERSION_COMPAT;
			memcpy(dptr, (const char *)startId,
			       strlen((const char*)startId) + 1);
			ret = cursor_.get(
				docKey_,
				buf->dbt,
				cursorFlags_|DB_GET_BOTH|DB_MULTIPLE);
		} else {
			ret = cursor_.get(
				docKey_,
				buf->dbt,
				cursorFlags_|DB_NEXT_DUP|DB_MULTIPLE);
		}
		if (ret != 0) {
			// if buf was just allocated, free it; otherwise,
			// put it back on the reuse list
			if (freeList_ == buf) {
				freeList_ = buf->freeNext;
				::free(buf);
			} else {
				buf->reuseNext = reuseList_;
				reuseList_ = buf;
			}
			if (ret == DB_BUFFER_SMALL) {
				// necessary size is in buf->dbt.size
				while (bufSize < buf->dbt.size)
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
		DB_MULTIPLE_NEXT(buf->p, &buf->dbt, data.data, data.size);
		if (buf->p == 0) {
			NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
						 "Failed to find node.",
						 __FILE__, __LINE__);
			return;
		}
		*bufp = buf;
	}
	buf->nNodes++;
}

// reset current_ to the parent of the current node
void
NsUpgradeReader::popElement()
{
	popElement_ = false;
	NsUpgradeReaderNodeList *tmp = current_;
	current_ = tmp->parent;
	releaseNode(tmp->buffer);
	NsUtil::deallocate(tmp->node);
	delete tmp;
}
	
void
NsUpgradeReader::releaseNode(NsUpgradeReaderBuf *buf)
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

//
// Unmarshaling code for old format
//
extern "C" {
typedef struct {
	union idStore_u {
		xmlbyte_t *idPtr;
		xmlbyte_t idBytes[4];
	} idStore;
	uint32_t idLen;
} old_nid_t;

typedef struct {
	uint32_t ce_textIndex;
	old_nid_t ce_id;
} old_nsChildEntry_t;

};
#define NS_UNMARSH_MIN sizeof(OldNsNode) + \
	sizeof(nsTextList_t) +		\
	sizeof(nsAttrList_t) +		\
	sizeof(nsChildListCompat_t)

// static
OldNsNode *
OldNsNode::unmarshalOldNode(unsigned char *buf,
			    uint32_t bufsize, bool adoptBuffer)
{
	uint32_t allocSize;
	OldNsNode *node;
	char *curP;
	unsigned char *ptr = buf;
	xmlbyte_t *endP;

	if (*ptr++ != NS_PROTOCOL_VERSION_COMPAT)
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "protocal mismatch in node",
					 __FILE__, __LINE__);
	/*
	 * use unmarshal size, which follows the node id,
	 * to determine allocation size.  This means skipping into
	 * the buffer, past the node id.
	 */
	const xmlbyte_t *tptr = NsFormat::skipId(ptr);

	NsFormat::unmarshalInt(tptr, &allocSize);

	// We don't trust the unmarshal size, as it may have stored
	// old sizeof() information in the database.
	allocSize *= sizeof(nsChildEntryCompat_t);
	allocSize /= sizeof(old_nsChildEntry_t);

	allocSize += NS_UNMARSH_MIN;

	// Now allocate, using the adjusted allocSize
	curP = (char *)NsUtil::allocate(allocSize);
	memset(curP, 0, allocSize);
	node = (OldNsNode *) curP;
	endP = (xmlbyte_t *) (curP + allocSize - 1);
	curP = (char *) (node + 1);

	if(adoptBuffer) node->ond_memory = buf;

	ptr = unmarshalOldHeader(node, &endP, ptr, /*copyStrings*/false);

	/*
	 * Continue with header bits, and easy structures,
	 * carving them off from curP, as necessary.
	 */
	if (node->hasChildElem()) {
		nsChildListCompat_t *child = (nsChildListCompat_t *)curP;
		node->setChildListCompat(child);
		ptr += NsFormat::unmarshalInt(ptr, &child->cl_numChild);
		child->cl_maxChild = child->cl_numChild;
		curP = (char *)(child + 1);
		curP += (child->cl_numChild - 1) * sizeof(nsChildEntryCompat_t);
		if (node->hasText()) {
			for (unsigned int i = 0; i < child->cl_numChild; ++i) {
				ptr += NsFormat::unmarshalInt(ptr, &(child->cl_child[i].ce_textIndex));
				ptr += NsFormat::unmarshalId(ptr,
					&child->cl_child[i].ce_id,
					&endP, /*copyStrings*/false);
			}
		}
		else {
			for (unsigned int i = 0; i < child->cl_numChild; ++i) {
				ptr += NsFormat::unmarshalId(ptr,
					&child->cl_child[i].ce_id,
					&endP, /*copyStrings*/false);
			}
		}
	}

	if (node->hasAttr()) {
		nsAttrList_t *attrs = (nsAttrList_t *)curP;
		node->ond_attrs = attrs;
		/* need to know number before can carve structure */
		ptr += NsFormat::unmarshalInt(ptr, &(attrs->al_nattrs));
		attrs->al_max = attrs->al_nattrs;
		curP = (char *)(attrs + 1);
		/* add nattrs-1 worth of nsText_t */
		curP += ((attrs->al_nattrs - 1) * sizeof(nsAttr_t));
		/* unmarshal the attributes */
		ptr = NsFormat::unmarshAttrs(ptr, attrs, &endP,
					     /*copyStrings*/false);
	}

	if (node->hasText()) {
		nsTextList_t *text = (nsTextList_t *)curP;
		node->ond_text = text;
		ptr += NsFormat::unmarshalInt(ptr, &(text->tl_ntext));
		text->tl_max = text->tl_ntext;
		curP = (char *)(text + 1);
		curP += ((text->tl_ntext - 1) * sizeof(nsTextEntry_t));
		ptr = NsFormat::unmarshTextList(ptr, text, &endP,
						/*copyStrings*/false);
	}

	/* debug */
	if (curP >= (char *)endP) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "unmarshalOldNode overlap",
					 __FILE__, __LINE__);
	}
	return node;
}

//static
unsigned char *
OldNsNode::unmarshalOldHeader(OldNsNode *node, unsigned char **endPP,
			      unsigned char *ptr, bool copyStrings)
{
	uint32_t dummy;
	ptr += NsFormat::unmarshalId(ptr, node->getNid(), endPP, copyStrings);
	/* already used unmarsh size */
	ptr += NsFormat::unmarshalInt(ptr, &dummy);
	ptr += NsFormat::unmarshalInt(ptr, node->getFlagsPtr());
	ptr += NsFormat::unmarshalInt(ptr, node->getLevelPtr());
	if (!(node->checkFlag(OLD_NS_ISDOCUMENT))) {
		nsName_t *nm = node->getName();
		ptr += NsFormat::unmarshalId(ptr, node->getParentNid(),
					     endPP, copyStrings);
		
		if (node->checkFlag(OLD_NS_NAMEPREFIX))
			ptr += NsFormat::unmarshalInt(ptr,
						      (uint32_t*)&(nm->n_prefix));
		else
			nm->n_prefix = NS_NOPREFIX;
		if (node->checkFlag(OLD_NS_HASURI))
			ptr += NsFormat::unmarshalInt(
				ptr, (uint32_t*)node->uriIndexPtr());
		else
			*(node->uriIndexPtr()) = 0;
#ifdef NS_USE_SCHEMATYPES		
		if (node->checkFlag(OLD_NS_NAMETYPE)) {
			ptr += NsFormat::unmarshalInt(ptr,
						      (uint32_t*)&(nm->n_type));
			if (node->checkFlag(OLD_NS_UNIONTYPE))
				ptr += NsFormat::unmarshalInt(
					ptr, (uint32_t*)&(nm->n_utype));
		} else
			nm->n_type = NS_NOTYPE;
#endif
		ptr += NsFormat::unmarshText(ptr, &(nm->n_text),
					     endPP, copyStrings);
	}

	return ptr;
}

