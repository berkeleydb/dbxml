//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//

#include "../DbXmlInternal.hpp"
#include "NsReindexer.hpp"
#include "NsUpdate.hpp"
#include "NsDom.hpp"
#include "NsEventReader.hpp"
#include "EventReaderToWriter.hpp"
#include "../Document.hpp"
#include "../UTF8.hpp"

using namespace DbXml;
using namespace std;

class ElementIndexList;

class ElementIndexListEntry {
public:
	ElementIndexListEntry(NsNode *node, bool presenceIndex,
			      bool valueIndex, bool isTarget) :
		node_(node), pindex_(presenceIndex), vindex_(valueIndex),
		isTarget_(isTarget) {}
	ElementIndexListEntry(const ElementIndexListEntry &other)
		: node_(other.node_), pindex_(other.pindex_),
		  vindex_(other.vindex_), isTarget_(other.isTarget_) {}
private:
	NsNodeRef node_;
	bool pindex_;
	bool vindex_;
	bool isTarget_;
	friend class ElementIndexList;
};

// list of elements to index for an element reindex operation
class ElementIndexList {
public:
	ElementIndexList(NsReindexer &reindexer) : reindexer_(reindexer) {}
	void push(NsNode *node, const std::string &uri, bool presenceIndex,
		  bool valueIndex, bool isTarget);
	bool generate(EventWriter &writer, bool targetFullGen);
	void generateStartEvent(NsNode *node, const std::string &uri,
				EventWriter &writer);
	void generateEndEvent(NsNode *node, EventWriter &writer);
	void generateEvents(NsNode *node, EventWriter &writer);
private:
	vector<ElementIndexListEntry> elements_;
	vector<std::string> elementUris_;
	NsReindexer &reindexer_;
};

NsReindexer::NsReindexer(Document &document,
			 IndexInfo *indexInfo,
			 int timezone,
			 bool forDelete, bool updateStats)
	: Indexer(timezone), key_(timezone), parentKey_(timezone),
	  document_(document), oldNodeSize_(0)
{
	if ((indexInfo == 0) || (document.getContainerID() == 0))
		return;
	container_ = indexInfo->getContainer();
	DBXML_ASSERT(container_);
	is_ = indexInfo->getIndexSpecification();
	DBXML_ASSERT(is_);
	// Because is_ is reused it may need to be reset
	if (forDelete)
		is_->set(Index::INDEXER_DELETE);
	else
		is_->set(Index::INDEXER_ADD);
	// init remaining Indexer state
	oc_ = &indexInfo->getOperationContext();
	initIndexContent(*is_, document.getID(), 0, stash_,
			 (updateStats &&
			  (container_->getStructuralStatsDB() != 0)), // updateStats
			 false, // writeNsInfo
			 forDelete);
	dictionary_ = container_->getDictionaryDatabase();
}

void NsReindexer::updateIndexes()
{
	if (!container_)
		return;
	// update the actual index(es) involved
	stash_.updateIndex(*oc_, container_);
	if (updateStats_) {
		container_->getStructuralStatsDB()->addStats(*oc_, getStatsCache());
		resetStats(); // TBD -- may not be necessary
	}
}

// This code is the same for removal and insertions.
// The difference is in the KeyStash (for delete or not)
void NsReindexer::indexAttributes(NsNodeRef &node)
{
	if (attributesIndexed_ && node->hasAttributes()) {
		for (int i = 0; i < (int)node->numAttrs(); i++) {
			nsAttr_t *attr = node->getAttr(i);
			indexAttribute((const char *)attr->a_name8,
				       node->attrUri(i),
				       node, i);
		}
	}
}

void
NsReindexer::indexElement(NsNodeRef &node, bool targetFullGen, NsNidWrap &vindexNid)
{
	NsNodeRef newNode(node);
	if (elementsIndexed_ || updateStats_) {
		bool isTarget = true;
		const DocID &did = document_.getID();
		DbWrapper &db = *document_.getDocDb();
		ElementIndexList nodes(*this);
		// special-case situation where the node is the document.
		// this means that a new root element has been inserted, so
		// reindex the entire doc (which is probably just the new
		// content)
		if (newNode->isDoc()) {
			// force full reindex...
			partialIndexNode_ = *newNode;
			nodes.push(*newNode, "doc", false, false, true);
			targetFullGen = true; // force this for docs
		} else {
			if (updateStats_)
				partialIndexNode_ = *node;
			do {
				bool hasValueIndex = false;
				bool hasPresenceIndex = false; // may not be necessary
				lookupIndexes(newNode, hasValueIndex, hasPresenceIndex);
				if (hasValueIndex)
					vindexNid.copy(newNode->getNid());
				string uri;
				if (newNode->hasUri())
					uri = lookupUri(newNode->uriIndex());
				nodes.push(*newNode, uri, hasPresenceIndex,
					   hasValueIndex, isTarget);
				isTarget = false;
				newNode = NsUpdate::fetchNode(NsNid(newNode->getParentNid()),
							      did, db, *oc_);
			} while(!newNode->isDoc());
		}
		(void) nodes.generate(*this, targetFullGen);
		partialIndexNode_ = 0;
        } else if (attributesIndexed_) {
                //[#17671] Make sure any attribute indexes are updated if no element indexes exist
                indexAttributes(node);
        }
}

// reindex if:
//  o there is a container and
//  o it's a node container with a node index
// all other containers require full document reindexing on update
bool NsReindexer::willReindex() const
{
	return (container_ &&
		container_->isNodeContainer() &&
                container_->nodesIndexed());
}

// Determine what indexes are present on the node
void NsReindexer::lookupIndexes(NsNodeRef &node, bool &hasValueIndex,
				bool &hasPresenceIndex)
{
	// does this element have a value index (equality or substring)
	const IndexVector *iv = 0;
	
	Buffer buf;
	makeUriName((node->hasUri() ? node->uriIndex() : NS_NOURI),
		    (const char *)node->getNameChars(), buf);
	const char *uriname = (const char *)buf.getBuffer();

	if ((iv = is_->getIndexOrDefault(uriname)) != 0) {
		if (iv->isEnabled(Index::NODE_ELEMENT, Index::NODE_MASK)) {
			if ((iv->isEnabled(Index::XEE, Index::NK_MASK) ||
			     iv->isEnabled(Index::XES, Index::NK_MASK)))
				hasValueIndex = true;
			if ((iv->isEnabled(Index::EEP,Index::PNK_MASK)) ||
			    (iv->isEnabled(Index::NEP,Index::PNK_MASK)))
				hasPresenceIndex = true;
		}
	}
}

void NsReindexer::indexAttribute(const char *aname,
				 int auri, NsNodeRef &parent,
				 int attrIndex)
{
	DBXML_ASSERT(container_ && attributesIndexed_);
	// some of the work done here is redundant wrt to Indexer,
	// but it's cheaper here
	const IndexVector *iv = 0;
	Buffer buf;
	const char *uriname = makeUriName(auri, aname, buf);
	if ((iv = is_->getIndexOrDefault(uriname)) != 0) {
		if (iv->isEnabled(Index::NODE_ATTRIBUTE, Index::NODE_MASK)) {
			NsNodeIndexNodeInfo ninfo(parent.get());
			indexEntry_.setIndexNodeInfo(&ninfo);
			indexEntry_.setLastDescendant(
				NsNid(parent->getLastDescendantNidOrSelf()));
			key_.reset();
			parentKey_.reset();
			// add value and name id to key
			const char *val = aname;
			while(*(val++)); // get past null to value
			key_.addValue(val, ::strlen(val));
			NameID &id = key_.getID1();
			dictionary_->lookupIDFromName(
				*oc_, uriname, id, /* define */ true);
			indexEntry_.setIndex(attrIndex);
			// generate for node indexes
			generateKeys(*iv, Index::NA, Index::PN_MASK, key_,
				     stash_);

			// add parent id, and generate for edge indexes
			buf.reset();
			const char *parentUriname =
				makeUriName((parent->hasUri() ? parent->uriIndex() : NS_NOURI),
					    (const char *)parent->getNameChars(), buf);
			NameID &parentID = parentKey_.getID1();
			dictionary_->lookupIDFromName(
				*oc_, parentUriname, parentID,
				/* define */ false); // name id should exist
			key_.setID2(parentID);
			generateKeys(*iv, Index::EA, Index::PN_MASK, key_,
				     stash_);
		}
	}
}

// virtual
bool NsReindexer::collectFullStats(IndexNodeInfo *ninfo)
{
	// partialIndexNode_ or descendent, collect full stats.
	// once the PIN is hit, clear it to avoid the overhead.
	DBXML_ASSERT(updateStats_);
	bool retVal = false;
	if (partialIndexNode_) {
		// ninfo will be null on document node which happens when
		// inserting content into an empty doc
		// (see Indexer::writeEndDocument())
		NsNid nid(ninfo ? ninfo->getNodeID() : *NsNid::getRootNid());
		int ret =  partialIndexNode_->getNid().
			compareNids(nid);
		if (ret == 0) {
			retVal = true; // hit target, stop collecting after
			partialIndexNode_ = 0;
		} else if (ret < 0) {
			// if last desc is >= to the nid, it's in the tree
			NsNid last(partialIndexNode_->getLastDescendantNidOrSelf());
			if (last.compareNids(nid) >= 0)
				retVal = true;
		}
	}
	return retVal;
}

// save original node size for this node for attributes changes
void NsReindexer::saveStatsNodeSize(NsNodeRef &node)
{
	if (container_->isNodeContainer() && container_->getStructuralStatsDB()) {
		oldNodeSize_ = NsFormat::getNodeDataSize(*node);
		DBXML_ASSERT(oldNodeSize_ != 0);
	}
}

// get new size for this node for attributes changes and update
// stats.  This is unfortunately necessary to track
// increases/decreases in stats related to node sizes when
// attributes are modified
void NsReindexer::updateStatsNodeSize(NsNodeRef &node)
{
	if (oldNodeSize_ != 0) {
		size_t newNodeSize_ = NsFormat::getNodeDataSize(*node);
		if (newNodeSize_ != oldNodeSize_) {
			// need to update stats
			size_t change;
			bool subtract;
			if (newNodeSize_ > oldNodeSize_) {
				change = newNodeSize_ - oldNodeSize_;
				subtract = false;
			} else {
				change = oldNodeSize_ - newNodeSize_;
				subtract = true;
			}
			NsNodeRef currentNode = node;
			std::map<NameID, StructuralStats> smap;
			StructuralStats *stats = &smap[0];
			stats->sumSize_ = change; // start with just size
			while (true) {
				NameID id = getNameID(currentNode);
				if (subtract)
					statsCache_.subtract(id, smap);
				else
					statsCache_.add(id, smap);
				if (currentNode->isDoc())
					break;
				currentNode = NsUpdate::fetchNode(NsNid(currentNode->getParentNid()),
								  document_.getID(),
								  *document_.getDocDb(), *oc_);
				// no need for full stats reset, change different
				// fields as we walk up past parent to ancestors
				if (stats->sumSize_) {
					// for direct parent, change childSize
					stats->sumSize_ = 0;
					stats->sumChildSize_ = change;
				} else if (stats->sumChildSize_)
					stats->sumChildSize_ = 0; // now past parent
				stats->sumDescendantSize_ = change;
			}
			int err = container_->getStructuralStatsDB()->
				addStats(*oc_, statsCache_);
			if (err != 0)
				throw XmlException(err);
		}
	}
}

const char *NsReindexer::lookupUri(int uriIndex)
{
	
	if (uriIndex != NS_NOURI) {
		NameID id(uriIndex);
		return dictionary_->lookupName(*oc_, id);
	}
	return 0;
}

const char *NsReindexer::makeUriName(int uriIndex, const char *lname,
				     Buffer &buffer)
{
	const char *uri = lookupUri(uriIndex);
	size_t lnameLen = NsUtil::nsStringLen((const unsigned char *)lname);
	size_t uriLen = 0;
	if (uri) {
		uriLen = NsUtil::nsStringLen((const unsigned char *)uri);
		++uriLen; // ensure null termination
	} else
		++lnameLen; // ensure null termination

	Name::writeToBuffer(buffer, lname, lnameLen,
			    uri, uriLen);
	return (const char *)buffer.getBuffer();
}

NameID NsReindexer::getNameID(NsNodeRef &node)
{
	NameID id;
	Buffer buf;
	if (node->isDoc())
		return NameID(2); // document node
	const char *uriname = makeUriName(
		(node->hasUri() ? node->uriIndex() : NS_NOURI),
		(const char *)node->getNameChars(), buf);
	container_->getDictionaryDatabase()->
		lookupIDFromName(*oc_, uriname, id, /* define */ false);
	return id;
}

EventReader *NsReindexer::getEventReader(NsNode *node)
{
	NsNid nid(node->getNid());
	return new NsEventReader(
		oc_->txn(),
		document_.getDocDb(),
		dictionary_,
		document_.getID(),
		document_.getContainerID(),
		0, // no flags
		NS_EVENT_BULK_BUFSIZE,
		&nid);
}
	
// ElementIndexList

void ElementIndexList::push(NsNode *node, const std::string &uri, bool presenceIndex,
			    bool valueIndex, bool isTarget)
{
	elements_.push_back(ElementIndexListEntry(node, presenceIndex, valueIndex,
						  isTarget));
	elementUris_.push_back(uri);
}

static void logEvent(NsNode *node, const char *event)
{
#if 0
	const char *name = (node? (const char *)node->getNameChars() : 0);
	std::cout << "Event: " << event;
	if (name)
		std::cout << ", name " << name;
	else
		std::cout << ", noname";
	if (node) {
		std::cout << ", Nid: ";node->getNid().displayNid(std::cout);
	}
	std::cout << std::endl;
#endif
}

// This is the guts of reindexing elements.
//
// Generate manual start/end events for the parent of
// the top-most indexed element in the tree.  Ignore
// elements from there to the root, since no indexes are affected.
//
// Generate full events for indexed nodes if asked, return whether
// or not full generation was done somewhere in the tree, which tells
// the user whether further reindexing may be necessary.
//
bool ElementIndexList::generate(EventWriter &writer, bool targetFullGen)
{

	int curIndex = (int)(elements_.size()) - 1;
	int stopIndex = -1;
	bool didFullGeneration = false;
	
	logEvent(0, "Start generation");
	// if not generating for an entire document, write the doc event
	if (elements_[curIndex].node_->isDoc()) {
		generateEvents(elements_[curIndex].node_.get(), writer);
		didFullGeneration = true;
	} else {
		// start the doc
		writer.writeStartDocument(0,0,0);
		// generate simple start events until reaching either (1)
		// a value index or (2) the target.  If the target do full
		// generation if requested.
		while (curIndex >= 0) {
			if (elements_[curIndex].vindex_ ||
			    (elements_[curIndex].isTarget_ && targetFullGen)) {
				generateEvents(elements_[curIndex].node_.get(), writer);
				stopIndex = curIndex;
				didFullGeneration = true;
				break;
			}
			// else simple generation
			generateStartEvent(elements_[curIndex].node_.get(),
					   elementUris_[curIndex],
					   writer);
			--curIndex;
		}
		// now generate end events, in reverse order
		curIndex = stopIndex + 1;
		while (curIndex < (int)elements_.size()) {
			generateEndEvent(elements_[curIndex].node_.get(), writer);
			++curIndex;
		}
		// end the doc
		writer.writeEndDocument();
	}
	return didFullGeneration;
}

void ElementIndexList::generateStartEvent(NsNode *node,
					  const std::string &uri,
					  EventWriter &writer)
{
	logEvent(node, "Start");
	NsNodeIndexNodeInfo ninfo(node);
	writer.writeStartElementWithAttrs(
		(const unsigned char *)node->getNameChars(),
		0, // prefix -- not used
		(const unsigned char *)(uri.c_str()),
		0, 0, // no attrs, attrlist
		&ninfo,
		false);
}

void ElementIndexList::generateEndEvent(NsNode *node, EventWriter &writer)
{
	logEvent(node, "End");
	NsNodeIndexNodeInfo ninfo(node);
	writer.writeEndElementWithNode(0, 0, 0, // no localname, prefix, uri
				       &ninfo);
}

void ElementIndexList::generateEvents(NsNode *node, EventWriter &writer)
{
	logEvent(node, "Fullgen");
	AutoDelete<EventReader> reader(reindexer_.getEventReader(node));
	// run the events (do not donate objects to r2w)
	EventReaderToWriter r2w(*reader, writer, false, false);
	r2w.start();
}


