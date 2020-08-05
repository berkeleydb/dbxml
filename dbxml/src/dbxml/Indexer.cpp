//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlNamespace.hpp"
#include "Container.hpp"
#include "Indexer.hpp"
#include "Log.hpp"
#include "SyntaxManager.hpp"
#include "UTF8.hpp"
#include "Document.hpp"
#include "Manager.hpp"
#include "KeyStash.hpp"
#include "ScopedPtr.hpp"
#include "IndexEntry.hpp"

#include "nodeStore/NsUtil.hpp"
#include "nodeStore/NsEvent.hpp"
#include "nodeStore/NsDocument.hpp"
#include "nodeStore/NsXercesIndexer.hpp"
#include "nodeStore/NsDom.hpp"
#include "nodeStore/NsDocumentDatabase.hpp"

#include <string>

using namespace DbXml;
using namespace std;

static const xmlbyte_t *xmlnsUri = (const xmlbyte_t *)"http://www.w3.org/2000/xmlns/";

// IndexerState

IndexerState::IndexerState(int timezone)
	: key_(timezone),
	  iv_(0),
	  attrIndex_(-1),
	  isLeaf_(true) // is leaf until proven otherwise
{}

IndexerState::~IndexerState()
{
}

void IndexerState::saveUriname(const char* const uri, const char* const localname)
{
	key_.reset();
	size_t u_len = uri == 0 ? 0 : NsUtil::nsStringLen((const unsigned char*)uri);
	size_t l_len = NsUtil::nsStringLen((const unsigned char*)localname);
	// need to ensure null termination in buffer.
	if (u_len)
		++u_len;
	else
		++l_len;
	buffer_.reset();
	Name::writeToBuffer(buffer_, localname, l_len,
			    uri, u_len);
}

void IndexerState::startNode(const IndexSpecification &indexSpecification,
	const char *const uri, const char *const localname, int attrIndex)
{
	saveUriname(uri, localname);
	iv_ = indexSpecification.getIndexOrDefault(getName());
	attrIndex_ = attrIndex;

        stats.clear();
	isLeaf_ = true;  // reset "leaf-ness"
}

void IndexerState::characters(const char *s, size_t l)
{
	key_.addValue(s, l);
}

bool IndexerState::isIndexed(const Index::Type &type) const
{
	return iv_ && iv_->isEnabled(type, Index::NODE_MASK);
}

Key &IndexerState::getKey(Container &container, OperationContext &context)
{
	NameID &id = key_.getID1();
	if (id == 0) {
		DbtIn uriname((void *) getName(), getNameLen());
		container.getDictionaryDatabase()->lookupIDFromQName(
			context, uriname, id, /*define=*/true);
	}
	return key_;
}

void IndexerState::reset()
{
	key_.reset();
	iv_ = 0;
}

// IndexerStateStack

IndexerStateStack::IndexerStateStack(int timezone)
	: top_(0),
	  timezone_(timezone)
{
	v_.reserve(16);
}

IndexerStateStack::~IndexerStateStack()
{
	std::vector<IndexerState*>::iterator i;
	for(i = v_.begin(); i != v_.end(); ++i) {
		delete *i;
	}
}

IndexerState *IndexerStateStack::push()
{
	IndexerState *is = 0;
	if (top_ == v_.size()) {
		v_.push_back(new IndexerState(timezone_));
		is = v_[top_];
		++top_;
	} else {
		is = v_[top_];
		++top_;
		is->reset();
	}
	return is;
}

IndexerState *IndexerStateStack::top(unsigned int index)
{
	IndexerState *is = 0;
	if (top_ > index && top_ != 0) {
		is = v_[top_ - index - 1];
	}
	return is;
}

void IndexerStateStack::pop()
{
	if (top_ != 0) {
		--top_;
	}
}

bool IndexerStateStack::empty()
{
	return (top_ == 0);
}

void IndexerStateStack::reset()
{
  top_ = 0;
}

// Indexer

Indexer::Indexer(int timezone)
	: container_(0),
	  dict_(0),
	  indexSpecification_(0),
	  attributesIndexed_(false),
	  elementsIndexed_(false),
	  updateStats_(false),
	  isDelete_(false),
	  stateStack_(timezone),
	  autoIndexSpecification_(0)
{
}

Indexer::~Indexer()
{
	if (autoIndexSpecification_)
		delete autoIndexSpecification_;
}

void Indexer::close()
{
	delete this;
}

void Indexer::indexMetaData(const IndexSpecification &indexSpecification,
			    const Document &document, KeyStash &stash,
			    bool checkModified)
{
	//
	// Build index keys for the document metadata. We use the
	// IndexSpecification::getIndexed() method to avoid using
	// a metadata iterator, which would cancel any benefit
	// gained from a lazy document.
	//
	// Note that metadata indexes don't have node/edge set on them.
	// Special case a default metadata index
	//
	if (indexSpecification.isMetaDataDefaultIndex()) {
		indexDefaultMetaData(indexSpecification, document, stash,
				     checkModified);
		return;
	}
		
	IndexSpecification::NameList md_names = indexSpecification.
		getIndexed(Index::NODE_METADATA, Index::NODE_MASK);
	if(!md_names.empty()) {
		indexEntry_.setDocID(document.getID());
		indexEntry_.setIndexNodeInfo(0);

		Key mdkey(stateStack_.getTimezone());
		IndexSpecification::NameList::iterator end = md_names.end();
		for(IndexSpecification::NameList::iterator it = md_names.begin();
		    it != end; ++it) {
			const Name &mdname = *it;
			if(!checkModified || document.isMetaDataModified(mdname)) {
				const MetaDatum *md = document.getMetaDataPtr(mdname);
				if(md != 0 && md->canBeIndexed()) {
					const IndexVector *iv = indexSpecification.
						getIndexOrDefault(mdname.getURIName().c_str());

					if(iv && iv->isEnabled(Index::NODE_METADATA, Index::NODE_MASK)) {
						mdkey.reset();
						container_->getDictionaryDatabase()->lookupIDFromName(*oc_, mdname, mdkey.getID1(), /*define=*/true);
						mdkey.setValue((const char*)md->getDbt()->data,
							       md->getDbt()->size - 1);

						mdkey.setIndex(Index::NM);
						generateKeys(*iv, Index::NM, Index::PN_MASK, mdkey, stash);
					}
				}
			}
		}
	}
}

// Special case for indexing metadata if there is a default m-d index.
// In this case, using a metadata iterator on the document cannot be
// avoided, so it defaults lazy docs -- a point to remember when using
// default m-d indexes.
void Indexer::indexDefaultMetaData(
	const IndexSpecification &indexSpecification,
	const Document &document, KeyStash &stash,
	bool checkModified)
{
	IndexSpecification::NameList md_names = indexSpecification.
		getIndexed(Index::NODE_METADATA, Index::NODE_MASK);
	
	indexEntry_.setDocID(document.getID());
	indexEntry_.setIndexNodeInfo(0);

	Key mdkey(stateStack_.getTimezone());
	MetaData::const_iterator i;
	for (i = document.metaDataBegin(); i != document.metaDataEnd(); ++i) {
		if (!checkModified || (*i)->isModified()) {
			const MetaDatum *md = *i;
			if(md != 0 && md->canBeIndexed()) {
				const Name &mdname = (*i)->getName();
				const IndexVector *iv = indexSpecification.
					getIndexOrDefault(mdname.getURIName().c_str());
				if(iv && iv->isEnabled(Index::NODE_METADATA,
						       Index::NODE_MASK)) {
					mdkey.reset();
					container_->getDictionaryDatabase()->lookupIDFromName(*oc_, mdname, mdkey.getID1(), /*define=*/true);
					mdkey.setValue((const char*)md->getDbt()->data,
						       md->getDbt()->size - 1);
					
					mdkey.setIndex(Index::NM);
					generateKeys(*iv, Index::NM, Index::PN_MASK, mdkey, stash);
				}
			}
		}
	}
}

void Indexer::resetContext(Container *container, OperationContext *oc)
{
	oc_ = oc;
	container_ = container;
	dict_ = 0;
	reset();
}

void Indexer::reset()
{
	// Cleanup
	indexEntry_.reset();
	stateStack_.reset();
	uniqueKeysStash_.reset();
	if (autoIndexSpecification_) {
		delete autoIndexSpecification_;
		autoIndexSpecification_ = 0;
	}
}

void Indexer::resetStats()
{
	statsCache_.clear();
}

// EventWriter interface

void Indexer::writeStartDocument(const unsigned char *version,
	const unsigned char *encoding, const unsigned char *standalone)
{
	if(elementsIndexed_ || attributesIndexed_ || updateStats_) {
		IndexerState *eis = stateStack_.push();
		eis->startNode(*indexSpecification_, metaDataNamespace_uri, metaDataName_root);
	}
}

void Indexer::writeStartElementWithAttrs(const unsigned char *localName,
					 const unsigned char *prefix,
					 const unsigned char *uri,
					 int attrCount,
					 NsEventAttrList *attrs,
					 IndexNodeInfo *ninfo,
					 bool isEmpty)
{
	// If parsing for insertion into wholedoc container, add
	// namespace information to dictionary
	// NOTE: it'd be better to manage this as an EventWriter
	// inserted into the chain before the indexer.  If done,
	// think about how to manage object lifetimes in
	// Container::addDocumentAsEventWriter.  TBD.
	if (dict_) {
		DBXML_ASSERT(container_->getContainerType() ==
			     XmlContainer::WholedocContainer);
		if (prefix)
			addIDForString(prefix);
		if (uri)
			addIDForString(uri);
		for (int i = 0; i < attrCount; ++i) {
			const xmlbyte_t *t = attrs->uri(i);
			if (t) {
				addIDForString(t);
				t = attrs->prefix(i);
				if (t)
					addIDForString(t);
			}
		}
	}
	if (elementsIndexed_ || attributesIndexed_ ||
	    updateStats_ || autoIndexSpecification_) {
		// Maintain the last descendant NID
		indexEntry_.setLastDescendant(
			(ninfo ? ninfo->getNodeID(): NsNid()));

		// only push state if there is indexing going on
		if (autoIndexSpecification_) {
			IndexerState *pis = getParentState(0);
			if (pis)
				pis->setNotLeaf();
		}
		IndexerState *eis = stateStack_.push();
		eis->startNode(*indexSpecification_, (const char*)uri, (const char*)localName);
		if ((attributesIndexed_||autoIndexSpecification_) && (attrCount != 0)) {
			for (int i = 0; i < attrCount; ++i) {
				IndexerState *ais = stateStack_.push();
				const char *uri = (const char *)attrs->uri(i);
				ais->startNode(*indexSpecification_, uri,
					       (const char*)attrs->localName(i), i);
				if (autoIndexSpecification_ &&
				    (!uri ||
				     !NsUtil::nsStringEqual((const xmlbyte_t *)uri,xmlnsUri))) {
					autoIndexSpecification_->
						enableAutoAttrIndexes(ais->getName());
				}

				if (ais->isIndexed(Index::NODE_ATTRIBUTE)) {
					const char *v =	(const char *)attrs->value(i);
					// do NOT add trailing null
					ais->characters(v, ::strlen(v));
				} else {
					// It's not indexed, so we might as well
					// not keep it around.
					stateStack_.pop();
				}
			}
		}
	}

	if(isEmpty)
		writeEndElementWithNode(localName, prefix, uri, ninfo);
}

void Indexer::writeText(XmlEventReader::XmlEventType type,
			const unsigned char *chars, size_t len)
{
	// entities are always expanded by this time
	writeTextWithEscape(type, chars, len, false);
}

/**
 * Note - Elements with multiple text node descendants are
 * concatenated into a single key.
 *
 * Eg <a>x<b>y</b>z</a> is a=xyz, b=y
 */
void Indexer::writeTextWithEscape(XmlEventReader::XmlEventType type,
				  const unsigned char *chars,
				  size_t len, bool needsEscape)
{
	if (elementsIndexed_ && len != 0) {
		if (type == XmlEventReader::Comment ||
		    type == XmlEventReader::Whitespace)
			return;
		DBXML_ASSERT(type == XmlEventReader::Characters ||
			     type == XmlEventReader::CDATA);
		//
		// *-element-equality-*
		// *-element-substring-*
		//
		unsigned int index = 0;
		for(IndexerState *is = stateStack_.top(index); is != 0;
		    is = stateStack_.top(++index)) {
			if(!is->isAttribute() && is->isIndexed(Index::NODE_ELEMENT) &&
			   (is->iv().isEnabled(Index::XEE, Index::NK_MASK) ||
			    is->iv().isEnabled(Index::XES, Index::NK_MASK))) {
				// Note that an element value can be split into
				// multiple calls to characterData. First time
				// past the key goes from Presence to Equality,
				// there after the data is just appended to
				// the Equality key.
				//
				// Note that we do not trim leading whitespace.
				// trailing null must NOT be added to len
				//
				is->characters((const char *)chars, len);
			}
		}
	}
}

IndexerState *Indexer::getParentState(int index)
{
	
	for(IndexerState *is = stateStack_.top(index); is != 0;
	    is = stateStack_.top(++index)) {
		if(!is->isAttribute()) {
			return is;
		}
	}

	return 0;
}

void Indexer::writeEndElementWithNode(const unsigned char *localName,
				      const unsigned char *prefix,
				      const unsigned char *uri,
				      IndexNodeInfo *ninfo)
{
	UNUSED(uri);
	UNUSED(localName);
	UNUSED(prefix);

	if (elementsIndexed_ || attributesIndexed_ ||
	    updateStats_ || autoIndexSpecification_) {
		
		indexEntry_.setIndexNodeInfo(ninfo);

		// Generate keys for this element and it's attributes
		IndexerState *cis;
		do {
			cis = stateStack_.top();
			DBXML_ASSERT(cis != 0);
			if (autoIndexSpecification_ && cis->isLeaf() &&
			    !cis->isAttribute()) {
				// if autoindexing on and current element
				// is a leaf, add indexes for it to the list
				autoIndexSpecification_->
					enableAutoElementIndexes(cis->getName());
			}
			if(cis->isAttribute()) {
				if(attributesIndexed_ && cis->isIndexed(Index::NODE_ATTRIBUTE)) {
					//
					// node-attribute-*
					//
					Key &k = cis->getKey(*container_, *oc_);
					indexEntry_.setIndex(cis->getAttrIndex());
					generateKeys(cis->iv(), Index::NA, Index::PN_MASK, k, *keyStash_);
					//
					// edge-attribute-*
					//
					k.setID2(getParentState()->getKey(*container_, *oc_).getID1());
					generateKeys(cis->iv(), Index::EA, Index::PN_MASK, k, *keyStash_);
				}
			} else {
				bool genKeys = (elementsIndexed_ && cis->isIndexed(Index::NODE_ELEMENT));
				if (genKeys || updateStats_) {
					// getKey can be expensive -- defer if possible
					Key &k = cis->getKey(*container_, *oc_);
					IndexerState *pis = getParentState();
					DBXML_ASSERT(pis);
					k.setID2(pis->getKey(*container_, *oc_).getID1());
					if (genKeys) {
						//
						// node-element-*-*
						//
						generateKeys(cis->iv(), Index::NE, Index::PN_MASK, k, *keyStash_);
						//
						// edge-element-*-*
						//
						generateKeys(cis->iv(), Index::EE, Index::PN_MASK, k, *keyStash_);
					}
					if (updateStats_) {
						// fullStats means "collect and propagate all stats."  It will
						// be true when doing 'normal' indexing (document insert/delete)
						// and also when indexing new or deleted content in partial updates.
						// It will be false ONLY during partial updates when indexing
						// portions of the document that have not changed.  So, when
						// false, stats on the *current* node are not relevant; however,
						// already-set stats are propagated up the anscestor chain.
						bool fullStats = collectFullStats(ninfo);
						// Get the size of the node
						size_t nodeSize = 0;
						// Node size is kept only for node containers
						if (fullStats && ninfo != 0 && container_->isNodeContainer()) {
							const NsFormat &fmt =
								NsFormat::getFormat(NS_PROTOCOL_VERSION);
							nodeSize = ninfo->getNodeDataSize();
							//nodeSize = fmt.marshalNodeData(node, 0, /*count*/true);
							nodeSize += fmt.marshalNodeKey(indexEntry_.getDocID(),
										       ninfo->getNodeID(),
										       0, /*count*/true);
						}
						
						// Store the node stats for this node
						StructuralStats *cstats = &cis->stats[0];
						if (fullStats) {
							cstats->numberOfNodes_ = 1;
							cstats->sumSize_ = nodeSize;
						}
						
						// Increment the descendant stats in the parent, this is done
						// whether or not fullStats is true but the specific information
						// propagated changes
						StructuralStats *pstats = 0;
						pstats = &pis->stats[0];
						if (container_->isNodeContainer()) {
							if (fullStats) {
								pstats->sumChildSize_ += nodeSize;
								pstats->sumDescendantSize_ +=
									nodeSize + cstats->sumDescendantSize_;
							} else {
								// still need to account for descendant size
								// but not *this* node's size
								pstats->sumDescendantSize_ +=
									cstats->sumDescendantSize_;
							}
						}
						
						if (fullStats) {
							pstats = &pis->stats[k.getID1()];
							pstats->sumNumberOfChildren_ += 1;
							pstats->sumNumberOfDescendants_ += 1;
						}
						
						// add/subtract stats in cache
						if (isDelete_)
							statsCache_.subtract(k.getID1(), cis->stats);
						else
							statsCache_.add(k.getID1(), cis->stats);
						
						// propagate the per-node #descendant numbers up the ancestor
						// chain
						for (map<NameID, StructuralStats>::iterator it =
							     cis->stats.begin();
						     it != cis->stats.end(); ++it) {
							if (it->first != 0) {
								pis->stats[it->first].
									sumNumberOfDescendants_ +=
									it->second.sumNumberOfDescendants_;
							}
						}
						cis->stats.clear();
					}
				}
			}
			stateStack_.pop();
		} while (cis->isAttribute());
	}
}

void Indexer::writeEndDocument()
{
	if(elementsIndexed_ || attributesIndexed_ || updateStats_) {
		IndexerState *cis = stateStack_.top();
		DBXML_ASSERT(cis != 0);

		if (updateStats_) {
			Key &k = cis->getKey(*container_, *oc_);

			// Store the node stats for this node
			StructuralStats *cstats = &cis->stats[0];
			if (collectFullStats(NULL)) {
				// use a constant for size rather than
				// calculating actual node size
				cstats->sumSize_ = 12; // TBD -- maybe find actual size
				cstats->numberOfNodes_ = 1;
			}
			if(isDelete_) statsCache_.subtract(k.getID1(), cis->stats);
			else statsCache_.add(k.getID1(), cis->stats);
		}

		stateStack_.pop();
	}
}

//
// end of EventWriter interface
//

void Indexer::generateKeys(const IndexVector &iv, Index::Type pnk,
	Index::Type mask, Key &key, KeyStash &stash)
{
	// Iterate over the index vector looking for indexes that match
	// the path-node-key provided (pnk). For each syntax create the
	// keys for that syntax. Note that the key passed through is
	// reused...
	//
	if(indexEntry_.hasIndexNodeInfo() && container_->nodesIndexed()) {
		if(pnk & Index::NODE_ELEMENT) {
			indexEntry_.setFormat(IndexEntry::DSEL_FORMAT);
		}
		else if(pnk & Index::NODE_ATTRIBUTE) {
			indexEntry_.setFormat(IndexEntry::ATTRIBUTE_FORMAT);
		}
		else {
			indexEntry_.setFormat(IndexEntry::D_FORMAT);
		}
	} else {
		indexEntry_.setFormat(IndexEntry::D_FORMAT);
	}

	KeyGenerator::Ptr kg;
	int i = 0;
	Index index;
	const Syntax *syntax = iv.getNextSyntax(i, pnk, mask, index);
	while (syntax != 0) {
		key.setIndex(index);
		if(index.getUnique() == Index::UNIQUE_ON && index.indexerAdd()) {
			checkUniqueConstraint(key);
		}
		// use the correct substring key generation algorithm
		// to handle old and new (2.4+) versions
		bool generateShortKeys = (container_->getIndexVersion() >=
					  SUBSTRING_3CHAR_VERSION);
		kg = syntax->getKeyGenerator(index, key.getValue(),
					     key.getValueSize(), generateShortKeys);
		const char *keyValue = 0;
		size_t keyLength = 0;
		while(kg->next(keyValue, keyLength)) {
			stash.addKey(key, keyValue, keyLength, indexEntry_);
		}

		syntax = iv.getNextSyntax(i, pnk, mask, index);
	}
}

void Indexer::checkUniqueConstraint(const Key &key)
{
	KeyGenerator::Ptr kg = key.getSyntax()->getKeyGenerator(key.getIndex(),
		key.getValue(), key.getValueSize(), /*forIndex*/false);

	SyntaxDatabase *database = container_->
		getIndexDB(key.getSyntaxType(), oc_->txn(), true);

	DbtIn dbt;
	const char *keyValue= 0;
	size_t keyLength= 0;
	while(kg->next(keyValue, keyLength)) {
		KeyStash::Entry *entry = uniqueKeysStash_.
			addUniqueKey(key, keyValue, keyLength);
		if(entry != 0) {
			entry->getKey(dbt);

			IndexEntry ie;
			int err = database->getIndexDB()->getIndexEntry(*oc_, dbt, ie);
			if(err != 0 && err != DB_NOTFOUND) throw XmlException(err);

			if(err == DB_NOTFOUND || ie.getDocID() == indexEntry_.getDocID())
				continue;
		}

		Key copy(key);
		copy.setValue(keyValue, keyLength);
		std::ostringstream oss;
		std::string keyStr = copy.asString_XML(*oc_, *container_);
		oss << "Uniqueness constraint violation for key: " <<
			keyStr;
		container_->log(Log::C_INDEXER, Log::L_INFO, oss);
		throw XmlException(XmlException::UNIQUE_ERROR, oss.str());
	}
}

void Indexer::initIndexContent(const IndexSpecification &indexSpecification,
	const DocID &did, NsPushEventSource *source, KeyStash &stash,
	bool updateStats, bool writeNsInfo, bool isDelete)
{
	indexSpecification_ = &indexSpecification;
	keyStash_ = &stash;
	if (writeNsInfo) {
		DBXML_ASSERT(container_);
		DBXML_ASSERT(!isDelete); // should not be set on deletions
		dict_ = container_->getDictionaryDatabase();
	}

	attributesIndexed_ = indexSpecification_->
		isIndexed(Index::NODE_ATTRIBUTE, Index::NODE_MASK);
	elementsIndexed_ = indexSpecification_->
		isIndexed(Index::NODE_ELEMENT, Index::NODE_MASK);
#ifdef DBXML_DONT_DELETE_STATS
	// only update if not deleting
	updateStats_ = (updateStats && !isDelete);
#else
	updateStats_ = updateStats;
#endif
	isDelete_ = isDelete;

	indexEntry_.setDocID(did);

	// auto-indexing.  Non-null autoIndexSpecification_ means
	// it is turned on
	if (indexSpecification_->getAutoIndexing() && !isDelete_) {
		DBXML_ASSERT(!autoIndexSpecification_);
		// cannot just use copy ctor here (TBD: look into
		// this for efficiency
		autoIndexSpecification_ = new IndexSpecification();
		container_->getConfigurationDB()->
			getIndexSpecification(oc_->txn(),*autoIndexSpecification_);
	}

	// allow source to be null.  In this case, its event handler
	// is set elsewhere.
	if (source)
		source->setEventWriter(this);
}

void
Indexer::addIDForString(const unsigned char *strng)
{
	DBXML_ASSERT(dict_);
	const char *s = (const char *)strng;
	size_t len = ::strlen(s);
	NameID id;
	int err = dict_->lookupIDFromStringName(*oc_, s, len, id, true);
	if (err != 0) {
		std::string error =  "Indexer: unable to add a URI or prefix string to dictionary: ";
		error += (std::string)s;
		throw XmlException(XmlException::DATABASE_ERROR,
				   error.c_str(),
				   __FILE__, __LINE__);
	}
}
