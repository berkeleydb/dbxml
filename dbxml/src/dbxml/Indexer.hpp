//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __INDEXER_HPP
#define	__INDEXER_HPP

#include <string>
#include <vector>

#include "Key.hpp"
#include "IndexEntry.hpp"
#include "IndexSpecification.hpp"
#include "nodeStore/EventWriter.hpp"
#include "OperationContext.hpp"
#include "KeyStash.hpp"
#include "StructuralStatsDatabase.hpp"

namespace DbXml
{
	
class IndexNodeInfo;
class Transaction;
class Container;
class Buffer;
class Document;
class NsDomElement;
class DocID;
class NameID;
class NsPushEventSource;
class IndexerState
{
public:
	IndexerState(int timezone);
	~IndexerState();
	void startNode(const IndexSpecification &indexSpecification,
		const char *const uri, const char *const localname,
		int attrIndex = -1);
	void characters(const char *s, size_t l);
	/// type should be NODE_ELEMENT, NODE_ATTRIBUTE, or NODE_METADATA
	bool isIndexed(const Index::Type &type) const;
	Key &getKey(Container &container, OperationContext &context);
	void reset();
	const IndexVector &iv() const
	{
		return *iv_;
	}
	int getAttrIndex() const { return attrIndex_; }
	bool isAttribute() const { return attrIndex_ != -1; }

        std::map<NameID, StructuralStats> stats;

	const char *getName() const {
		return (const char*)buffer_.getBuffer();
	}
	size_t getNameLen() const {
		// buffer has space for a null but that
		// should not be part of the namelen used for DBTs
		return buffer_.getOccupancy() - 1;
	}
	
	void setNotLeaf() { isLeaf_ = false; }
	bool isLeaf() const { return isLeaf_; }
private:
	IndexerState(const IndexerState&);
	void operator=(const IndexerState&);

	void saveUriname(const char* const uri, const char* const localname); // copies

	Key key_;
	const IndexVector *iv_;
	Buffer buffer_;
	int attrIndex_;
	bool isLeaf_;
};

class IndexerStateStack
{
public:
	IndexerStateStack(int timezone);
	~IndexerStateStack();

	IndexerState *push();
	IndexerState *top(unsigned int index = 0);
	void pop();
	bool empty();
	void reset();

	int getTimezone() const { return timezone_; }
private:
	IndexerStateStack(const IndexerStateStack&);
	void operator=(const IndexerStateStack &);

	std::vector<IndexerState*> v_;
	size_t top_;
	int timezone_;
};

class Indexer : public EventWriter
{
public:
	Indexer(int timezone);
	~Indexer();
	virtual void close();

	/*
	 * Generates a set of index keys for the document metadata based on
	 * the indexing specification. reset() must be called with a valid
	 * txn and container before this method is called.
	 *
	 * This method is not thread safe. One indexer is needed per thread.
	 *
	 * After the document is indexed the KeyStash::updateIndex() method
	 * must be called to write the keys to the index databases.
	 */
	void indexMetaData(const IndexSpecification &indexSpecification,
			   const Document &document, KeyStash &stash,
			   bool checkModified);
	void indexDefaultMetaData(const IndexSpecification &indexSpecification,
				  const Document &document, KeyStash &stash,
				  bool checkModified);
	/*
	 * Initialize an Indexer for handling content events for indexing.
	 * reset() must be called with a valid
	 * txn and container before this method is called.
	 *
	 * The NsPushEventSource::start() method must be called for the
	 * actual indexing to take place, after which the results are found
	 * in the KeyStash
	 *
	 * This method is not thread safe. One indexer is needed per thread.
	 *
	 * After the document is indexed the KeyStash::updateIndex() method
	 * must be called to write the keys to the index databases.
	 */
	void initIndexContent(const IndexSpecification &indexSpecification,
		const DocID &did, NsPushEventSource *source, KeyStash &stash,
		bool updateStats, bool writeNsInfo, bool isDelete);

	void resetContext(Container *container, OperationContext *oc);
	void reset();

	Container *getContainer()
	{
		return container_;
	}

	const StructuralStatsWriteCache &getStatsCache() const { return statsCache_; }
	void resetStats();

	IndexSpecification *getAutoIndexSpecification() {
		return autoIndexSpecification_;
	}
	
protected:
	// no need for copy and assignment
	Indexer(const Indexer&);
	Indexer &operator=(const Indexer &);

	// EventWriter interface
	virtual void writeStartDocument(const unsigned char *version,
					const unsigned char *encoding,
					const unsigned char *standalone);
	virtual void writeEndDocument();

	virtual void writeStartElementWithAttrs(const unsigned char *localName,
						const unsigned char *prefix,
						const unsigned char *uri,
						int attrCount,
						NsEventAttrList *attrs,
						IndexNodeInfo *ninfo,
						bool isEmpty);

	virtual void writeEndElementWithNode(const unsigned char *localName,
					     const unsigned char *prefix,
					     const unsigned char *uri,
					     IndexNodeInfo *ninfo);

	virtual void writeText(XmlEventReader::XmlEventType type,
			       const unsigned char *text,
			       size_t length);

	virtual void writeTextWithEscape(XmlEventReader::XmlEventType type,
					 const unsigned char *text,
					 size_t length,
					 bool needsEscape);
	// end of EventWriter API
	
	virtual bool collectFullStats(IndexNodeInfo *ninfo) { return true; }
	IndexerState *getParentState(int index = 1);

	void generateKeys(const IndexVector &iv, Index::Type pnk,
			  Index::Type mask, Key &key, KeyStash &stash);
	void checkUniqueConstraint(const Key &key);

	void addIDForString(const unsigned char *strng);

protected:	
	// The operation context within which the index keys are added
	// to the container.
	mutable OperationContext *oc_;

	// Reference to the Container that this Indexer indexes for.
	Container *container_;

	DictionaryDatabase *dict_;

	// The index keys that are to be created.
	const IndexSpecification *indexSpecification_;

	// The default indexes.
	const IndexVector *default_;

	// We check the index specification up front to see if there are any
	// attribute indexes. We can avoid some code if they're not needed.
	bool attributesIndexed_;

	// We check the index specification up front to see if there are any
	// element indexes. We can avoid some code if they're not needed.
	bool elementsIndexed_;

	// Whether the structural statistics should be calculated
	bool updateStats_;

	// Whether this index is adding or removing information
	bool isDelete_;

	// A mutable IndexEntry for constructing the indexes
	IndexEntry indexEntry_;

	// Data structures maintained whilst parsing the XML document.
	IndexerStateStack stateStack_;

	// The index keys are buffered up in a 'keyStash'.
	KeyStash *keyStash_;

	// For storing unique index keys seen in this document
	KeyStash uniqueKeysStash_;

	// For temporarily storing structural statistics
	StructuralStatsWriteCache statsCache_;

	// Used by auto-indexing
	IndexSpecification *autoIndexSpecification_;
};

}

#endif
