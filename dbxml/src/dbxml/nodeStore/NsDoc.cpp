//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "../DbXmlInternal.hpp"
#include "NsDoc.hpp"
#include "NsUtil.hpp"
#include "NsConstants.hpp"

using namespace DbXml;
using namespace std;

//
// NsDoc
//

void NsDoc::init(Transaction *txn, DbWrapper *docdb,
		 DictionaryDatabase *ddb, const DocID &did,
		 int cid, u_int32_t flags)
{
//	if (docdb && !docdb->isTransacted())
//		txn = 0;
	oc_.set(txn);
	docdb_ = docdb;
	dict_ = ddb;
	did_ = did;
	cid_ = cid;
	flags_ = flags;
}

NsDoc::~NsDoc()
{
}

int
NsDoc::addIDForString(const char *strng, size_t len)
{
	DBXML_ASSERT(dict_);
	DBXML_ASSERT(::strlen(strng) == len);
	NameID id;
	int err = dict_->lookupIDFromStringName(oc_, strng, len, id, true);
	if (err != 0) {
		std::string error =  "Unable to add a URI or prefix string to dictionary: ";
		error += (std::string)strng;
		NsUtil::nsThrowException(XmlException::DATABASE_ERROR,
					 error.c_str(),
					 __FILE__, __LINE__);
	}
	return (int)id.raw();
}

const char *
NsDoc::getStringForID(int32_t dictId)
{
	if (dictId != NS_NOPREFIX) {
		DBXML_ASSERT(dict_);
		NameID id(dictId);
		return dict_->lookupName(oc_, id);
	}
	return NULL;
}

const xmlch_t *
NsDoc::getStringForID16(int32_t dictId)
{
	if (dictId != NS_NOPREFIX) {
		DBXML_ASSERT(dict_);
		NameID id(dictId);
		const char *retval = dict_->lookupName(oc_, id);
		if (retval) {
			// transcode.  Memory is in the _oc.data
			// DBT, which needs to be reallocated for
			// transcoding.  Use a std::string to hold
			// the original (utf-8) string
			std::string temp(retval);
			DbtOut &data = oc_.data();
			size_t len = temp.size() + 1;
			data.set(0, len * 3);
			xmlch_t *dest = (xmlch_t *) data.data;
			(void)NsUtil::nsFromUTF8(&dest,
						 (const xmlbyte_t*)temp.c_str(),
						 len, len);
			DBXML_ASSERT(dest == (xmlch_t *)data.data);
			return dest;
		}
	}
	return NULL;
}

// utility method to return a qualified name.  If the node is
// utf16, there is no transcoding to do, but there may be
// allocation/copying, if there is a namespace prefix present.
// if allocation is performed, the "owned" boolean is set to true,
// to indicate that the memory is owned by the caller, and must be
// freed.
const xmlch_t *
NsDoc::getQname(const nsName_t *name)
{
	const xmlch_t *prefix = 0;
	if (name->n_prefix != NS_NOPREFIX)
		prefix = getStringForID16(name->n_prefix);

	// must transcode and/or concat prefix:name
	// collect len in terms of utf-16 characters to hold
	size_t plen = 0;
	size_t len = 1;  // add trailing null
	if (prefix) {
		plen = NsUtil::nsStringLen(prefix) + 1; // add ':'
		len += plen;
	}
	len += name->n_text.t_len;

	xmlch_t *qname = (xmlch_t *)
		NsUtil::allocate(len << 1);
	xmlch_t *ptr = qname;
	if (prefix) {
		memcpy(qname, prefix, ((plen-1) << 1));
		qname[plen-1] = xmlchColon;
		ptr = qname + plen;
	}
	// truncate len back to just the text
	len = len - plen;
	// transcode to ptr
	NsUtil::nsFromUTF8(&ptr,
			   (const xmlbyte_t*)name->n_text.t_chars,
			   len, len);
	return qname;
}

const xmlch_t *
NsDoc::getText(const nsText_t *text)
{
	// must transcode
	size_t len = text->t_len + 1; // add null
	xmlch_t *newText = NULL;
	NsUtil::nsFromUTF8(&newText,
			   (const xmlbyte_t*)text->t_chars,
		    len, len);
	DBXML_ASSERT(newText);
	return newText;
}

//
// Access to persistent nodes
//

NsNode *
NsDoc::getNode(const NsNid &nid, bool getNext)
{
	if (!docdb_) {
		NsUtil::nsThrowException(XmlException::INTERNAL_ERROR,
					 "getNode: db is not initialized",
					 __FILE__, __LINE__);
	}
	DBXML_ASSERT((nid.getBytes()));
	
	// create data key
	NsNode *nsNode = 0;
	int err = 0;
	try {
		DbXmlDbt data;
		data.set_flags(DB_DBT_MALLOC);

		if(getNext) {
			err = NsFormat::getNextNodeRecord(*docdb_, oc_,
							      did_, nid,
							      &data, flags_);
			if(err == 0) {
				unsigned char *datap = (unsigned char *)data.data;
				const NsFormat &fmt = NsFormat::getFormat((int)(*datap));

				nsNode = fmt.unmarshalNodeData(
					datap,
					/*adoptBuffer*/true);
				DBXML_ASSERT(nsNode);

				// Set the node ID in nsNode from the key of the next
				// node, which is contained in context.key()
				DocID did;
				fmt.unmarshalNodeKey(did, nsNode->getFullNid(),
					(unsigned char *)oc_.key().data, /*copyStrings*/true);
			}
		} else {
			err = NsFormat::getNodeRecord(*docdb_, oc_, did_,
							  nid, &data, flags_);
			if(err == 0) {
				unsigned char *datap = (unsigned char *)data.data;
				const NsFormat &fmt = NsFormat::getFormat((int)(*datap));

				nsNode = fmt.unmarshalNodeData(
					datap, /*adoptBuffer*/true);
				DBXML_ASSERT(nsNode);

				// Set the node ID in nsNode
				nsNode->getFullNid()->copyNid((const xmlbyte_t *)nid,
							      nid.getLen());
			}
		}
		if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG))
			NsFormat::logNodeOperation(*docdb_, did_, nsNode,
						   (nsNode ? nsNode->getNid() : nid), 0,
						   (getNext ? "getNext" : "get"), err);
	}
	catch(XmlException &de) {
		// probably a deadlock, re-throw
		throw;
	}
	catch(...) {
		// fall through, return null; caller throws
	}

	// DB_NOTFOUND, DB_LOCK_DEADLOCK need to throw...
	// For now just throw on DEADLOCK and let callers
	// handle null return from DB_NOTFOUND
//	if(err == DB_NOTFOUND || err == DB_LOCK_DEADLOCK)
	if(err == DB_LOCK_DEADLOCK)
		throw XmlException(err);
	return nsNode;
}

