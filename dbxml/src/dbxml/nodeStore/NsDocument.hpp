//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSDOCUMENT_HPP
#define __DBXMLNSDOCUMENT_HPP

#include "NsDoc.hpp"
#include "../Document.hpp"
#include <db.h>
#include <set>
#include <map>

namespace DbXml
{

// forwards
class NsDomElement;
class NsDomObj;
class NsDomNode;
class NsDocumentDatabase;
class NsDocument;
class Document;
class NsDocInfo;
class DbWrapper;
	
/*
 * NsDocument
 *
 * The class that encapsulates Node Storage state for documents
 * that are being actively read/written
 *
 * There is one instance of this class for each XML document.
 */
class NsDocument : public NsDoc
{
public:	
	// Basic ctor, needs initialize call to specialize
	NsDocument(const Document *ownerDoc);
	~NsDocument();

	// init to create a new node storage doc
	void initDoc(Transaction *txn, DbWrapper *docdb,
		     DictionaryDatabase *ddb,
		     const DocID &docId, int cid, u_int32_t flags);
	// init for handling NsDom (used for both parse and
	// node storage materialization).  Requires dictionary.
	void initNsDom(const DocID &docId,
		       DictionaryDatabase *ddb);

	void completeNode(NsNode *node, size_t nodeSize = 0);
	//Used to grab space in the database before an 
	//internal node is fully explored
	void reserveNode(const NsNid &nid, size_t nodeSize);
	void completeDocument(void);
	//
	// access to document metadata (NsDocInfo object)
	//
	const xmlbyte_t *getXmlDecl() const;
	const xmlch_t *getXmlDecl16() const;
	void setXmlDecl(int32_t decl);
	const xmlbyte_t *getSniffedEncodingStr() const;
	void setSniffedEncodingStr(const xmlbyte_t *str);
	const xmlch_t *getEncodingStr16() const;
	const xmlbyte_t *getEncodingStr() const;
	void setEncodingStr(const xmlbyte_t *str);
	const xmlbyte_t *getStandaloneStr() const;
	void setStandalone(bool standalone);
	bool getIsStandalone() const;

	void setDocumentNode(NsNode *node) { docNode_ = node; }
	NsDomElement *getDocumentNode();
	//
	// DB access
	//
	void updateSpecialNodes(OperationContext &oc);
private:
	// internal get/put methods for nodes
 	void ensureDocInfo(bool fetch) const {
		if (!docInfo_)
			createDocInfo(fetch);
	}
	void createDocInfo(bool fetch) const;
private:
	// state for both parsing and access
	const Document *ownerDoc_;

	// xml decl info
	mutable NsDocInfo *docInfo_;

	NsNodeRef docNode_;
	bool addNsInfo_;

	//The dummy data used by reserveNode()
	DbtOut reserveNode_;
};

}

#endif
