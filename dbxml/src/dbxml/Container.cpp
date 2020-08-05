//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlContainer.hpp"
#include "dbxml/XmlDocument.hpp"
#include "dbxml/XmlIndexSpecification.hpp"
#include "dbxml/XmlTransaction.hpp"
#include "dbxml/XmlContainerConfig.hpp"
#include "Container.hpp"
#include "SyntaxManager.hpp"
#include "Cursor.hpp"
#include "OperationContext.hpp"
#include "Results.hpp"
#include "Indexer.hpp"
#include "IndexLookup.hpp"
#include "UpdateContext.hpp"
#include "Name.hpp"
#include "Document.hpp"
#include "Value.hpp"
#include "QueryContext.hpp"
#include "dbxml/XmlManager.hpp"
#include "Manager.hpp"
#include "IndexEntry.hpp"
#include "Globals.hpp"
#include "nodeStore/NsDocumentDatabase.hpp"
#include "nodeStore/NsEvent.hpp"
#include "nodeStore/NsEventWriter.hpp"
#include <dbxml/XmlEventReaderToWriter.hpp>
#include "dataItem/DbXmlFactoryImpl.hpp"
#include "query/SequentialScanQP.hpp" // for iterators GMF TBD -- eliminate this

#include <math.h> // for ceil()

#define USE_DBXML_DISK 0 //change to 1 to used dbxml_disk

using namespace DbXml;
using namespace std;

// default to 8k pages for node storage, 16k for whole-doc
#define DBXML_NODE_PAGESIZE (8 * 1024)
#define DBXML_DOC_PAGESIZE (16 * 1024)

// Counters
#define INCR(ctr) Globals::incrementCounter(ctr)

// forward
static void doUpgrade(const std::string &name,
		      const std::string &tname, Manager &mgr,
		      unsigned int old_version, unsigned int current_version);
static void throwUnique(const XmlException &xe, const std::string name,
			bool isDuplicateMD);

namespace DbXml {

/*
 * Transaction::Notify instance to track index DBs created
 * on demand, during transactions.  Must be lock protected by
 * lock on container.
 */
class IndexDbNotify : public Transaction::Notify
{
public:
	IndexDbNotify(Container *cont) : cont_(cont) {}
	void add(int idx) { dbs_.push_back(idx); }
	virtual void postNotify(bool commit);
private:
	vector<int> dbs_;
	Container *cont_;
};

}

/*
 * This is the version of the container format
 * supported by this release.  If it does not
 * match that of an opened container, upgrade is required,
 * if supported
 * release 1.2.1 -- version 2
 * release 2.0.9 -- version 3
 * release 2.1.x -- version 4
 * release 2.2.x -- version 5
 * Upgrades supported:
 *  o version 3->[4,5]
 *  o version 4->5
 * Upgrades not supported:
 *  o version 2->*
 */

const unsigned int Container::version = CURRENT_VERSION;

static const string document_name("document");

static void checkSameContainer(const Container &cont,
			       const Document &doc)
{
	if (doc.getID() != DocID(0)) {
		int cid = doc.getContainerID();
		if (cid && (cid != cont.getContainerID())) {
			string msg = "Cannot perform operation on document from a different container.  The XmlDocument is from container ";
			msg += doc.getContainerName();
			msg += ", and this container is ";
			msg += cont.getName();
			throw XmlException(XmlException::INVALID_VALUE,
					   msg);
		}
	}
}

ScopedContainer::ScopedContainer(Manager &mgr, int id, bool mustExist)
	: c_(0)
{
	// this call gets a ref count on the object (acquire)
	c_ = mgr.getContainerFromID(id, true);
	if (!c_ & mustExist) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Attempt to reference a container that does not exist or has been closed");
	}
}


Container::Container(
	Manager &mgr, const std::string &name,
	Transaction *txn, const ContainerConfig &config, bool doVersionCheck)
	: ContainerBase(mgr, name),
	  xmlMgr_(mgr), // holds reference to object
	  indexNodes_(false),
	  indexVersion_(0),
	  doValidation_(true),
	  id_(-1),
	  usingTxns_((txn != 0) || (config.getTransactional())),
	  usingCDB_(false),
	  readOnly_(false),
	  indexDbNotify_(0),
	  containerConfig_(config),
	  compression_(0)
	  
{
	bool isTransactional = containerConfig_.getTransactional();
	containerConfig_.setTransactional(false);
	int err = 0;

	// We don't use the transaction passed in, because it screws up opening the
	// DbSequence, and can end up deadlocking because of the auto-child transaction
	// code.
	//
	// This means you can't rollback container creation, but it's the lesser of two
	// evils.
	txn = 0;

	TransactionGuard txnGuard;
	// do not check read-only status in this call
	txn = autoTransact(txn, txnGuard, false);
	
	err = openInternal(txn, containerConfig_, doVersionCheck);
	if(err == 0) txnGuard.commit();
	// explicitly handle DB_NOTFOUND and EEXIST
	if (err == EEXIST) {
		std::ostringstream s;
		s << "XmlContainer already exists: " << name;
		throw XmlException(XmlException::CONTAINER_EXISTS,
				   s.str().c_str());
	}
	if (err == DB_NOTFOUND) {
		std::ostringstream s;
		s << "XmlContainer not found: " << name;
		throw XmlException(XmlException::CONTAINER_NOT_FOUND, 
				   s.str().c_str());
	}
	if (err != 0) {
		throw XmlException(err);
	}
	containerConfig_.setContainerOwned(false);
	containerConfig_.setTransactional(isTransactional);
	containerConfig_.setContainerOwned(true);
}

void
Container::release()
{
	int newValue;
	// Musn't hold the mutex lock when this class is deleted
	{
		MutexLock lock(mutex_);
		newValue = --count_;
	}
	// If this is the last reference, and the manager closes it
	// without reactivation, it is safe to delete the object
	if(newValue == 0) {
		if (((Manager &)mgr_).openContainers_.closeContainer(this, 0))
			delete this;
	}
}

Container::~Container()
{
	if(isLogEnabled(C_CONTAINER, L_INFO)) {
		log(C_CONTAINER, L_INFO, "container closed (deleted)");
	}
}

int Container::openInternal(Transaction *txn, const ContainerConfig &config, bool doVersionCheck)
{
	containerConfig_ = config;

	// The default is NLS -> node indexes, DLS -> document indexes
	indexNodes_ = containerConfig_.getContainerType() == XmlContainer::NodeContainer;

	// The DBXML_INDEX_NODES and DBXML_NO_INDEX_NODES flags override the default
	if(containerConfig_.getIndexNodes() == XmlContainerConfig::On)
		indexNodes_ = true;
	if(containerConfig_.getIndexNodes() == XmlContainerConfig::Off)
		indexNodes_ = false;

	// The DBXML_STATISTICS and DBXML_NO_STATISTICS flags override the default
	bool useStats = true;
	if(containerConfig_.getStatistics() == XmlContainerConfig::Off)
		useStats = false;

	doValidation_ = containerConfig_.getAllowValidation();

	if (containerConfig_.getReadOnly())
		readOnly_ = true;
	
	// set default page sizes if creating, and not already set
	//
	// [#13781],[#13782]: DB does not currently ignore pagesize
	// for opening existing databases with the DB_CREATE flag.
	// This means, if the container exists, and DB_CREATE is
	// set, and pagesize should be left as the default.  Alternatively,
	// DB_CREATE could be stripped, but the result is the same.
	// TBD: remove this code when 13781 is fixed.
	bool containerBeingCreated = false;
	if (containerConfig_.getAllowCreate()) {
		// this check is the code to remove.  A non-zero return
		// means the container exists
		containerBeingCreated = checkContainer(name_.c_str(),
			mgr_.getDB_ENV()) == 0;
		if(!containerBeingCreated) containerConfig_.setPageSize(0);
		else if (containerConfig_.getPageSize() == 0) {
			if (containerConfig_.getContainerType() == XmlContainer::WholedocContainer)
				containerConfig_.setPageSize(DBXML_DOC_PAGESIZE);
			else
				containerConfig_.setPageSize(DBXML_NODE_PAGESIZE);
		}
	} else
		containerConfig_.setPageSize(0); // irrelevant if not creating

	// Create/Open Databases
	DB_ENV *env = mgr_.getDB_ENV();
	configuration_.reset(new ConfigurationDatabase(env, txn, name_,
						       containerConfig_, indexNodes_,
						       doVersionCheck));
	//create or verify the compression algorithm
 	std::string compName(containerConfig_.getCompressionName());
	if (containerConfig_.getContainerType() == XmlContainer::NodeContainer &&
		::strcmp(compName.c_str(), XmlContainerConfig::DEFAULT_COMPRESSION) != 0 &&
		::strcmp(compName.c_str(), XmlContainerConfig::NO_COMPRESSION) != 0) {
			throw XmlException(XmlException::INVALID_VALUE,
 				"Compression cannot be used with a container of type NodeContainer, the compression used is: " 
 				+ compName);
	}
#ifndef DBXML_COMPRESSION
	//set default compression to none if default compression is disabled
	if(::strcmp(compName.c_str(), XmlContainerConfig::DEFAULT_COMPRESSION) == 0) {
		compName = XmlContainerConfig::NO_COMPRESSION;
		containerConfig_.setCompressionName(XmlContainerConfig::NO_COMPRESSION);
	}
#endif
 	if(containerBeingCreated)
 		configuration_->putCompressionName(txn, containerConfig_.getCompressionName());
 	else if(name_.length() > 0) { //if database is not anonymous
		std::string name = configuration_->getCompressionName(txn);
		if(name.empty())
			compName = XmlContainerConfig::NO_COMPRESSION;
		else 
			compName = name;
		containerConfig_.setCompressionName(compName.c_str());
 	} else //use default compression for anonymous databases that are not being explicitly created
		compName = XmlContainerConfig::DEFAULT_COMPRESSION;
 	compression_ = mgr_.getCompression(compName);
 	if(!compression_ && ::strcmp(compName.c_str(), XmlContainerConfig::NO_COMPRESSION) != 0
		&& ::strcmp(compName.c_str(), XmlContainerConfig::DEFAULT_COMPRESSION) != 0){
 		throw XmlException(XmlException::INVALID_VALUE,
 		"Compression used by container not registered, the compression is: " 
 		+ compName);
 	}
	configuration_->getIndexVersion(txn, indexVersion_);

	dictionary_.reset(new DictionaryDatabase(env, txn, name_, containerConfig_));

	switch(containerConfig_.getContainerType()) {
	case XmlContainer::WholedocContainer: {
		documentDb_.reset(new DocumentDatabase(
					  env, txn, name_, containerConfig_, compression_));
		break;
	}
	case XmlContainer::NodeContainer: {
		documentDb_.reset(new NsDocumentDatabase(env, txn,
							 name_, containerConfig_, compression_));
		break;
	}
	default: DBXML_ASSERT(false);
	}

	openIndexDbs(txn, containerConfig_);

	// If the container is being created, create the structural statistics
	// database unless the DBXML_NO_STATISTICS flag is specified.
	//
	// If the container isn't being created, try to open the structural statistics
	// database, but never create it.
	if(!containerBeingCreated || useStats) {
		ContainerConfig sConfig(containerConfig_);
		if(!containerBeingCreated) {
			sConfig.setAllowCreate(false); // don't allow creation
			sConfig.setExclusiveCreate(false);
		}

		try {
			stats_.reset(new StructuralStatsDatabase(env, txn, name_,
					     sConfig, usingCDB_));
		} catch (XmlException &xe) {
			// ENOENT is not an error
			if (xe.getDbErrno() != ENOENT)
				throw;
		}
		useStats = (bool)stats_;
	}

	// Set pageSize_ to the correct value, based on existing databases.
	// This works whether or not they were just created.
	containerConfig_.setPageSize(documentDb_->getPageSize());
	// Log the opening
	if(isLogEnabled(C_CONTAINER, L_INFO)) {
		ostringstream oss;
		switch(containerConfig_.getContainerType()) {
		case XmlContainer::WholedocContainer: {
			oss << "Document";
			break;
		}
		case XmlContainer::NodeContainer: {
			oss << "Node";
			break;
		}
		default: DBXML_ASSERT(false);
		}
		oss << " storage container opened.";
		log(C_CONTAINER, L_INFO, oss);
	}

	//Set the correct values in containerConfig_
	if(indexNodes_)
		containerConfig_.setIndexNodes(XmlContainerConfig::On);
	else
		containerConfig_.setIndexNodes(XmlContainerConfig::Off);

	if(useStats)
		containerConfig_.setStatistics(XmlContainerConfig::On);
	else
		containerConfig_.setStatistics(XmlContainerConfig::Off);
	containerConfig_.setCompressionName(compName.c_str());

	containerConfig_.setContainerOwned(true);
	return 0;
}

void Container::close()
{
	((Manager &)mgr_).openContainers_.closeContainer(this, 0);
	configuration_.reset(0);
	dictionary_.reset(0);
	documentDb_.reset(0);
	closeIndexes();
	stats_.reset(0);
	if(isLogEnabled(C_CONTAINER, L_INFO)) {
		log(C_CONTAINER, L_INFO, "container closed");
	}

}

void Container::closeIndexes(int idx)
{
	if (idx == -1) {
		int j = 0;
		const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
		while (syntax != 0) {
			indexes_[syntax->getType()].reset(0);
			syntax = SyntaxManager::getInstance()->getNextSyntax(j);
		}
	} else {
		indexes_[idx].reset(0);
	}
}

bool Container::addAlias(const std::string &alias)
{
	if ((alias.find('/') != std::string::npos) || (alias.find('\\') != std::string::npos)) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"XmlContainer::addAlias: aliases cannot include a path separator character ('/' or '\\')");
	}
	bool retval = ((Manager &)mgr_).openContainers_.addAlias(alias, this);
	return retval;
}

bool Container::removeAlias(const std::string &alias)
{
	return ((Manager &)mgr_).openContainers_.removeAlias(alias, this);
}

void Container::sync()
{
	configuration_->sync();
	dictionary_->sync();
	documentDb_->sync();
	int j = 0;
	const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	while (syntax != 0) {
		SyntaxDatabase *sdb = indexes_[syntax->getType()].get();
		if (sdb)
			sdb->sync();
		syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	}
	if(stats_) stats_->sync();
}

int Container::setIndexSpecification(
	Transaction *txn,
	const XmlIndexSpecification &index, UpdateContext &context)
{
	TransactionGuard txnGuard;
	txn = autoTransact(txn, txnGuard);

	int err = setIndexSpecificationInternal(txn, index, context);
	if(err == 0) txnGuard.commit();

	return err;
}

int Container::setIndexSpecificationInternal(Transaction *txn,
					     const IndexSpecification &newis,
					     UpdateContext &context)
{
	int err = 0;
	if (newis.isAutoIndexModified()) {
		 err = IndexSpecification::setAutoIndex(
			 getConfigurationDB(), txn,
			 newis.getAutoIndexing());
		 if (err != 0)
			 return err;
	}
	// if index spec itself is not modified, nothing to do
	if (!newis.isSpecModified())
		return 0;

	//
	// Get the current indexing specification for comparison
	//
	IndexSpecification oldis;
	err = configuration_->getIndexSpecification(txn, oldis, /*lock=*/true);
	if (err == 0) {
		// The indexing strategies to add
		//
		IndexSpecification addis(newis);
		bool removedDefault = addis.disableIndex(oldis);
		//
		// The indexing strategies to delete
		//
		IndexSpecification delis(oldis);
		bool addedDefault = delis.disableIndex(newis);

		// remove deleted strategies
		if (addedDefault || delis.isIndexed(Index::NONE, Index::NONE)) {
			err = removeIndexes(txn, delis, context);
		}
		// reindex with new strategies (will walk the entire
		// container)
		if (err == 0 &&
		    (removedDefault ||addis.isIndexed(Index::NONE, Index::NONE))) {
			// if a default index was removed, use the new spec without
			// any disabling for removal; this handles the case where
			// a removed default index masked a non-default index
			// [#15412].  This is heavy-weight, but alternatives
			// mean more complex IndexSpecification logic.
			if (removedDefault)
				err = reindex(txn, newis, /*updateStats*/false);
			else
				err = reindex(txn, addis, /*updateStats*/false);
		}
		if (err == 0) {
			err = configuration_->
				putIndexSpecification(txn, newis);
		}
		if (err == 0) {
			string uri, name, index;
			XmlIndexSpecification::Type stype;
			XmlValue::Type vtype;
			// create index databases for new
			// types.  Safer/more concurrent to
			// do this now, rather than when adding
			// initial entries.
			// TBD: is there a better way to get Syntax::Type?
			IndexSpecificationIterator i0(addis);
			while (i0.next(uri, name, stype, vtype)) {
				Syntax::Type synType =
					AtomicTypeValue::convertToSyntaxType(
						vtype);
				getIndexDB(synType, txn, true);
			}
			//
			// Log the indexing changes.
			//

			IndexSpecificationIterator i1(addis);
			while (i1.next(uri, name, index)) {
				ostringstream oss;
				oss << "Add '" << index <<
					"' index for node '" << uri <<
					"," << name << "'";
				log(C_CONTAINER, L_INFO, oss);
			}
			IndexSpecificationIterator i2(delis);
			while (i2.next(uri, name, index)) {
				ostringstream oss;
				oss << "Delete '" << index <<
					"' index for node '" << uri <<
					"," << name << "'";
				log(C_CONTAINER, L_INFO, oss);
			}
		}
	}
	return err;
}

Results *Container::lookupIndex(Transaction *txn,
				XmlQueryContext &context,
				const IndexLookup &il,
				u_int32_t flags)
{
	checkFlags(Log::misc_flag_info, "lookupIndex()", flags,
		   DB_READ_UNCOMMITTED|DB_RMW|DB_READ_COMMITTED|DBXML_LAZY_DOCS|
		   DBXML_REVERSE_ORDER|DBXML_INDEX_VALUES|DBXML_NO_INDEX_NODES|
		   DBXML_CACHE_DOCUMENTS|DB_TXN_SNAPSHOT|DBXML_WELL_FORMED_ONLY);
	Index index;
	if(!index.set(il.getIndex())) {
		throw XmlException(
			XmlException::UNKNOWN_INDEX,
			"Unknown index specification, '" +
			(std::string)il.getIndex() + ".");
	}
	if(index.getKey() == Index::KEY_SUBSTRING) {
		throw XmlException(
			XmlException::UNKNOWN_INDEX,
			"Index lookup on a substring index is not available.");
	}
	if((index.getPath() == Index::PATH_NODE) && il.hasParent()) {
		// Deal with an edge lookup on a node
		// index as a special case
		return new ValueResults(getXmlManager());
	}
		
	if(context.getEvaluationType() ==
	   XmlQueryContext::Eager) {
		return new ValueResults(
			new LazyIndexResults(
				*this, context, txn, index, il, flags),
			getXmlManager());
	} else {
		return new LazyIndexResults(
			*this, context, txn, index, il, flags);
	}
}

Statistics *Container::lookupStatistics(Transaction *txn,
					Name *child,
					Name *parent,
					const std::string &indexString,
					const XmlValue &value)
{
	std::string pUri = (parent ? parent->getURIName() : "");
	const char *p_uri = pUri.length() ? pUri.c_str() : 0;
	Index index_obj;
	if(!index_obj.set(indexString)) {
		throw XmlException(
			XmlException::UNKNOWN_INDEX,
			"Unknown index specification, '" +
			(std::string)indexString + ".");
	}
	KeyStatistics stats = getKeyStatistics(
		txn, index_obj,
		child->getURIName().c_str(),
		p_uri, value);
	return new Statistics(stats, index_obj, value);
}

KeyStatistics Container::getKeyStatistics(Transaction *txn, const Key &key)
{

	DbtOut tkey, data;
	
	SyntaxDatabase *database = const_cast<SyntaxDatabase*>(
		getIndexDB(key.getSyntaxType()));

	if (!database)
		return KeyStatistics();
	
	key.setDbtFromThis(tkey);
	size_t structureLength = Key::structureKeyLength(key.getIndex(),
						      tkey);
	if(structureLength < tkey.size)
		tkey.set(0, structureLength); // trim the value off

	KeyStatistics tmpStats;
	KeyStatistics result;

	// Behave like a prefix cursor, adding all the statistics with a key
	// whose prefix matches our key
	int found = 0;
	Cursor cursor(*database->getStatisticsDB(), txn, CURSOR_READ);
	int err = cursor.error();
	if(err == 0) {
		DbtOut original;
		original.set(tkey.data, tkey.size);

		err = cursor.get(tkey, data, DB_SET_RANGE);
		while(err == 0) {
			if(tkey.size < original.size ||
			   memcmp(original.data, tkey.data,
				  original.size) != 0) {
				// We've reached the end, so set a flag saying so
				err = DB_NOTFOUND;
			} else {
				++found;
				tmpStats.setThisFromDbt(data);

				// Fix the unique keys value, if necessary
				if(tmpStats.numUniqueKeys_ == 0 &&
				   tmpStats.numIndexedKeys_ != 0) {
					tmpStats.numUniqueKeys_ = 1;
				}

				// add the value it to the results
				result.add(tmpStats);

				// Get the next key/data pair
				tkey.set(original.data, original.size);
				err = cursor.get(tkey, data, DB_NEXT);
			}
		}
		if(err == DB_NOTFOUND || err == DB_KEYEMPTY)
			err = 0;
	}
	if (err)
		throw XmlException(err);
	return result;
}

KeyStatistics Container::getKeyStatistics(Transaction *txn, const Index &index,
					  const char *child, const char *parent,
					  const XmlValue &value)
{
	OperationContext oc(txn);

	Key key(((Manager&)getManager()).getImplicitTimezone());
	key.setIndex(index);

	if(!value.isNull() && AtomicTypeValue::convertToSyntaxType(value.getType()) !=
	   key.getSyntaxType()) {
		throw XmlException(XmlException::INVALID_VALUE, "Value type does not match index syntax type.");
	}
	if(!value.isNull() && key.getSyntaxType() == Syntax::NONE) {
		throw XmlException(XmlException::INVALID_VALUE, "A value has been specified for an index that does not require one.");
	}

	key.setIDsFromNames(oc, *this, parent, child);
	if(index.getPath() == Index::PATH_EDGE && parent == 0) {
		key.setNodeLookup(true);
	}

	if(!value.isNull()) {
		key.setValue(value);
	}

	return  getKeyStatistics(txn, key);
}

int Container::reindex(Transaction *txn, const IndexSpecification &is,
		       bool updateStats)
{
	// create new, local update context for this operation.  This
	// call can be made from within putDocument() and other
	// operations that may have access to the UpdateContext parameter
	UpdateContext context(getXmlManager());
	context.init(txn, this);
	Indexer &indexer = context.getIndexer();
	KeyStash &stash = context.getKeyStash();
	OperationContext &oc = context.getOperationContext();
	indexer.resetContext(this, &oc);
	const Manager &mgr = (Manager&)getManager();
	// Use DB_READ_COMMITTED locking to avoid lots of locks/lockers
	// during reindex
	u_int32_t flags = 0;
	if (mgr.isTransactedEnv() ||  mgr.isLockingEnv()) {
		flags = DB_READ_COMMITTED;
	}

	ScopedPtr<DocumentCursor> cursor;
	int err = documentDb_->createDocumentCursor(txn, cursor, flags);
	if(err == 0) {
		XmlDocument document;
		DocID id;
		err = cursor->first(id);
		while(id != 0) {
			indexer.reset();
			stash.reset();

			if ((err = getDocument(oc, id, document, flags)) == 0) {
				documentDb_->reindex(is, document, context, stash, updateStats);
				err = stash.updateIndex(oc, this);
				if(err == 0 && stats_ && updateStats) {
					// Update the structural statistics
					err = stats_->addStats(oc, indexer.getStatsCache());
					indexer.resetStats();
				}
			}
			if(err == 0) err = cursor->next(id);
			else break;
		}

		//displayStructuralStats(oc, cerr);
	}

	if(err == DB_NOTFOUND)
		err = 0;

	return err;
}

int Container::removeIndexes(Transaction *txn, const IndexSpecification &is,
			     UpdateContext &uc)
{
	int err = 0;
	UpdateContext context(getXmlManager());
	context.init(txn, this);
	OperationContext &oc = context.getOperationContext();
	const IndexVector *iv = is.getDefaultIndex();
	err = removeIndexes(oc, *iv, context, true);
	
	IndexSpecification::const_iterator current = is.begin();
	IndexSpecification::const_iterator end = is.end();
	while (current != end) {
		iv = current->second;
		err = removeIndexes(oc, *iv, context, false);
		current++;
	}
	return err;
}

int Container::removeIndexes(OperationContext &oc, const IndexVector &iv,
			     UpdateContext &context, bool isDefault)
{
	int err = 0;
	NameID id;
	if (!isDefault) {
		err = getDictionaryDatabase()->lookupIDFromName(
			oc, iv.getNameObject(), id, false);
		if(err == DB_NOTFOUND) {
			// DB_NOTFOUND means there's no data in the index yet
			return 0;
		}
		else if (err != 0) return err;
	}
	IndexVector::const_iterator i;
	for (i = iv.begin(); i != iv.end(); ++i) {
		SyntaxDatabase *sdb = getIndexDB(i->getSyntax(),
						 oc.txn(), false);
		if (sdb) {
			err = sdb->removeIndex(oc, *i, id);
			if(err == DB_NOTFOUND) {
				// DB_NOTFOUND just means there's no data in the index
				err = 0;
			}
			else if(err != 0) break;
		}
	}

	return err;
}

//
// add a document as an XmlEventWriter.  Return an object to use.
// caller *must* call XmlEventWriter::close() to not only destroy
// the object, but to perform final cleanup of state.
// This is true whether or not an exception is thrown during processing.
//
NsEventWriter &Container::addDocumentAsEventWriter(
	Transaction *txn, Document &document,
	UpdateContext &context,
	u_int32_t flags)
{
	// Because it's a 2-part mechanism, autocommit (and CDB)
	// won't work, so they need an explicit transaction
	if (txn == 0 && (usingTxns_||usingCDB_)) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlContainer::addDocumentAsEventWriter requires an explicit transaction");
	}
	// do not return events object, we'll create it here
	(void)prepareAddDocument(txn, document, context, flags, false);

	NsDocumentDatabase *ndd = documentDb_->getNsDocumentDatabase();
	NsEventWriter *writer =
		new NsEventWriter(this, &context, &document, flags);
	writer->setBaseEventWriter(&(context.getIndexer()));
	if (!ndd)
		writer->createNsWriter(); // write wholedoc content, as well
	return *writer;
}

int Container::addDocumentAsEventReader(Transaction *txn, Document &document,
					UpdateContext &context, u_int32_t flags)
{
	DBXML_ASSERT(document.getDefinitiveContent() == Document::READER);
	// NOTE: this code is virtually identical to above, in ...AsEventWriter
	// except for the end
	
	// do not return events object, we'll create it here
	(void)prepareAddDocument(txn, document, context, flags, false);

	NsDocumentDatabase *ndd = documentDb_->getNsDocumentDatabase();
	NsEventWriter *writer =
		new NsEventWriter(this, &context, &document, flags);
	writer->setBaseEventWriter(&(context.getIndexer()));
	if (!ndd)
		writer->createNsWriter(); // write wholedoc content, as well
	XmlEventReaderToWriter r2w(document.getContentAsEventReader(),
				   *writer);
	// reader is a one-time use only; it's now owned by the r2w object
	DBXML_ASSERT(document.getDefinitiveContent() == Document::NONE);
	r2w.start();
	return 0;
}

int Container::addDocument(Transaction *txn, Document &document,
				     UpdateContext &context, u_int32_t flags)
{
	TransactionGuard txnGuard;
	txn = autoTransact(txn, txnGuard);

	int err = addDocumentInternal(txn, document, context, flags);
	if(err == 0) txnGuard.commit();

	return err;
}

int Container::addDocumentInternal(Transaction *txn, Document &document,
				   UpdateContext &context, u_int32_t flags)
{
	if(isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(document, "Adding document: ");
	
	if (document.getDefinitiveContent() == Document::READER)
		return addDocumentAsEventReader(txn, document, context, flags);
	NsPushEventSource *events =
		prepareAddDocument(txn, document, context, flags);
	int err = 0;
	if (events) {
		// events is donated to this call, which deletes it
		err = indexAddDocument(events, document, context);
	}
	if (!err)
		err = completeAddDocument(document, context);
	if(!err && isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(document, "Added document successfully: ");
	return err;
}

// part 1 of adding a document: generate id (and optionally name),
// index metadata, and prepare for indexing content.
// Most intermediate state is maintained in UpdateContext
NsPushEventSource *Container::prepareAddDocument(Transaction *txn,
						 Document &document,
						 UpdateContext &context,
						 u_int32_t flags,
						 bool createEvents)
{
	// Count putDocument here
	INCR(Counters::num_putdoc);
	if (document.getContainerID() && document.getDefinitiveContent() == Document::NONE)
		document.setEagerContentAndMetaData();
	else
		document.setEagerMetaData(); // Just in case
	document.setContainer(this); // also, just in case
	if ((flags & DBXML_WELL_FORMED_ONLY) != 0)
		document.setValidation(Document::WF_ONLY);
	else if (doValidation_)
		document.setValidation(Document::VALIDATE);

	context.init(txn, this);
	OperationContext &oc = context.getOperationContext();
	(void) ensureDocName(oc, document, flags); // throws on error

	KeyStash &stash = context.getKeyStash();
	Indexer &indexer = context.getIndexer();
	NsPushEventSource *events = 0;
	try {
		// index metadata
		indexer.indexMetaData(context.getIndexSpecification(),
				      document, stash, false /*checkModified*/);

		if (createEvents) {
			// If using wholedoc storage, and document content is a
			// stream, convert it to a dbt -- streams can only
			// be used once...
			NsDocumentDatabase *ndd = documentDb_->
				getNsDocumentDatabase();
			if (!ndd && (document.getDefinitiveContent() ==
				     Document::INPUTSTREAM))
				document.getContentAsDbt();


			// create event source, and initialize indexer, but
			// only if there is content to index
			if (document.getDefinitiveContent() != Document::NONE)
				events = document.getContentAsEventSource(
					oc.txn(),
					/*needsValidation*/true,
					nodesIndexed());
		}
		// it's ok for events to be null
		indexer.initIndexContent(
			context.getIndexSpecification(),
			document.getID(),
			events, stash, stats_,
			(containerConfig_.getContainerType() == XmlContainer::WholedocContainer),
			/*isDelete*/false);
	}
	catch (XmlException &xe) {
		if (xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			throwUnique(xe, document.getName(), true);
		} else
			throw; // re-throw
	}
	
	return events;
}

// Perform the actual indexing step.  This is where these errors are caught:
// o parse errors
// o unique index errors on content
// o database errors
//
// Call the underlying DocumentDatabase instances on error and success to
// allow for cleanup.
//
int Container::indexAddDocument(NsPushEventSource *events, Document &document,
				UpdateContext &context)
{
	try {
		events->start(); // run the indexer
	}
	catch (XmlException &xe) {
		delete events;
		bool isDbExc = (xe.getExceptionCode() ==
				XmlException::DATABASE_ERROR);
		documentDb_->addContentException(document, context, isDbExc);
		if (xe.getExceptionCode() == XmlException::UNIQUE_ERROR) {
			throwUnique(xe, document.getName(), false);
		} else if (xe.getExceptionCode() ==
			   XmlException::INDEXER_PARSER_ERROR) {
			// append doc name to message.  Cannot modify the
			// XmlException, so throw a new one
			std::string msg = xe.what();
			msg += " (Document: " + document.getName() + ")";
			throw XmlException(XmlException::INDEXER_PARSER_ERROR,
					   msg);
		} else
			throw; // re-throw
	}
	catch (...) {
		delete events;
		documentDb_->addContentException(document, context, false);
		throw;
	}
	delete events;
	// success, finish adding content (if there's anything left)
	int err = documentDb_->addContent(document, context);
	if (err == 0)
		document.setContentModified(false);
	return 0;
}

int Container::completeAddDocument(Document &document, UpdateContext &context)
{
	// indexing is done
	// write metadata, data, and index keys
	OperationContext &oc = context.getOperationContext();
	
	int err = documentDb_->addMetaData(oc, getDictionaryDatabase(), document);
	if(!err) {
		// Write the index keys
		err = context.getKeyStash(false).updateIndex(oc, this);
		if(err == 0 && stats_) {
			// Update the structural statistics
			err = stats_->addStats(oc, context.getIndexer().getStatsCache());
			context.getIndexer().resetStats();
		}
		if (!err) {
			document.setMetaDataFromContainer(
				this,
				document.getID(),
				oc.txn(), 0 /* no flags */);
		}
	}
	if (!err) {
		IndexSpecification *autoIs =
			context.getIndexer().getAutoIndexSpecification();
		if (autoIs)
			err = doAutoIndex(autoIs,
					  context.getIndexSpecification(), oc);
	}
	return err;
}

// auto-indexing
int Container::doAutoIndex(IndexSpecification *addis,
			   IndexSpecification &is,
			   OperationContext &oc)
{
	DBXML_ASSERT(addis);
	int err = 0;
	// This is similar to part of setIndexSpecificaton -- 
	// "subtract" the existing indexes from the to-be-added
	// ones and if there are any left, add them
	// Get the record value *before* subtracting the current IS
	Buffer buf;
	addis->writeToBuffer(buf);
	// IS must be set to ADD -- we never delete indexes in this
	// path and if it is set to delete the "subtraction" operation
	// will not work properly.  This means that the IS state is
	// changed and should not be unconditionally re-used after this call
	is.set(Index::INDEXER_ADD);

	addis->disableIndex(is);
	if (addis->isIndexed(Index::NONE, Index::NONE)) {
		err = reindex(oc.txn(), *addis,
			      false /*updateStats*/);
		if (err == 0)
			err = addis->write(configuration_.get(),
					   oc.txn(), &buf);
	}
	return err;
}

// generates ID and makes sure that the document has a name
int Container::ensureDocName(OperationContext &context, Document &document,
			     u_int32_t flags) const
{
	string name = document.getName();
	DocID &id = document.getIDToSet();
	id = 0;
	int err = configuration_->generateID(context.txn(), id);
	if (err)
		throw XmlException(XmlException::INVALID_VALUE,
				   "Unable to generated a document ID");
	if ((flags & DBXML_GEN_NAME) != 0) {
		static const char *genPrefix = "dbxml_";
		if (name == (std::string)"")
			name = genPrefix;
		else
			name += "_";
		name += id.asString();
		document.setName(name, /*modified*/true);
	} else {
		// name must be set if DBXML_GEN_NAME is not used
		const char *tname = name.c_str();
		if (tname && (*tname == 0))
			throw XmlException(XmlException::INVALID_VALUE,
					   "A valid name, or specification of DBXML_GEN_NAME, is required to put a XmlDocument into a container");
	}
	return 0;
}

int Container::getDocumentID(OperationContext &context,
			     const std::string &name, DocID &id) const
{
	// Using built in index "unique-metadata-equality-string" on dbxml:name
	Key key(0);
	key.getIndex().set(Index::NME | Syntax::STRING, Index::PNKS_MASK);
	key.setID1(dictionary_->getNIDForName());
	key.setValue(name.c_str(), name.length());

	const SyntaxDatabase *database = getIndexDB(Syntax::STRING);

	IndexEntry ie;
	key.setDbtFromThis(context.key());
	int err = database->getIndexDB()->getIndexEntry(context, context.key(), ie);
	if(err == 0) {
		id = ie.getDocID();
	}
	return err;
}

int Container::getDocument(OperationContext &context,
			   const std::string &name,
			   XmlDocument &document, u_int32_t flags) const
{
	DocID id;
	int err = getDocumentID(context, name, id);
	if(err != 0) return err;

	err = getDocument(context, id, document, flags);
	if (err == 0)
		((Document&)document).setName(name, /*modified*/false);

	return err;
}

int Container::getDocument(OperationContext &context, const DocID &did,
			   XmlDocument &document, u_int32_t flags) const
{
	// Count getDocument here
	INCR(Counters::num_getdoc);

	document = const_cast<Manager&>(mgr_).createDocument();
	bool wfonly = false;
	if ((flags & DBXML_WELL_FORMED_ONLY) != 0) {
		wfonly = true;
		flags &= ~DBXML_WELL_FORMED_ONLY;
	}
	((Document*)document)->setMetaDataFromContainer((Container *)this, did, context.txn(), flags);
	if (wfonly)
		((Document*)document)->setValidation(Document::WF_ONLY);
	return 0;
}

int Container::deleteDocument(Transaction *txn,
			      const std::string &name,
			      UpdateContext &context)
{
	TransactionGuard txnGuard;
	txn = autoTransact(txn, txnGuard);

	XmlDocument document;
	OperationContext &oc = context.getOperationContext();
	oc.set(txn);
	// Add DB_RMW if transacted to reduce deadlocks
	int err = getDocument(oc, name, document,
			      ((txn && !usingCDB_) ? DB_RMW : 0));
	if(err == 0)
		err = deleteDocumentInternal(txn, document, context);
	if(err == 0) txnGuard.commit();
	return err;
}

int Container::deleteDocument(Transaction *txn, Document &document,
			      UpdateContext &context)
{
	// handle Document object with name only
	if (document.getID() == 0)
		return deleteDocument(txn, document.getName(), context);
	TransactionGuard txnGuard;
	txn = autoTransact(txn, txnGuard);

	int err = deleteDocumentInternal(txn, document, context);
	if(err == 0) txnGuard.commit();

	return err;
}

int Container::deleteDocumentInternal(Transaction *txn, Document &document,
				      UpdateContext &context)
{
	checkSameContainer(*this, document);
	if (document.getDefinitiveContent() != Document::NONE) {
		// replicate code from above to avoid recursive
		// auto-transact 
		XmlDocument xdocument;
		OperationContext &oc = context.getOperationContext();
		oc.set(txn);
		int err = getDocument(oc, document.getName(),
				      xdocument, DBXML_LAZY_DOCS);
		// recursive call
		if(err == 0)
			err = deleteDocumentInternal(txn, xdocument, context);
		return err;
	}
	if(isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(document, "Deleting document: ");

	context.init(txn, this);
	OperationContext &oc = context.getOperationContext();
	KeyStash &stash = context.getKeyStash();
	DocID id = document.getID();

	//
	// We may have been given an empty document, or a document that has
	// already been deleted, or a document with a mangled ID. db->del
	// returns EINVAL if it can't delete a key/data pair. We change this
	// to DB_NOTFOUND, which makes more sense.
	//
	int err = 0;
	// Index the (possibly lazy) document and remove it's content
	err = documentDb_->removeContentAndIndex(document, context,
						 stash, stats_);
	
	if (err == EINVAL)
		err = DB_NOTFOUND;
	if(err != 0) return err;

	// Delete meta-data items from the document secondary database.
	err = documentDb_->removeMetaData(oc, id, 0);
	if(err != 0) return err;

	// Remove the document's index entries
	err = stash.updateIndex(oc, this);
	if(err == 0 && stats_) {
		// Update the structural statistics
		err = stats_->addStats(oc, context.getIndexer().getStatsCache());
		context.getIndexer().resetStats();
	}
	if(!err && isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(document, "Deleted document successfully: ");
	return err;
}

int Container::updateDocument(Transaction *txn, Document &document,
			      UpdateContext &context)
{
	TransactionGuard txnGuard;
	txn = autoTransact(txn, txnGuard);

	int err = updateDocumentInternal(txn, document, context, true);
	if(err == 0) txnGuard.commit();

	return err;
}

int Container::updateDocumentInternal(Transaction *txn, Document &new_document,
				      UpdateContext &context, bool validate)
{
	// Count updateDocument here
	INCR(Counters::num_updatedoc);

	if (new_document.getDefinitiveContent() == Document::READER) {
		// do not (yet) support updateDocument using XmlEventReader
		// content.  More code reorganization is necessary to do this.
		throw XmlException(XmlException::INVALID_VALUE,
				   "updateDocument is not supported for XmlEventReader content");
	}
	if(isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(new_document, "Updating document: ");
	
	checkSameContainer(*this, new_document);
	context.init(txn, this);
	new_document.setContainer(this); // just in case
	OperationContext &oc = context.getOperationContext();
	KeyStash &stash = context.getKeyStash();

	// Generate index keys for the new document and update the content
	int err = documentDb_->updateContentAndIndex(new_document,
						     context, stash, validate,
						     stats_);
	if(err != 0) return err;

	// Update the metadata
	err = documentDb_->updateMetaData(oc, getDictionaryDatabase(),
					  new_document);
	if(err != 0) return err;

	// Make the index changes necessary
	err = stash.updateIndex(oc, this);
	if(err == 0 && stats_) {
		// Update the structural statistics
		err = stats_->addStats(oc, context.getIndexer().getStatsCache());
		context.getIndexer().resetStats();
	}
	if(!err && isLogEnabled(C_CONTAINER, L_INFO))
		logDocumentOperation(new_document, "Updated document successfully: ");
	if (!err) {
		IndexSpecification *autoIs =
			context.getIndexer().getAutoIndexSpecification();
		if (autoIs)
			err = doAutoIndex(autoIs,
					  context.getIndexSpecification(), oc);
	}

	return err;
}

void Container::logDocumentOperation(const Document &doc, const char *msg)
{
	string logmsg = msg;
	if (doc.getName() != (string)"")
		logmsg += doc.getName();
	else
		logmsg += "(no name)";
	log(C_CONTAINER, L_INFO, logmsg);
}

void Container::dump(Manager &mgr, const std::string &name,
		     std::ostream *out)
{
	int err = 0;
	err = ConfigurationDatabase::dump(mgr.getDB_ENV(),
					  name, out);
	if (err == 0)
		err = DictionaryDatabase::dump(mgr.getDB_ENV(),
					       name, out);
	if (err == 0) {
		switch(ConfigurationDatabase::readContainerType(
			       mgr.getDB_ENV(), name)) {
		case XmlContainer::WholedocContainer: {
			err = DocumentDatabase::dump(
				mgr.getDB_ENV(), name,
				XmlContainer::WholedocContainer,
				out);
			break;
		}
		case XmlContainer::NodeContainer: {
			err = NsDocumentDatabase::dump(mgr.getDB_ENV(),
						       name, out);
			break;
		}
		default: DBXML_ASSERT(false);
		}
	}
	if (err) {
		throw XmlException(err);
	} else {
		Log::log(mgr.getDB_ENV(), C_CONTAINER, L_DEBUG,
			 name.c_str(), "Container dumped");
	}
}

void Container::load(Manager &mgr, const std::string &name,
		     std::istream *in, unsigned long *lineno,
		     UpdateContext &context)
{
	int ret = 0;
	XmlContainer::ContainerType ctype;
	ret = ConfigurationDatabase::load(mgr.getDB_ENV(), name,
					  in, lineno);
	if (ret == 0)
		ret = DictionaryDatabase::load(mgr.getDB_ENV(), name,
					       in, lineno);
	if (ret == 0) {
		ctype = ConfigurationDatabase::readContainerType(
			mgr.getDB_ENV(), name);
		switch(ctype) {
		case XmlContainer::WholedocContainer: {
			ret = DocumentDatabase::load(
				mgr.getDB_ENV(), name,
				XmlContainer::WholedocContainer,
				in, lineno);
			break;
		}
		case XmlContainer::NodeContainer: {
			ret = NsDocumentDatabase::load(mgr.getDB_ENV(), name,
						       in, lineno);
			break;
		}
		default: DBXML_ASSERT(false);
		}
	}

	if (ret == 0) {
		ContainerConfig config;
		config.setAllowCreate(true);
		config.setContainerType(ctype);
		config.setMode(0666);
		XmlContainer container = 
			mgr.openContainer(name, 0, config, true);
		((Container&)container).reloadIndexes(0, context, DEFAULT_CONFIG);
	}
	if (ret) {
		throw XmlException(ret);
	} else {
		Log::log(mgr.getDB_ENV(), C_CONTAINER, L_DEBUG,
			 name.c_str(), "Container loaded");
	}
}

//
// Verify is really 2 interfaces:
// 1. verify
// 2. salvage
// The verify behavior needs 2 passes on the databases in a container:
//   1.  for each database, verify it using DB_NOORDERCHK.
//   2.  for each database, verify it using DB_ORDERCHKONLY, after
// Salvage behavior is different. In this case, there is no attempt
// to salvage index databases, and the *CHK* flags are irrelevant,
// so it iterates over the content databases, creating key/value pair
// output for user salvage.
//
// Salvage order is configuration db, dictionary primary, secondary,
// document primary, secondary.
//
// NOTE: according to the Db::verify() interface, the Db handle used
// for a call to verify() cannot be used after that call, regardless of
// return.
//
extern "C" int __db_verify_internal __P((DB *, const char *, const char *, void *, int (*)(void *, const void *), u_int32_t));

extern "C"
int _verify_callback_dbxml(void *handle, const void *str_arg)
{
	char *str = (char *)str_arg;
	ostream *out = (ostream*)handle;
	(*out) << str;
	if (out->fail())
		return (EIO);
	return (0);
}

void Container::verify(Manager &mgr, const std::string &name,
		       std::ostream *out, u_int32_t flags)
{
	int ret = 0;

	if (!(flags & DB_SALVAGE)) {
		// only done for verify w/o salvage
		flags |= DB_NOORDERCHK;
		flags &= ~DB_ORDERCHKONLY;
		DB *db;
		ret = db_create(&db, mgr.getDB_ENV(), 0);
		if (ret == 0)
			ret = __db_verify_internal(
				db,
				(name.length() == 0 ? 0 : name.c_str()), //db
				NULL, //subdb
				out, _verify_callback_dbxml, flags);
	}
	if (ret == 0) {
		if (!(flags & DB_SALVAGE)) {
			flags &= ~DB_NOORDERCHK;
			flags |= DB_ORDERCHKONLY;
		}
		// this pass is done for both the salvage and
		// verify cases.
		ret = ConfigurationDatabase::verify(
			mgr.getDB_ENV(), name, out, flags);
		if (ret == 0) {
			ret = DictionaryDatabase::verify(
				mgr.getDB_ENV(), name, out, flags);
		}
		if (ret == 0) {
			switch(ConfigurationDatabase::readContainerType(
				       mgr.getDB_ENV(), name)) {
			case XmlContainer::WholedocContainer: {
				ret = DocumentDatabase::verify(
					mgr.getDB_ENV(), name,
					XmlContainer::WholedocContainer,
					out, flags);
				break;
			}
			case XmlContainer::NodeContainer: {
				ret = NsDocumentDatabase::verify(
					mgr.getDB_ENV(), name, out, flags);
				break;
			}
			default: DBXML_ASSERT(false);
			}
		}
	}
	if ((ret == 0) && !(flags & DB_SALVAGE)) {
		// only done for verify -- do ordercheck on index dbs
		int j = 0;
		const Syntax *syntax =
			SyntaxManager::getInstance()->getNextSyntax(j);
		while(syntax != 0 && ret == 0) {
			ret = SyntaxDatabase::verify(syntax,
						     mgr.getDB_ENV(),
						     name, NULL, flags);
			syntax = SyntaxManager::getInstance()->
				getNextSyntax(j);
		}
	}
	if (ret) {
		throw XmlException(ret);
	} else {
		Log::log(mgr.getDB_ENV(), C_CONTAINER, L_DEBUG,
			 name.c_str(), "Container verified");
	}
}

int Container::writeHeader(const std::string &name, std::ostream *out)
{
	(*out) << "xml_database=" << name << endl;
	return 0;
}

int Container::verifyHeader(const std::string &name, std::istream *in)
{
	int ret = 0;
	char keyname[64], dbname[256];

	if(!in->get(keyname, sizeof keyname, '=') || strcmp(keyname, "xml_database") != 0 || in->get() != '=' ||
	   !in->get(dbname, sizeof dbname) || in->get() != '\n'|| name != dbname) {
		ret = EINVAL;
	}

	return ret;
}

// (re) set the indexNodes state
void Container::setIndexNodes(Transaction *txn, bool indexNodes)
{
	getConfigurationDB()->
		setIndexNodes(txn, indexNodes);
	indexNodes_ = indexNodes;
}

// reindex container (closed container)
//static
void Container::reindexContainer(Transaction *txn,
				 Manager &mgr,
				 const std::string &name,
				 UpdateContext &context,
				 const ContainerConfig &flags)
{
	Log::log(mgr.getDB_ENV(), C_CONTAINER, L_INFO,
		 name.c_str(), "Reindexing container");

	// need an open container (will throw if container
	// doesn't exist)
	ContainerConfig tflags;
	if(txn)
		tflags.setTransactional(true);
	// don't use top-level txn for opening the container
	XmlContainer cont = mgr.openContainer(
		name, 0, tflags, true);

	// The DBXML_INDEX_NODES and DBXML_NO_INDEX_NODES flags override the default
	if(flags.getIndexNodes() == XmlContainerConfig::On) {
		((Container&)cont).setIndexNodes(txn, true);
	}
	if(flags.getIndexNodes() == XmlContainerConfig::Off) {
		((Container&)cont).setIndexNodes(txn, false);
	}
	
	// reindex (use flags passed in, they were checked in the caller)
	((Container&)cont).reloadIndexes(txn, context, flags);
	Log::log(mgr.getDB_ENV(), C_CONTAINER, L_INFO,
		 name.c_str(), "Reindex complete");
}

class truncator : public DbWrapper::DbFunctionRunner
{
public:
	truncator() {}
	void run(Transaction *txn, DbWrapper &database) {
		if(Log::isLogEnabled(Log::C_CONTAINER, Log::L_INFO)) {
			std::string msg = "Truncating DB: ";
			msg += database.getDatabaseName();
			Log::log(database.getEnvironment(),
				 Log::C_CONTAINER, Log::L_INFO,
				 msg.c_str());
		}
		u_int32_t count;
		DB_TXN *dbTxn = (txn ? txn->toDB_TXN(txn) : 0);
		DB *db = database.getDb();
		int err = db->truncate(db, dbTxn, &count, 0);
		if (err != 0)
			throw XmlException(err);
	}
};

class compactor : public DbWrapper::DbFunctionRunner
{
public:
	compactor() {}
	void run(Transaction *txn, DbWrapper &database) {
#if DBVER > 43
		if(Log::isLogEnabled(Log::C_CONTAINER, Log::L_INFO)) {
			std::string msg = "Compacting DB: ";
			msg += database.getDatabaseName();
			Log::log(database.getEnvironment(),
				 Log::C_CONTAINER, Log::L_INFO,
				 msg.c_str());
		}
		DB_TXN *dbTxn = (txn ? txn->toDB_TXN(txn) : 0);
		// TBD: get the statistics, and log them
		DB_COMPACT stats;
		memset(&stats, 0, sizeof(stats));
		int err = database.getDb()->compact(database.getDb(), dbTxn, 0,
						    0, &stats, DB_FREE_SPACE, 0);
		if (err != 0)
			throw XmlException(err);
		if(Log::isLogEnabled(Log::C_CONTAINER, Log::L_INFO)) {
			ostringstream oss;
			oss << "Stats for DB " << database.getDatabaseName() << ":\n";
			oss << "\tpages examined:\t" << stats.compact_pages_examine << "\n";
			oss << "\tpages freed:\t" << stats.compact_pages_free << "\n";
			oss << "\tlevels removed:\t" << stats.compact_levels << "\n";
			oss << "\tpages returned to file system:\t" << stats.compact_pages_truncated << "\n";
			Log::log(database.getEnvironment(),
				 Log::C_CONTAINER, Log::L_INFO,
				 oss.str().c_str());
		}
#endif			
	}
};

#if USE_DBXML_DISK //used by the dbxml_disk utility
class stats : public DbWrapper::DbFunctionRunner
{
public:
	typedef std::map<std::string, DB_BTREE_STAT*> databaseStats; //since all dbxml databases are b-trees
	stats() : sp() {}
	~stats() {
		for(databaseStats::iterator itr = sp.begin(); itr != sp.end(); itr++)
			free(itr->second);
	}
	void run(Transaction *txn, DbWrapper &database) {
		if(Log::isLogEnabled(Log::C_CONTAINER, Log::L_INFO)) {
			std::string msg = "Retrieving stats for DB: ";
			msg += database.getDatabaseName();
			Log::log(database.getEnvironment(),
				 Log::C_CONTAINER, Log::L_INFO,
				 msg.c_str());
		}
		
		DB_BTREE_STAT *dbStats = 0;
		int err = database.stat(txn, &dbStats, 0);			
		if (err != 0)
			throw XmlException(err);
		sp[database.getDatabaseName()] = dbStats;
	}

	databaseStats &getStats() { 
		return sp; 
	}
private:
	databaseStats sp; 
};

class keyDataSize : public DbWrapper::DbFunctionRunner
{
public:
	//maps the name of the database with the total bytes occupied by keys and data
	struct keyData { 
		size_t keySize;
		size_t dataSize;
		NsFormat::nodeRecordSizes nodeSizes;
	};
	typedef std::map<std::string, keyData> keyDataStats;
	keyDataSize() : sp(), nodeStorageName() {}
	~keyDataSize() {}
	void run(Transaction *txn, DbWrapper &database) {
		keyData kd;
		kd.keySize = 0;
		kd.dataSize = 0;
		Cursor cur(database, txn, CURSOR_READ);
		DbXmlDbt key, data;
		int err = 0;
		bool nodeDatabase = false;
		if (nodeStorageName == database.getDatabaseName()) {
			nodeDatabase = true;
			kd.nodeSizes.attributes = 0;
			kd.nodeSizes.header = 0;
			kd.nodeSizes.name = 0;
			kd.nodeSizes.navigation = 0;
			kd.nodeSizes.text = 0;
		}
		while ((err = cur.get(key, data, DB_NEXT)) == 0) {
			kd.keySize += key.size;
			kd.dataSize += data.size;
			if(nodeDatabase) {
				unsigned char *datap = (unsigned char *)data.data;
				const NsFormat &fmt = NsFormat::getFormat((int)(*datap));
				NsNode *nsnode = fmt.unmarshalNodeData(datap, false, &kd.nodeSizes);
				nsnode->acquire();
				nsnode->release();
			}
		}
		if(err != DB_NOTFOUND)
			throw XmlException(XmlException::DATABASE_ERROR, "Error occured while reading the database.");
		sp[database.getDatabaseName()] = kd;
	}
	
	keyDataStats &getStats() { 
		return sp; 
	}

	void setNodeStorageName(const std::string &name) {
		nodeStorageName = name;
	}

private:
	keyDataStats sp; 
	std::string nodeStorageName;
};

#endif

// Implementation of NodeIterator that walks all documents in a container

class DocumentIterator : public NodeIterator
{
public:
	DocumentIterator(Transaction *txn, const Container *cont,
			 const DocumentDatabase *docDb,
			 const LocationInfo *location, u_int32_t flags)
		: NodeIterator(location), cont_(cont)
	{
		docDb->createDocumentCursor(txn, cursor_, flags);
	}
	//  NodeIterator interface
	virtual bool next(DynamicContext *context) {
		int err = cursor_->next(did_);
		if(err != 0) throw XmlException(err);

		return (did_ != 0);
	}
	
	virtual bool seek(int containerID, const DocID &did,
			  const NsNid &nid, DynamicContext *context) {
		if(cont_->getContainerID() < containerID) return false;

		did_ = did;
		int err = cursor_->seek(did_); // will reset did_
		if(err != 0) throw XmlException(err);

		return (did_ != 0);
	}
	
	virtual DbXmlNodeImpl::Ptr asDbXmlNode(DynamicContext *context) {
		OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();
		XmlDocument doc;
		cont_->getDocument(oc, did_, doc, DBXML_LAZY_DOCS);
		return (DbXmlNodeImpl*)((DbXmlFactoryImpl*)context->
			getItemFactory())->createNode(doc, context).get();
	}

	// NodeInfo interface
	virtual Type getType() const { return DOCUMENT; }
	virtual int getContainerID() const { return cont_->getContainerID(); }
	virtual DocID getDocID() const { return did_; }
	virtual const NsNid getNodeID() const {
		return *NsNid::getRootNid();
	}

	// Use DBXML_ASSERT to find stray callers if they exist
	virtual const NsNid getLastDescendantID() const { DBXML_ASSERT(false); return NsNid(); }
	virtual u_int32_t getNodeLevel() const { return 0; }
	virtual u_int32_t getIndex() const { return 0; }
	virtual bool isLeadingText() const { return false; }

private:
	ScopedPtr<DocumentCursor> cursor_;
	const Container *cont_;
	DocID did_;
};

// truncate container (closed container)
//static
void Container::truncateContainer(Transaction *txn,
				  Manager &mgr,
				  const std::string &name,
				  UpdateContext &context)
{
	Log::log(mgr.getDB_ENV(), C_CONTAINER, L_INFO,
		 name.c_str(), "Truncating container");
	// need an open container (will throw if container
	// doesn't exist)
	ContainerConfig tflags;
	if(txn)
		tflags.setTransactional(true);
	// don't use top-level txn for opening the container
	XmlContainer cont = mgr.openContainer(
		name, 0, tflags, true);
	truncator trunc;
	// skip core dbs, or the container is corrupt
	((Container&)cont).runOnAllDatabases(txn, trunc, true);
}

// compact container (closed container)
//static
void Container::compactContainer(Transaction *txn,
				 Manager &mgr,
				 const std::string &name,
				 UpdateContext &context)
{
	Log::log(mgr.getDB_ENV(), C_CONTAINER, L_INFO,
		 name.c_str(), "Compacting container");
	// need an open container (will throw if container
	// doesn't exist)
	ContainerConfig tflags;
	if(txn)
		tflags.setTransactional(true);
	// don't use top-level txn for opening the container
	XmlContainer cont = mgr.openContainer(name, 0, tflags, true);
	compactor comp;
	((Container&)cont).runOnAllDatabases(txn, comp, false);
}

// upgrade functions

// assumes container is not open. Caller is responsible
// for checking (XmlManager)
//static
void Container::upgradeContainer(const std::string &name,
				 Manager &mgr,
				 UpdateContext &context)
{
	int err = 0;
	DB_ENV *env = mgr.getDB_ENV();
	// check for existence of container, and version first.
	unsigned int old_version = Container::checkContainer(name, env);
	if (old_version == version) // version is a class static
		return;
	else if (!old_version) {
		ostringstream s;
		s << "Cannot upgrade non-existent container: " << name;
		throw XmlException(XmlException::INVALID_VALUE, s.str());
	}
	if (version < old_version) {
		ostringstream s;
		s << "Container version '";
		s << old_version;
		s << "' is more recent than the bdbxml library version '";
		s << version;
		s << "'.  Use a more recent release of the bdbxml library";
		throw XmlException(XmlException::VERSION_MISMATCH, s.str());
	}
	// Versions and supported upgrades
	// 3 (2.0)
	// 4 (2.1)
	// 5 (2.2)
	// Current version is 6 (2.3)
	// 2.0 or 2.1 must be upgraded to 2.2 first
	if (old_version != VERSION_20 &&
	    old_version != VERSION_21 &&
	    old_version != VERSION_22) {
		throw XmlException(
			XmlException::VERSION_MISMATCH,
			"Upgrade is not supported from release 1.2.x to release 2.x.");
	}

	// Start upgrade...

#if 0
	// 4.3 and 4.4 do not require DB upgrade, and
	// DB upgrade requires matching endianism of host,
	// while BDB XML upgrade does not, so leave out for now.
	//
	// Upgrade Berkeley DB
	// NOTE: maybe this needs to be done first...
	//
	{
		DB *dbp;
		err = db_create(env, &dbp, 0);
		if (err == 0)
			err = db->upgrade(dbp, name.c_str(), 0);
		else
			throw XmlException(err);
		dbp->close(dbp, 0);
	}
#endif	
	//
	// Upgrade Berkeley DB XML
	//
	if(err == 0) {
		std::string tname = name;
		tname += "_tempUpgrade";
		doUpgrade(name, tname, mgr, old_version, version);
		{
			Log::log(env, C_CONTAINER, L_INFO,
				 name.c_str(), "Upgrade: reloading indexes");
			// reload indexes requires an open container
			ContainerConfig config;
			config.setCompressionName(XmlContainerConfig::NO_COMPRESSION);
			XmlContainer cont = mgr.openContainer(
				tname, 0, config, true);
			((Container&)cont).reloadIndexes(0, context, DEFAULT_CONFIG);
			Log::log(env, C_CONTAINER, L_INFO,
				 name.c_str(), "Upgrade: done reloading indexes");
		}
		Log::log(env, C_CONTAINER, L_INFO,
			 name.c_str(), "Upgrade: removing/renaming");
		// remove old container and rename new
		err = env->dbremove(env, 0, name.c_str(), 0, 0);
		if (err == 0)
			err = env->dbrename(env,
					    0, tname.c_str(), 0,
					    name.c_str(), 0);
		if (err == 0)
			Log::log(env, C_CONTAINER, L_INFO,
				 name.c_str(), "Upgrade complete");
		else
			throw XmlException(err);
	}
}

// 2.0 -> 2.1
//  o  Databases created on big-endian machines --
//     IDs need byte-swapping in dictionary secondary data
//  o  Containers created on big-endian machines --
//     IDs need byte-swapping in document secondary keys
//  o  NOTE: if the "current" machine is little-endian, it needs to
//     detect the big-endian nature of the container and byte-swap, anway,
//     and big-endian machines need to do the reverse -- detect that
//     the container was created little-endian, and *not* swap...
//     In other words, the check cannot be based on the current machine's
//     endian-ness.
//  o  upgrade version (ConfigurationDatabase)
//  o  dump/load indexes
// 2.1 -> 2.2
//  o upgrade version in configuration DB
//  o configuration DB change:
//    -- added upgrade state item to config. DB to track upgrade
//    -- removed DbSequence record; it is now in its own
//       btree database -- "secondary_sequence"
//  o Changed doc and name ids, and keys to a marshaled form, and use
//  custom comparison function.  This affects these databases:
//    -- content_document (key)
//    -- node_nodestorage (key)
//    -- secondary_document (metadata keys)
//    -- secondary_dictionary (metadata values)
//  Index formats have also changed, so reindexing is necessary.
// 2.2 -> 2.3
//  o version number in configuration DB
//  o node storage format change (including node ID now in BTree key)
//  o remove index types (anyURI, QName, NOTATION)
//

static void doUpgrade(const std::string &name,
		      const std::string &tname, Manager &mgr,
		      unsigned int old_version, unsigned int current_version)
{
	char buf[4096];
	sprintf(buf, "Upgrading container %s from format version %d to version %d\n",
		name.c_str(), old_version, current_version);
	Log::log(mgr.getDB_ENV(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), buf);

	ConfigurationDatabase::upgrade(name, tname, mgr, old_version, current_version);
	Log::log(mgr.getDB_ENV(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), "Configuration upgrade complete");

	// It's safe to open configuration DB to read container type
	// now that it's upgraded
	XmlContainer::ContainerType ctype =
		ConfigurationDatabase::readContainerType(mgr.getDB_ENV(), name);
	
	DictionaryDatabase::upgrade(name, tname, mgr, old_version, current_version);
	Log::log(mgr.getDB_ENV(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), "Dictionary upgrade complete");

	Log::log(mgr.getDB_ENV(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), "Starting Document upgrade");
	DocumentDatabase::upgrade(name, tname, mgr, old_version, current_version);
	Log::log(mgr.getDB_ENV(), Log::C_CONTAINER, Log::L_INFO,
		 name.c_str(), "Document upgrade complete");
	if (ctype == XmlContainer::NodeContainer) {
		Log::log(mgr.getDB_ENV(), Log::C_CONTAINER, Log::L_INFO,
			 name.c_str(), "Starting Node Storage upgrade");
		NsDocumentDatabase::upgrade(name, tname, mgr,
					    old_version, current_version);
		Log::log(mgr.getDB_ENV(), Log::C_CONTAINER, Log::L_INFO,
			 name.c_str(), "Node Storage upgrade complete");
	}
}

// This class could create Index databases on demand,
// from the Indexer and KeyStash code.
// All the code is here, except for this problem:
//  o if the transaction in which the DB(s) is/are created is
//  aborted, the new DBs must be cleared.
//  o this means using tracking new DBS using
//    the abort hooks for txn.  Just a little more code.
// Once this is done, just un-comment out the flags &= line
void Container::openIndexDbs(Transaction *txn, const ContainerConfig &origFlags)
{
	// Iterate over the registered syntax types, creating a
	// SyntaxDatabase for each.
	indexes_.resize(SyntaxManager::getInstance()->size());
	int j = 0;
	ContainerConfig flags(origFlags);
	flags.setAllowCreate(false); // don't allow creation
	flags.setExclusiveCreate(false);
	const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	while (syntax != 0) {
		SyntaxDatabase *sdb = 0;
		ContainerConfig tflags(containerConfig_);
		if (syntax->getType() == Syntax::STRING) 
			tflags.setFlags(origFlags);
		else 
			tflags.setFlags(flags);
		try {
			sdb = new SyntaxDatabase(
                                syntax, mgr_.getDB_ENV(),
                                txn, name_, indexNodes_, tflags,
                                usingCDB_);
		} catch (XmlException &xe) {
			// ENOENT is not an error
			if (xe.getDbErrno() != ENOENT)
				throw;
		}
		indexes_[syntax->getType()].reset(sdb);
		syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	}
}

//
// upgrade for indexes -- equivalent to dump/load, but in-place
// 1. truncate indexes
// 2. grab IndexSpecification and reindex
//
void Container::reloadIndexes(Transaction *txn, UpdateContext &uc, const ContainerConfig &flags)
{
	int j = 0;
	u_int32_t count = 0;
	const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	while (syntax != 0) {
		SyntaxDatabase *sdb = indexes_[syntax->getType()].get();
		if (sdb) {
			sdb->getIndexDB()->truncate(txn, &count, 0);
			sdb->getStatisticsDB()->truncate(txn, &count, 0);
		}
		syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	}

	if((flags.getStatistics() == XmlContainerConfig::Off) && stats_) {
		// Close the handle on the StructuralStatsDatabase first
		stats_.reset(0);
		// Then remove the StructuralStatsDatabase
		int err = StructuralStatsDatabase::remove(mgr_.getDB_ENV(), txn, name_);
		if(err != 0) throw XmlException(err);
	}
	else if(stats_) {
		stats_->truncate(txn, &count, 0);
	}
	else if(flags.getStatistics() == XmlContainerConfig::On) {
		// In this case, we actually create the StructuralStatsDatabase
		ContainerConfig config;
		config.setAllowCreate(true);
		config.setPageSize(containerConfig_.getPageSize());
		if(containerConfig_.getEncrypted()) config.setEncrypted(true);
		stats_.reset(new StructuralStatsDatabase(mgr_.getDB_ENV(), txn, name_,
				     config, usingCDB_));
	}

	// Update the index version to the current version
	configuration_->updateIndexVersion(txn);

	// reindex (this is the same as code in load)
	XmlIndexSpecification is;
	int ret = getConfigurationDB()->
		getIndexSpecification(txn, is);
	if(ret == 0) {
		ret = reindex(txn, (IndexSpecification&)is, /*updateStats*/true);
	}
}

SyntaxDatabase *Container::getIndexDB(Syntax::Type type, Transaction *txn,
				      bool toWrite)
{
	SyntaxDatabase *sdb = indexes_[type].get();
	ContainerConfig flags;
	flags.setAllowCreate(true);
	flags.setPageSize(containerConfig_.getPageSize());
	flags.setMode(0);
	flags.setDbSetFlags(containerConfig_.getDbSetFlags());
	if (!sdb && toWrite) {
		indexes_.resize(SyntaxManager::getInstance()->size());
		sdb = new SyntaxDatabase(
                        SyntaxManager::getInstance()->getSyntax(type),
                        mgr_.getDB_ENV(), txn, name_, indexNodes_, flags,
                        usingCDB_);
		indexes_[type].reset(sdb);
		if (txn != 0) {
			// protect initialization among threads
			lock();
			if (indexDbNotify_ == 0) {
				indexDbNotify_ = new IndexDbNotify(this);
				txn->registerNotify(indexDbNotify_);
			}
			indexDbNotify_->add(type);
			unlock();
		}
	}
	return sdb;
}

const SyntaxDatabase * Container::getIndexDB(Syntax::Type type) const
{
	return indexes_[type].get();
}

void IndexDbNotify::postNotify(bool commit)
{
	if (!commit) {
		vector<int>::iterator it;
		for (it = dbs_.begin();
		     it != dbs_.end(); it++) {
			cont_->closeIndexes(*it);
		}
	}
	cont_->lock();
	IndexDbNotify *toDel = cont_->indexDbNotify_;
	DBXML_ASSERT(toDel == this);
	cont_->indexDbNotify_ = 0;
	cont_->unlock();
	delete toDel;
}

Transaction *Container::autoTransact(Transaction *txn,
				     TransactionGuard &txnGuard,
				     bool ckReadOnly) const
{
	if (ckReadOnly)
		checkReadOnly(); // may throw
	return Transaction::autoTransact(txn, mgr_, txnGuard, usingTxns_, usingCDB_);
}

/////////////////////////////////
// Implemention of ContainerBase methods

void Container::checkReadOnly() const
{
	if (readOnly_) {
		string msg = "Cannot perform updating operation on read-only container: ";
		msg += getName();
		throw XmlException(XmlException::INVALID_VALUE,
				   msg);
	}
}

void Container::getIndexSpecification(Transaction *txn,
				      IndexSpecification &is)
{
	getConfigurationDB()->getIndexSpecification(txn, is);
}

Cost Container::getDocumentSSCost(OperationContext &oc, StructuralStatsCache &cache)
{
	// We don't need the timezone for a presence lookup
	Key key(0);
	key.setIndex(Index::PATH_NODE|Index::NODE_METADATA|Index::KEY_EQUALITY|Syntax::STRING);
	key.setID1(dictionary_->getNIDForName());

	return getIndexCost(oc, DbWrapper::PREFIX, key);
}

Cost Container::getAttributeSSCost(OperationContext &oc, StructuralStatsCache &cache,
	const char *childUriName)
{
	// usable_page_size= (pagesize-overhead)*page_fill_factor
	//
	// DB4.1 has btree page overhead of 26. Fill factor for a full page
	// is between 50% and 100%... let's guess 75%.
	//
// 	double pageSize = (getDocumentDB()->getPageSize() - 26) * 0.75;

	// TBD We can't use the attribute name at the moment - jpcs
	// TBD save attribute counts? - jpcs

	StructuralStats docStats = cache.get(this, oc, 0, 0);

	Cost cost;
	cost.keys = (double) docStats.sumNumberOfDescendants_;
// 	cost.pages = ceil(docStats.sumDescendantSize_ / pageSize);
	cost.pagesForKeys = (double) docStats.sumDescendantSize_;

	if(!stats_ || isWholedocContainer()) {
		// Make the cost BIG, so the SS doesn't get chosen
		cost.pagesOverhead = cost.pagesForKeys * 1000;
		cost.pagesForKeys = 0;
	}

	return cost;
}

Cost Container::getElementSSCost(OperationContext &oc, StructuralStatsCache &cache,
	const char *childUriName)
{
	// usable_page_size= (pagesize-overhead)*page_fill_factor
	//
	// DB4.1 has btree page overhead of 26. Fill factor for a full page
	// is between 50% and 100%... let's guess 75%.
	//
// 	double pageSize = (getDocumentDB()->getPageSize() - 26) * 0.75;

	StructuralStats docStats = cache.get(this, oc, 0, 0);

	Cost cost;
	if(childUriName != 0) {
		NameID id;
		lookupID(oc, childUriName, ::strlen(childUriName), id);

		StructuralStats elemStats = cache.get(this, oc, id, 0);
		cost.keys = (double) elemStats.numberOfNodes_;
	} else {
		cost.keys = (double) docStats.sumNumberOfDescendants_;
	}
// 	cost.pages = ceil(docStats.sumDescendantSize_ / pageSize);
	cost.pagesForKeys = (double) docStats.sumDescendantSize_;

	if(!stats_ || isWholedocContainer()) {
		// Make the cost BIG, so the SS doesn't get chosen
		cost.pagesOverhead = cost.pagesForKeys * 1000;
		cost.pagesForKeys = 0;
	}

	return cost;
}

Cost Container::getIndexCost(OperationContext &oc,
			DbWrapper::Operation op1, const Key &key1,
			DbWrapper::Operation op2,
			const Key &key2)
{
	// usable_page_size= (pagesize-overhead)*page_fill_factor
	//
	// DB4.1 has btree page overhead of 26. Fill factor for a full page
	// is between 50% and 100%... let's guess 75%.
	//
// 	double pageSize = (getDocumentDB()->getPageSize() - 26) * 0.75;

	KeyStatistics statistics = getKeyStatistics(oc.txn(), key1);

	Cost result;

	// Let's just assume that the distribution of keys across
	// the domain of possible values is flat. There are probably
	// more clever things we could do.
	//
	switch(op1) {
	case DbWrapper::EQUALITY: {
		if(key1.getIndex().getKey() == Index::KEY_EQUALITY) {
			// jcm - could use key_range... but is this more accurate?
			// (number of equality keys / number of unique keys)
			result.keys =
				(statistics.numUniqueKeys_ > 0
				 ? (double)statistics.numIndexedKeys_ / (double)statistics.numUniqueKeys_
				 : 0);
		} else {
			result.keys = statistics.numIndexedKeys_;
		}
		break;
	}
	case DbWrapper::PREFIX: {
		if (key1.getValueSize() == 0) {
			// Doing a prefix search on just the structure.
			result.keys = statistics.numIndexedKeys_;
			// BREAK!
			break;
		} else {
			// Doing a prefix search on the structure, and the start of the value.
			// FALL THROUGH!
		}
	}
	case DbWrapper::LTX:
	case DbWrapper::LTE:
	case DbWrapper::GTX:
	case DbWrapper::GTE: {
		double percentageOfKeys = getPercentage(oc, op1, key1, op2, key2);
		result.keys = (double)statistics.numIndexedKeys_ * percentageOfKeys;
		break;
	}
	default: {  // keep compilers quiet
		break;
	}
	}

// 	result.pages = ceil((result.keys * statistics.averageKeyValueSize()) / pageSize);
	result.pagesForKeys = result.keys * statistics.averageKeyValueSize();

	if (op1 != DbWrapper::EQUALITY && op1 != DbWrapper::NONE) {
		// Add the page cost of sorting, which is 3 times the cost without sorting -
		// once for reading it, another for writing it back out in sorted order, and
		// then again to read it in sorted order.
		result.pagesOverhead = 2 * result.pagesForKeys;
	}

	return result;
}

double Container::getPercentage(OperationContext &oc,
				DbWrapper::Operation op1, const Key &key1,
				DbWrapper::Operation op2,
				const Key &key2)
{
	double percentage = 0.0;
	const SyntaxDatabase *database = getIndexDB(key1.getSyntaxType());
	if (database) {
		if(op2 == DbWrapper::NONE) {
			percentage = database->getStatisticsDB()->percentage(
				oc, op1, DbWrapper::NONE,
				DbWrapper::NONE, key1, key2);
		} else {
			percentage = database->getStatisticsDB()->percentage(
				oc, DbWrapper::RANGE,
				op1, op2, key1, key2);
		}
	}
	return percentage;
}

StructuralStats Container::getStructuralStats(OperationContext &oc, const NameID &id1, const NameID &id2) const
{
	if(!stats_) return StructuralStats(id1 != 0, id2 != 0);

	StructuralStats stats;
	int err = stats_->getStats(oc, id1, id2, stats);
	if(err != 0) throw XmlException(err);

	return stats;
}

//
// if docName is set, just return iterator for that specific document
//
NodeIterator *Container::createDocumentIterator(
	DynamicContext *context, const LocationInfo *location,
	const char *docName, size_t docNameLen) const
{
	OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();
	Transaction *txn = oc.txn();
	
 	if (docNameLen == 0) {
 		// iterate over entire container
 		return new DocumentIterator(txn, this, getDocumentDB(),
 					    location, 0 /*flags*/); 
 	}

	// Use metadata name index lookup for a named document
	// We don't need the timezone

	Key key(0);
	key.setIndex(Index::PATH_NODE|Index::NODE_METADATA|Index::KEY_EQUALITY|Syntax::STRING);
	key.setID1(dictionary_->getNIDForName());
	key.setValue(docName, docNameLen);

	return createIndexIterator(Syntax::STRING, context, /*documentIndex*/false, location,
		DbWrapper::EQUALITY, key);
}

NodeIterator *Container::createIndexIterator(
	Syntax::Type type, DynamicContext *context, bool documentIndex,
	const LocationInfo *l, DbWrapper::Operation op1, const Key &key1,
	DbWrapper::Operation op2, const Key &key2) const
{
	OperationContext &oc = GET_CONFIGURATION(context)->getOperationContext();
	Transaction *txn = oc.txn();

	if(!indexNodes_) documentIndex = false;

	const SyntaxDatabase *database = getIndexDB(type);
	return database->createNodeIterator(
		const_cast<Container*>(this), txn, documentIndex, l,
		op1, key1, op2, key2);
}

NamedNodeIterator *Container::createAttributeIterator(
	DynamicContext *context, const LocationInfo *location,
	const NameID &nsUriID) const
{
	Container *tcont = const_cast<Container*>(this);
	if (isNodeContainer()) 
		return new AttributeSSIterator(
			tcont->getDocumentDB()->getNodeDatabase(),
			tcont, context, location, nsUriID);
	else
		return new DLSAttributeSSIterator(
			tcont->getDocumentDB(),
			tcont->getDictionaryDatabase(),
			tcont, context, location, nsUriID);
}

NamedNodeIterator *Container::createElementIterator(
	DynamicContext *context, const LocationInfo *location) const
{
	Container *tcont = const_cast<Container*>(this);
	if (isNodeContainer())
		return new ElementSSIterator(
			tcont->getDocumentDB()->getNodeDatabase(),
			tcont, context, location);
	else
		return new DLSElementSSIterator(
			tcont->getDocumentDB(),
			tcont->getDictionaryDatabase(),
			tcont, context, location);
}

void Container::dumpStructuralStatistics(XmlTransaction &txn, std::ostream &out)
{
	OperationContext oc(txn);
	getStructuralStatsDB()->display(oc, out, getDictionaryDatabase());
}

static unsigned int _checkVersion(DB *dbp)
{
  /* check version */
  const char *version="version";
  int ret;
  char databuf[20];
  DBT versKey, versData;
  memset(&versKey, 0, sizeof(versKey));
  memset(&versData, 0, sizeof(versData));
  versKey.data = (void*)version;
  versKey.size = (u_int32_t)strlen(version) + 1;
  versKey.ulen = versKey.size;
  versKey.flags = DB_DBT_USERMEM;
  versData.data = databuf;
  versData.size = 20;
  versData.ulen = 20;
  versData.flags = DB_DBT_USERMEM;
      
  ret = dbp->get(dbp, NULL, &versKey, &versData, 0);
  if (ret == 0)
    return (unsigned int)atoi(databuf);
  return 0;
}

//static
unsigned int Container::checkContainer(const string &name, DB_ENV *dbenv)
{
	const char *dbxmlConfig = "secondary_configuration";
	DB *dbp;
	int ret;
	unsigned int version = 0;
	if (name.size() == 0)
		return 0;
	ret = db_create(&dbp, dbenv, 0);
	if (!ret) {
		ret = dbp->open(dbp, NULL, name.c_str(),
				dbxmlConfig, DB_BTREE, DB_RDONLY, 0);
		if (ret == 0) {
			version = _checkVersion(dbp);
		}
		dbp->close(dbp, 0);
	}
	return version;
}

void Container::runOnAllDatabases(Transaction *txn,
				  DbWrapper::DbFunctionRunner &runner,
				  bool skipCoreDbs)
{
	// databases:
	//   configuration, sequence, dictionary primary/secondary,
	//   document secondary, document content|node storage
	//   [indexes]
	if (!skipCoreDbs) {
		configuration_->run(txn, runner);
		dictionary_->run(txn, runner);
	}
	documentDb_->run(txn, runner);

	// indexes are harder...
	int j = 0;
	const Syntax *syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	while (syntax != 0) {
		SyntaxDatabase *sdb = indexes_[syntax->getType()].get();
		if (sdb)
			sdb->run(txn, runner);
		syntax = SyntaxManager::getInstance()->getNextSyntax(j);
	}

	if(stats_) stats_->run(txn, runner);
}

std::string Container::disk(Transaction *txn)
{
//used by the dbxml_disk utility
#if USE_DBXML_DISK
	std::string dbInfo;
	ostringstream oss;
	stats dbStats;
	oss << "Container disk usage\n";
	runOnAllDatabases(txn, dbStats, false);
	stats::databaseStats dbstat = dbStats.getStats();
	keyDataSize keyData;
	if(containerConfig_.getContainerType() == XmlContainer::NodeContainer)
		keyData.setNodeStorageName("node_nodestorage"); 
	runOnAllDatabases(txn, keyData, false);
	keyDataSize::keyDataStats kdstat = keyData.getStats();
	for(stats::databaseStats::iterator itr = dbstat.begin(); itr != dbstat.end(); itr++) {
		oss << "\nDatabase name: " << itr->first << "\n";
		keyDataSize::keyData sizes = kdstat[itr->first];
		DB_BTREE_STAT *dbbtree = itr->second;
		uint32_t TotalSize = (dbbtree->bt_int_pg + dbbtree->bt_leaf_pg + dbbtree->bt_dup_pg 
			+ dbbtree->bt_empty_pg + dbbtree->bt_over_pg + dbbtree->bt_free) * dbbtree->bt_pagesize;
		oss << "Total size (byte): " << TotalSize << "\n";
		uint32_t freeSpace = (dbbtree->bt_empty_pg + dbbtree->bt_free) * dbbtree->bt_pagesize
			+ dbbtree->bt_int_pgfree + dbbtree->bt_leaf_pgfree + dbbtree->bt_dup_pgfree
			+ dbbtree->bt_over_pgfree;
		oss << "Free Space (byte): " << freeSpace << "\n";
		oss << "Fill Factor: " 
			<< 100.0-((double)freeSpace*100.0)/((double)TotalSize)
			<< "\n";
		size_t TotalDataSize = sizes.keySize + sizes.dataSize;
		oss << "Total key size (byte): " << (uint32_t)sizes.keySize << "\n";
		oss << "Total data size (byte): " << (uint32_t)sizes.dataSize << "\n";
		oss << "Percentange of key/data bytes held by data: " 
			<< 100.0-((double)sizes.keySize*100.0)/((double)TotalDataSize)
			<< "\n";
		if(itr->first == "node_nodestorage") {
			oss << "Node Records \n";
			oss << "Total header size (byte): " << (uint32_t)sizes.nodeSizes.header 
				<< "\n precentage: " << ((double)sizes.nodeSizes.header*100.0)/((double)TotalDataSize) << "\n";
			oss << "Total element name size (part of the header) (byte): " << (uint32_t)sizes.nodeSizes.name 
				<< "\n precentage: " << ((double)sizes.nodeSizes.name*100.0)/((double)TotalDataSize) << "\n";
			oss << "Total text size (byte): " << (uint32_t)sizes.nodeSizes.text
				<< "\n precentage: " << ((double)sizes.nodeSizes.text*100.0)/((double)TotalDataSize) << "\n";
			oss << "Total attribute size (byte): " << (uint32_t)sizes.nodeSizes.attributes 
				 << "\n precentage: " << ((double)sizes.nodeSizes.attributes*100.0)/((double)TotalDataSize) << "\n";
			oss << "Total navigation size (byte): " << (uint32_t)sizes.nodeSizes.navigation
				<< "\n precentage: " << ((double)sizes.nodeSizes.navigation*100.0)/((double)TotalDataSize) << "\n";
		}
	}
	dbInfo.assign(oss.str());
	return dbInfo;
#endif
	return "";
}

static void throwUnique(const XmlException &xe, const std::string name,
			bool isDuplicateMD)
{
	std::string str(xe.what());
	if (isDuplicateMD && str.find("name,http://www.sleepycat.com")) {
		str = "Document exists: ";
	} else {
		str.append(", document: ");
	}
	str.append(name);
	throw XmlException(XmlException::UNIQUE_ERROR, str);
}

