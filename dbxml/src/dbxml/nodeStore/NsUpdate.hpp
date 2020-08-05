//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSUPDATE_HPP
#define __DBXMLNSUPDATE_HPP

#include <string>
#include <map>
#include <vector>
#include "NsNid.hpp"

/*
 * NsUpdate.hpp
 *
 * Class and utility functions that implement node storage format
 * updating functions used to modify existing nodes.
 *
 * This could properly be considered part of NsFormat, but is split
 * into a separate class/file for size and complexity reasons.
 * See NsFormat.cpp for format details.
 */

class DynamicContext;
class ATQNameOrDerived;

namespace DbXml
{

class OperationContext;
class DbWrapper;
class Document;
class Container;
class DbXmlNodeImpl;
class DocID;
class NsNode;
class NsFullNid;
class NsNid;
class NsDomNode;
class XmlManager;
class NsQName;
struct nsTextList;
struct nsTextEntry;
class NsNodeRef;
class IndexSpecification;
class NsReindexer;
	
// NidMarker is used to track nodes that need
// reindexing at the end of an update operation
class NidMarker {
public:
	NidMarker() {}
	NidMarker(Document *doc, const NsNid &nid,
		  bool updateStats,
		  bool attrsOnly, bool fullGen)
		: doc_(doc), attrsOnly_(attrsOnly),
		  fullGen_(fullGen),
		  updateStats_(updateStats),
		  nid_(nid) {}
	NidMarker(const NidMarker &other)
		: doc_(other.doc_), attrsOnly_(other.attrsOnly_),
		  fullGen_(other.fullGen_),
		  updateStats_(const_cast<NidMarker&>(other).updateStats_),
		  nid_(const_cast<NidMarker&>(other).nid_.get()) {}
	Document *getDocument() {return doc_;}
	NsFullNid *getNid() { return nid_.get(); }
	bool getUpdateStats() const { return updateStats_; }
	void setAttributes(bool attrs) {
		if (!attrs && attrsOnly_)
			attrsOnly_ = false;
	}
	bool attributesOnly() const { return attrsOnly_; }
	bool fullGeneration() const { return fullGen_; }
private:
	Document *doc_;
	bool attrsOnly_;
	bool fullGen_;
	bool updateStats_;
	NsFullNidWrap nid_;
};

class IndexInfo {
public:
	IndexInfo(Document &document, int cid, OperationContext &oc);
	~IndexInfo();
	Container *getContainer() { return cont_; }
	IndexSpecification *getIndexSpecification() { return is_; }
	IndexSpecification *getAutoIndexSpecification() { return autoIs_; }
	OperationContext &getOperationContext() { return oc_; }
private:
	IndexSpecification *is_;
	IndexSpecification *autoIs_;
	Container *cont_;
	OperationContext &oc_;
};
	
typedef std::multimap<std::string, int> UpdateMap;
typedef std::map<std::string, Document*> RewriteMap;
typedef std::map<std::string, NidMarker> NidMap;
typedef std::map<int, IndexInfo*> IndexInfoMap;

class NsUpdate {
public:
	NsUpdate() {}
	~NsUpdate();
	// insertion
	void insertElement(const DbXmlNodeImpl &node,
			   const DbXmlNodeImpl &parent,
			   const NsDomNode *next,
			   XmlManager &mgr,
			   Document &document,
			   OperationContext &oc,
			   DynamicContext *context,
                           bool firstOrAfter);

	void insertText(const DbXmlNodeImpl &node,
			const DbXmlNodeImpl &parent,
			const NsDomNode *next,
			Document &document,
			OperationContext &oc,
			DynamicContext *context);

	void insertAttributes(
		const std::vector<const DbXmlNodeImpl*> &attributes,
		const DbXmlNodeImpl &parent,
		Document &document,
		OperationContext &oc,
		DynamicContext *context);

	// removal
	void removeDocument(const DbXmlNodeImpl &node,
			    Container &container,
			    XmlManager &mgr,
			    OperationContext &oc);
	void removeAttribute(const DbXmlNodeImpl &node,
			     Document &document,
			     OperationContext &oc,
			     DynamicContext *context);
	void removeElement(const DbXmlNodeImpl &node,
			   Document &document,
			   OperationContext &oc,
			   DynamicContext *context);
	void removeText(const DbXmlNodeImpl &node,
			Document &document,
			OperationContext &oc,
			DynamicContext *context);
	// rename
	void renameElement(const DbXmlNodeImpl &node,
			   ATQNameOrDerived *qname,
			   Document &document,
			   OperationContext &oc,
			   DynamicContext *context);
	// used to rewrite modified wholedoc documents
	void completeUpdate(XmlManager &mgr,
			    OperationContext &oc,
			    DynamicContext *context);
	// adds auto-generated indexes if any
	void addAutoIndexes();
private:
	// internal utility functions
	int getAttributeIndex(const DbXmlNodeImpl &node) const;
	int getTextIndex(const DbXmlNodeImpl &node) const;
	int getTextIndex(int index, const NsNid &nid,
			 const DocID &did, const std::string &cname);
	int getTextIndex(const std::string &key, int index) const;
	void attributeRemoved(const DbXmlNodeImpl &node);
	void textRemoved(const DbXmlNodeImpl &node);
	void textRemoved(int index, const NsNid &nid,
			 const DocID &did,
			 const std::string &cname);
	void textInserted(int index, const DbXmlNodeImpl &node);
	void textInserted(int index, const NsNid &nid,
			  const DocID &did,
			  const std::string &cname);
	struct nsTextList *coalesceTextNodes(NsNode *from,
					     NsNode *to,
					     int startIndex,
					     int endIndex,
					     bool toChild,
					     Document &doc);
	struct nsTextList *insertInTextList(struct nsTextEntry *text,
					    int index,
					    NsNode *to,
					    bool replace,
					    bool asChild);
	// reindexing
	IndexInfo *getIndexInfo(Document &document,
					   OperationContext &oc);
	void removeAttributeIndexes(NsReindexer &reindexer,
				    NsNodeRef &parentNode, Document &doc,
				    OperationContext &oc);
	void removeElementIndexes(const DbXmlNodeImpl &node, Document &doc,
				  bool updateStats, bool targetFullGen,
				  NsNidWrap &vindexNid, OperationContext &oc);
	void markElement(NidMap &map, const NsNid &, bool updateStats,
			 Document &doc, bool attributes, bool fullGen);
	void markElement(NidMap &map, const std::string &key, const NsNid &,
			 bool updateStats, Document &doc, bool attributes, bool fullGen);
	bool indexesRemoved(const std::string &key, bool attributes, bool &fullGen) const;
	void reindex(OperationContext &oc);
	void coalesceText(OperationContext &oc);
	
	void markForUpdate(Document *doc, OperationContext &oc);
public:
	static NsNode *fetchNode(const DbXmlNodeImpl &node,
				 DbWrapper &db,
				 OperationContext &oc);
	static NsNode *fetchNode(const NsNid &nid,
				 const DocID &did,
				 DbWrapper &db,
				 OperationContext &oc);
private:
	void insertContentIntoEmptyDoc(const DbXmlNodeImpl &node,
				       Document &document,
				       OperationContext &oc,
				       DynamicContext *context);
	bool insertTextIntoEmptyDoc(const DbXmlNodeImpl &node,
				    Document &document,
				    OperationContext &oc,
				    DynamicContext *context);
	void doCoalesceText(NsNode *node, Document &doc, OperationContext &oc);
	bool coalesceEntry(struct nsTextList *list, struct nsTextEntry *entries,
			   int &lastType, int index);
	static void putNode(const NsNode *node,
			    DbWrapper &db,
			    const DocID &did,
			    OperationContext &oc);
	static void deleteTree(NsNode *node,
			       DbWrapper &db,
			       const DocID &did,
			       OperationContext &oc);
	static void updateLastDescendants(NsDomNode *node,
					  const NsFullNid *nid,
					  DbWrapper &db,
					  const DocID &did,
					  OperationContext &oc);
private:
	UpdateMap attrMap_;
	UpdateMap textInsertMap_;
	UpdateMap textDeleteMap_;
	RewriteMap documentUpdates_;
	NidMap elements_;
	NidMap textCoalesce_;
	IndexInfoMap indexMap_;
};

}

#endif
