//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __DBXMLNSREINDEXER_HPP
#define __DBXMLNSREINDEXER_HPP

#include "../Indexer.hpp"
#include "../NameID.hpp"

namespace DbXml {

class NsNodeRef;
class NsNode;
class Document;
class DbXmlNodeImpl;
class NsDomAttr;
class NsDomElement;
class DictionaryDatabase;
class EventReader;
class IndexInfo;
class Buffer;
	
// Only node containers with node level indexing can be parially
// reindexed.  Document level indexing cannot be partially reindexed
// because it results in index entries being deleted even if other
// parts of the document not deleted match the index entry.  Whole
// document containers with node level indexing have to be totally 
// reindexed because updates change the node ids.
class NsReindexer : public Indexer {
public:
	NsReindexer(Document &document,
		    IndexInfo *indexInfo,
		    int timezone,
		    bool forDelete, bool updateStats);


	void indexAttributes(NsNodeRef &node);
	 // returns true if ancestor has value index
	void indexElement(NsNodeRef &node, bool targetFullGen,
		     NsNidWrap &vindexNid);
	void lookupIndexes(NsNodeRef &node, bool &hasValueIndex,
			   bool &hasPresenceIndex);

	void updateIndexes();
	bool willReindex() const;
	EventReader *getEventReader(NsNode *node);
	void saveStatsNodeSize(NsNodeRef &node);
	void updateStatsNodeSize(NsNodeRef &node);
protected:
	NameID getNameID(NsNodeRef &node);
	
	const char *makeUriName(int uriIndex, const char *lname, Buffer &buffer);
	const char *lookupUri(int uriIndex);
	void indexAttribute(const char *aname, int auri,
			    NsNodeRef &parent, int index);
	virtual bool collectFullStats(IndexNodeInfo *ninfo);
private:
	IndexSpecification *is_;
	KeyStash stash_;
	Key key_;
	Key parentKey_;
	Document &document_;
	// this is redundant wrt Indexer, but dict_ in Indexer triggers
	// behavior that this class does not want
	DictionaryDatabase *dictionary_;
	// set if doing partial indexing
	NsNode *partialIndexNode_;
	size_t oldNodeSize_;
};

}
#endif
