//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//
#include "../DbXmlInternal.hpp"
#include "NsDocument.hpp"
#include "NsDom.hpp"
#include "NsConstants.hpp"
#include "NsDocumentDatabase.hpp"
#include "../Document.hpp"
#include "../NameID.hpp"
#include <vector>
#include <xercesc/util/XMLUniDefs.hpp>
#include <sstream>

using namespace DbXml;
using namespace std;
XERCES_CPP_NAMESPACE_USE

//
// internal classes
//
namespace DbXml {
//
// Constants -- see NsConstants.hpp
//
	
// URIs
const char *_xmlnsUri8 = "http://www.w3.org/2000/xmlns/";
const xmlch_t *_xmlnsUri16 = XMLUni::fgXMLNSURIName;
const char *_xmlUri8 = "http://www.w3.org/XML/1998/namespace";

// prefixes
const char *_xmlnsPrefix8 = "xmlns";
const xmlch_t *_xmlnsPrefix16 = XMLUni::fgXMLNSString;
const char *_xmlPrefix8 = "xml";
const xmlch_t *_xmlPrefix16 = XMLUni::fgXMLString;
const char *_dbxmlPrefix8 = "dbxml";
const xmlch_t _dbxmlPrefix16[] =
{
	chLatin_d, chLatin_b, chLatin_x, chLatin_m, chLatin_l, chNull
};

const xmlbyte_t *_standYes=(const xmlbyte_t*) "yes";
const xmlbyte_t *_standNo=(const xmlbyte_t*) "no";

// constants for XML declaration (1.0 vs 1.1)
const xmlbyte_t _decl8_1_0[] = {'1', '.', '0', 0};
const xmlbyte_t _decl8_1_1[] = {'1', '.', '1', 0};
const xmlch_t _decl_1_0[] = {'1', '.', '0', 0};
const xmlch_t _decl_1_1[] = {'1', '.', '1', 0};
const xmlch_t *_NsDecl[2] = {_decl_1_0, _decl_1_1};
const xmlbyte_t *_NsDecl8[2] = {_decl8_1_0, _decl8_1_1};

class NsDocInfo {
public:
	NsDocInfo()
		: xmlDecl_(-1), sniffedEnc_(0), enc_(0),
		standStr_(0), enc16_(0), modified_(false) {}
	~NsDocInfo();
	void putDocInfo(DbWrapper *docdb,
			OperationContext &oc, const DocID &docId);
	bool getDocInfo(DbWrapper *docdb,
			OperationContext &oc, const DocID &docId,
			u_int32_t flags);
	const xmlbyte_t *getEncodingStr();
	const xmlbyte_t *getSniffedEncodingStr();
	const xmlbyte_t *getStandaloneStr();
	const xmlch_t *getEncodingStr16();
	int32_t getXmlDecl();

	void setXmlDecl(int32_t decl);
	void setEncodingStr(const xmlbyte_t *str);
	void setSniffedEncodingStr(const xmlbyte_t *str);
	void setStandalone(bool standalone);
	bool getIsStandalone() const;
	bool isModified() const { return modified_; }
	void setModified() { modified_ = true; }
private:
	// NSDOC_FLAGS -- flags stored with a marshaled document
	enum NSDOC_FLAGS {
		NSDOC_STANDYES =  0x0001,
		NSDOC_STANDNO =   0x0002,
		NSDOC_HASENCODE = 0x0004,
		NSDOC_NAMESPACE = 0x0008, /**< do namespaces */
		NSDOC_HASNSNODE = 0x0010, /**< unused as of 2.3 */
		NSDOC_HASDECL   = 0x0020, /**< has xml decl */
		NSDOC_HASSNIFF  = 0x0040  /**< has sniffed encoding */
	};
	int32_t xmlDecl_;
	const xmlbyte_t *sniffedEnc_;
	const xmlbyte_t *enc_;
	const xmlbyte_t *standStr_;
	mutable const xmlch_t *enc16_;
	bool modified_;
};

} // namespace DbXml


//
// NsDocument implementation
//

// Basic ctor
NsDocument::NsDocument(const Document *ownerDoc) :
	ownerDoc_(ownerDoc),
	docInfo_(0), addNsInfo_(true)
{
}

// init to create a new node storage doc
void
NsDocument::initDoc(Transaction *txn, DbWrapper *docdb,
		    DictionaryDatabase *ddb, const DocID &docId,
		    int cid, u_int32_t flags)
{
	init(txn, docdb, ddb, docId, cid, flags);
}

//
// init for a parsing/construction/temporary storage
//
void
NsDocument::initNsDom(const DocID &docID,
		      DictionaryDatabase *ddb)
{
	did_= docID;
	dict_ = ddb;
}

NsDocument::~NsDocument()
{
	if (docInfo_)
		delete docInfo_;
}

void
NsDocument::completeNode(NsNode *node, size_t nodeSize)
{
	// if persistent, write node out, and free it.
	int err = 0;
	if (docdb_) {
		// use local function to put record.  All records
		// are new, so no need to look for existing nodes.
		// Make sure bulk put is used
		(void) oc_.getBulkPut(true);
		err = NsFormat::putNodeRecord(*docdb_, oc_, did_, node, true, nodeSize);
		if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG))
			NsFormat::logNodeOperation(*docdb_, did_, node, node->getNid(),
						   0, "add", err);
	}
	node->release();
	if(err != 0) throw XmlException(err);
}

void
NsDocument::reserveNode(const NsNid &nid, size_t nodeSize)
{
#ifdef DBXML_BULK
	return;
#else
	// if persistent, write node out
	int err = 0;
	if (docdb_) {
		/*	Every call to set_size for DbtOut results in a realloc
			if the new size is bigger than the last, so only change
			the size in reserveNode_ if the new size is bigger than
			the size of the buffer */
		if (reserveNode_.size < nodeSize)
			reserveNode_.set(0, nodeSize);
		const DbXmlDbt dbt(reserveNode_.data, (uint32_t)nodeSize);
		// use local function to put record.  All records
		// are new, so no need to look for existing nodes.
		err = NsFormat::putNodeRecord(*docdb_, oc_, did_, nid, &dbt);

		// NOTE: not currently logging node reservations
	}
	if (err != 0) throw XmlException(err);
#endif
}

//
// done parsing.
// if there is persistent information to write, write it
//
void
NsDocument::completeDocument()
{
	if (docdb_) {
		updateSpecialNodes(oc_);
		BulkPut *bp = oc_.getBulkPut(false);
		if (bp)
			bp->flush(oc_.txn());
	}
}

void
NsDocument::updateSpecialNodes(OperationContext &oc)
{
	if(docInfo_)
		docInfo_->putDocInfo(docdb_, oc, did_);
}

/**
 * methods accessing NsDocInfo
 */
const xmlbyte_t *
NsDocument::getEncodingStr() const
{
	ensureDocInfo(true);
	return docInfo_->getEncodingStr();
}

const xmlbyte_t *
NsDocument::getSniffedEncodingStr() const
{
	ensureDocInfo(true);
	return docInfo_->getSniffedEncodingStr();
}

const xmlbyte_t *
NsDocument::getStandaloneStr() const
{
	ensureDocInfo(true);
	return docInfo_->getStandaloneStr();
}

const xmlch_t *NsDocument::getEncodingStr16() const
{
	ensureDocInfo(true);
	return docInfo_->getEncodingStr16();
}

const xmlbyte_t *
NsDocument::getXmlDecl() const
{
	ensureDocInfo(true);
	int32_t decl = docInfo_->getXmlDecl();
	if (decl >= 0)
		return _NsDecl8[decl];
	return 0;
}

const xmlch_t *
NsDocument::getXmlDecl16() const
{
	ensureDocInfo(true);
	int32_t decl = docInfo_->getXmlDecl();
	if (decl >= 0)
		return _NsDecl[decl];
	return 0;
}

void NsDocument::setSniffedEncodingStr(const xmlbyte_t *str) {
	ensureDocInfo(false);
	docInfo_->setSniffedEncodingStr(str);
}

void NsDocument::setEncodingStr(const xmlbyte_t *str) {
	ensureDocInfo(false);
	docInfo_->setEncodingStr(str);
}

void
NsDocument::setStandalone(bool standalone)
{
	ensureDocInfo(false);
	docInfo_->setStandalone(standalone);
}

bool
NsDocument::getIsStandalone() const
{
	ensureDocInfo(true);
	return docInfo_->getIsStandalone();
}

void
NsDocument::setXmlDecl(int32_t decl)
{
	ensureDocInfo(false);
	docInfo_->setXmlDecl(decl);
}

void
NsDocument::createDocInfo(bool fetch) const
{
	DBXML_ASSERT(!docInfo_);
	docInfo_ = new NsDocInfo();
	if (fetch) {
		docInfo_->getDocInfo(docdb_, oc_, did_, flags_);
	}
}

/**
 * NsDocInfo implementation
 */

NsDocInfo::~NsDocInfo()
{
	if (sniffedEnc_)
		NsUtil::deallocate((void*)sniffedEnc_);
	if (enc_)
		NsUtil::deallocate((void*)enc_);
	if (enc16_)
		NsUtil::deallocate((void*)enc16_);
}

const xmlbyte_t *
NsDocInfo::getEncodingStr()
{
	return enc_;
}

const xmlbyte_t *
NsDocInfo::getSniffedEncodingStr()
{
	return sniffedEnc_;
}

const xmlbyte_t *
NsDocInfo::getStandaloneStr()
{
	return standStr_;
}

int32_t
NsDocInfo::getXmlDecl()
{
	return xmlDecl_;
}

void
NsDocInfo::setXmlDecl(int32_t decl)
{
	xmlDecl_ = decl;
	modified_ = true;
}

void
NsDocInfo::setSniffedEncodingStr(const xmlbyte_t *str)
{
	if (sniffedEnc_)
		NsUtil::deallocate((void*)sniffedEnc_);
	sniffedEnc_ = NsUtil::nsStringDup(str, 0);
	modified_ = true;
}

void
NsDocInfo::setEncodingStr(const xmlbyte_t *str)
{
	if (enc_)
		NsUtil::deallocate((void*)enc_);
	enc_ = NsUtil::nsStringDup(str, 0);
	modified_ = true;
}

void
NsDocInfo::setStandalone(bool standalone)
{
	if (standalone)
		standStr_ = _standYes;
	else
		standStr_ = _standNo;
	modified_ = true;
}

bool
NsDocInfo::getIsStandalone() const
{
	if (standStr_ && (::strcmp((const char *)standStr_,
				   (const char *)_standYes) == 0))
                return true;
        return false;
}

const xmlch_t *NsDocInfo::getEncodingStr16()
{
	if (!enc16_ && enc_) {
		size_t len = NsUtil::nsStringLen(enc_) + 1;
		xmlch_t *enc = 0;
		NsUtil::nsFromUTF8(&enc, enc_, len, len);
		enc16_ = enc;
	}
	return enc16_;
}

// marshal and put document metadata
void
NsDocInfo::putDocInfo(DbWrapper *docdb,
		      OperationContext &oc, const DocID &docId)
{
	DBXML_ASSERT(docdb);
	if(isModified()) {
		// buffer size count
		size_t size = NsFullNid::getNidOverhead() + 1; // +1 == flags byte
		uint32_t flags = 0;
		size_t encLen =0;
		size_t sniffLen =0;
		if (xmlDecl_ >= 0) {
			flags |= NSDOC_HASDECL;
			size += 1; // decl is 0 or 1, which is one byte
		}

		if (enc_) {
			flags |= NSDOC_HASENCODE;
			encLen = NsUtil::nsStringLen(enc_) + 1;
			size += encLen;
		}
		if (standStr_) {
			if (standStr_[0] == 'y')
				flags |= NSDOC_STANDYES;
			else {
				DBXML_ASSERT(standStr_[0] == 'n');
				flags |= NSDOC_STANDNO;
			}
		}
		if (sniffedEnc_) {
			flags |= NSDOC_HASSNIFF;
			sniffLen = NsUtil::nsStringLen(sniffedEnc_) + 1;
			size += sniffLen;
		}
		char *buf = (char *) NsUtil::allocate(size);

		xmlbyte_t *ptr = (xmlbyte_t*)buf;
		*ptr++ = NS_PROTOCOL_VERSION;
		ptr += NsFormat::marshalId(ptr, *NsNid::getMetaDataNid());

		ptr += NsFormat::marshalInt(ptr, flags);
		if (xmlDecl_ >= 0)
			ptr += NsFormat::marshalInt(ptr, xmlDecl_);
		if (enc_) {
			memcpy(ptr, enc_, encLen);
			ptr += encLen;
		}
		if (sniffedEnc_) {
			memcpy(ptr, sniffedEnc_, sniffLen);
			ptr += sniffLen;
		}

		DbXmlDbt data(buf, (unsigned int)size);
		int err = NsFormat::putNodeRecord(*docdb, oc, docId,
						  *NsNid::getMetaDataNid(),
						  &data);

		if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG))
			NsFormat::logNodeOperation(*docdb, docId, 0, *NsNid::getMetaDataNid(),
						   &data, "putDocInfo", err);

		NsUtil::deallocate(buf);
		if(err != 0)
			throw XmlException(err);
		modified_ = false;
	}
}

bool
NsDocInfo::getDocInfo(DbWrapper *docdb,
		      OperationContext &oc, const DocID &docId,
		      u_int32_t flags)
{
	if(!docdb) return true;

	DbtOut data;
	int ret = NsFormat::getNodeRecord(*docdb, oc, docId,
					  *NsNid::getMetaDataNid(),
					  &data, flags);
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG))
		NsFormat::logNodeOperation(*docdb, docId, 0,
					   *NsNid::getMetaDataNid(), &data,
					   "getDocInfo", ret);

	if (ret == 0) {
		const xmlbyte_t *ptr = (const xmlbyte_t *) data.data;
		ptr += NsFullNid::getNidOverhead(); // past version and id
		uint32_t flags;
		size_t len;
		ptr += NsFormat::unmarshalInt(ptr, &flags);
		if (flags & NSDOC_HASDECL) {
			ptr += NsFormat::unmarshalInt(ptr, &xmlDecl_);
			DBXML_ASSERT(xmlDecl_ == 0 || xmlDecl_ == 1);
		}
		if (flags & NSDOC_HASENCODE) {
			DBXML_ASSERT(!enc_);
			enc_ = NsUtil::nsStringDup(ptr, &len);
			ptr += len;
		}
		if (flags & NSDOC_HASSNIFF) {
			if (sniffedEnc_)
				NsUtil::deallocate((void*) sniffedEnc_);
			sniffedEnc_ = NsUtil::nsStringDup(ptr, &len);
			ptr += len;
		}
		if (flags & NSDOC_STANDYES)
			standStr_ = _standYes;
		if (flags & NSDOC_STANDNO)
			standStr_ = _standNo;
		modified_ = false;
		return true;
	} else {
		if (ret == DB_LOCK_DEADLOCK)
			throw XmlException(ret);
		// ID doesn't exist.  handle no-content documents
		return false;
	}
}

// Return (a copy of) the document node, which is of type NsDomElement.
NsDomElement *
NsDocument::getDocumentNode()
{
	if (!docNode_) {
		docNode_ = getNode(
			*NsNid::getRootNid(),
			/*getNext*/false);
	}
	if(docNode_)
		return new NsDomElement(*docNode_, this);
	return 0;
}

