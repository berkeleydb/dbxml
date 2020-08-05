//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSDOC_HPP
#define __DBXMLNSDOC_HPP

#include "NsNode.hpp"
#include "NsNid.hpp"
#include "NsFormat.hpp"
#include "../OperationContext.hpp"
#include "../DocID.hpp"
#include "../Document.hpp"
#include <db.h>
#include <set>
#include <map>

namespace DbXml
{

/*
 * NsDoc
 *
 * This utility class encapsulates enough state to fetch
 * and store nodes.  This includes:
 * o doc ID
 * o DbWrapper object for the target database
 * o DictionaryDatabase
 * o OperationContext
 * o Container ID
 * o flags
 */

class NsDoc
{
public:
	NsDoc() :docdb_(0), dict_(0), cid_(0), flags_(0) {}
	NsDoc(Transaction *txn, DbWrapper *docdb,
	      DictionaryDatabase *ddb,
	      const DocID &did, int cid, u_int32_t flags) {
		init(txn, docdb, ddb, did, cid, flags);
	}
	~NsDoc();
	void init(Transaction *txn, DbWrapper *docdb,
		  DictionaryDatabase *ddb,
		  const DocID &docId, int cid_, u_int32_t flags);

	//
	// accessors to namespace info (both utf-8 and utf-16)
	//
	int addIDForString(const char *strng, size_t len);
	const char *getStringForID(int32_t id);
	const xmlch_t *getStringForID16(int32_t id);

	const xmlch_t *getQname(const nsName_t *name);
	const xmlch_t *getText(const nsText_t *text);

	//
	// DB access
	//
	const DocID &getDocID(void) const { return did_; }
	Transaction *getTxn(void) const { return oc_.txn(); }
	DbWrapper *getDocDb() { return docdb_; }
	DictionaryDatabase *getDictionaryDatabase() { return dict_; }
	u_int32_t getFlags() const { return flags_; }
	int getContainerID() const { return cid_; }
	bool isSameDocument(const NsDoc *other) const {
		return this == other ||
			(did_ != 0 && did_ == other->getDocID() &&
			 cid_ == other->getContainerID());
	}
	NsNode *getNode(const NsNid &nid, bool getNext = false);
protected:
	mutable OperationContext oc_;
	DbWrapper *docdb_;
	DictionaryDatabase *dict_;
	DocID did_;
	int cid_;
	u_int32_t flags_;
};

}

#endif
