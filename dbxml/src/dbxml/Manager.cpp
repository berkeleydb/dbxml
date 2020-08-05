//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "Manager.hpp"
#include "dbxml/XmlManager.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlContainerConfig.hpp"
#include "Transaction.hpp"
#include "Container.hpp"
#include "UTF8.hpp"
#include "Globals.hpp"
#include "SyntaxManager.hpp"
#include "dataItem/DbXmlURIResolver.hpp"
#include "DictionaryDatabase.hpp"
#include "CacheDatabase.hpp"
#include "Document.hpp"
#include "LocalFileInputStream.hpp"
#include "MemBufInputStream.hpp"
#include "URLInputStream.hpp"
#include "query/QueryPlanHolder.hpp"
#ifdef DBXML_COMPRESSION
#include "CompressionZlib.hpp"
#endif

#include <xqilla/utils/ContextUtils.hpp>

// #define MANAGER_REUSE_IDS 1

using namespace DbXml;
using namespace std;

// Default environment cache size to 50MB
// This beats DB's default of 256K, and should not
// consume excessive resource
static const u_int32_t db_cachesize_default = 50 * 1024 * 1024;

// SET_DB_ERRCALL is never defined.  This is for debugging only
#ifdef SET_DB_ERRCALL
static void errcall(const DB_ENV *dbenv, const char *errpfx, const char *msg)
{
	cout << "message: " << (errpfx ? errpfx : "") << ": " << msg << endl;
}

#endif

static void throwContainerNotFound(const std::string &name)
{
	ostringstream oss;
	oss << "Container '" << name << "' does not exist.";
	throw XmlException(XmlException::CONTAINER_NOT_FOUND,
			   oss.str());
}

Manager::Manager(DB_ENV *dbEnv, u_int32_t flags)
	: dbEnv_(dbEnv),
	  dbEnvAdopted_((flags & DBXML_ADOPT_DBENV)!=0),
	  tempDbEnv_(0),
	  dbEnvOpenFlags_(0),
	  flags_(flags),
	  autoOpen_((flags & DBXML_ALLOW_AUTO_OPEN)!=0),
	  defaultContainerConfig_(),
  	  dictionary_(0),
  	  tempDocId_(1),
	  defaultQPR_(0),
	  compressionStore_(),
 	  defaultCompression_(0)
{
	checkFlags(construct_manager_flag_info, "Manager()",
		   flags, DBXML_ADOPT_DBENV|
		   DBXML_ALLOW_EXTERNAL_ACCESS|DBXML_ALLOW_AUTO_OPEN);
	if(dbEnv_ == 0) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Null DbEnv pointer passed as parameter to XmlManager.");
	}
	try {
		const char *dbhome = 0;
		dbEnv_->get_home(dbEnv_, &dbhome);
		if (dbhome && *dbhome)
			dbHome_ = dbhome;
		dbEnv_->get_open_flags(dbEnv_, &dbEnvOpenFlags_);
#if 0
		// Allow DB_AUTO_COMMIT for now -- this is handled by
		// using DB_NO_AUTO_COMMIT in CacheDatabase.cpp for temporary DBs.
		// leave code in case there are unforeseen issues...
		// DB_AUTO_COMMIT is in "regular" flags, not open flags
		u_int32_t dbEnvFlags = 0;
		dbEnv_->get_flags(dbEnv_, &dbEnvFlags);
		if (dbEnvFlags & DB_AUTO_COMMIT) {
			throw XmlException(
				XmlException::INVALID_VALUE,
				"A DbEnv using DB_AUTO_COMMIT cannot be used to construct an XmlManager object.");
		}
#endif
		if (dbEnvOpenFlags_ & DB_INIT_CDB) {
			u_int32_t envFlags = 0;
			dbEnv_->get_flags(dbEnv_, &envFlags);
			if (!(envFlags & DB_CDB_ALLDB)) {
				throw XmlException(
					XmlException::INVALID_VALUE,
					"A DbEnv opened with DB_INIT_CDB must set the DB_CDB_ALLDB flag when used with Berkeley DB XML.");
			}
		}
		
#ifdef DB_SET_ERRCALL
		dbEnv_->set_errcall(dbEnv_, errcall);
#endif
		resolvers_ = new ResolverStore();
		resolvers_->setSecure(
			(flags & DBXML_ALLOW_EXTERNAL_ACCESS) == 0);
		initialize(dbEnv_);
		initTempDbEnv(dbEnv_);
		timezone_ = ContextUtils::getTimezone();
		openContainers_.initialize(*this);
		defaultQPR_ = new QueryPlanRoot(
			openContainers_.getContainerFromID(0,
							   /*acquire*/false));
		(void)getDictionary();
#ifdef DBXML_COMPRESSION
		defaultCompression_ = new CompressionZlib();
#endif
		std::string none(XmlContainerConfig::NO_COMPRESSION);
		std::string defaultComp(
			XmlContainerConfig::DEFAULT_COMPRESSION);
		compressionStore_[none] = NULL;
		compressionStore_[defaultComp] = defaultCompression_;
		defaultContainerConfig_.setManager(this);
	} catch (...) {
		close();
		throw;
	}
}

Manager::Manager(u_int32_t flags)
	: dbEnv_(0),
	  dbEnvAdopted_(true),
	  tempDbEnv_(0),
	  dbEnvOpenFlags_(0),
	  flags_(flags),
	  autoOpen_((flags & DBXML_ALLOW_AUTO_OPEN)!=0),
	  defaultContainerConfig_(),
  	  dictionary_(0),
  	  tempDocId_(1),
	  defaultQPR_(0),
 	  compressionStore_(),
 	  defaultCompression_(0)
{
	int ret = db_env_create(&dbEnv_, 0);
	if (ret)
		throw XmlException(ret);
	try {
		checkFlags(construct_manager_flag_info, "Manager()",
			   flags,
			   DBXML_ALLOW_EXTERNAL_ACCESS|DBXML_ALLOW_AUTO_OPEN);
		resolvers_ = new ResolverStore();
		resolvers_->setSecure(
			(flags & DBXML_ALLOW_EXTERNAL_ACCESS) == 0);
#ifdef DBXML_COMPRESSION
		defaultCompression_ = new CompressionZlib();
#endif
		std::string none(XmlContainerConfig::NO_COMPRESSION);
		std::string defaultComp(
			XmlContainerConfig::DEFAULT_COMPRESSION);
		compressionStore_[none] = NULL;
		compressionStore_[defaultComp] = defaultCompression_;
		initialize(dbEnv_); // initializes Xerces and XQilla
		timezone_ = ContextUtils::getTimezone();
		// init DB defaults
		dbEnv_->set_cachesize(dbEnv_, 0, db_cachesize_default, 1);
		dbEnv_->set_errpfx(dbEnv_, "BDB XML");
		dbEnv_->set_errfile(dbEnv_, stderr);
		defaultContainerConfig_.setManager(this);
#ifdef DB_SET_ERRCALL
		dbEnv_->set_errcall(dbEnv_, errcall);
#endif
		ret = dbEnv_->open(dbEnv_, 0,
				   DB_PRIVATE|DB_CREATE|DB_THREAD|DB_INIT_MPOOL, 0);
		if (ret)
			throw XmlException(ret);
		dbEnv_->get_open_flags(dbEnv_, &dbEnvOpenFlags_);
		openContainers_.initialize(*this);
		defaultQPR_ = new QueryPlanRoot(
			openContainers_.getContainerFromID(0,
							   /*acquire*/false));
		initTempDbEnv(dbEnv_);
		(void)getDictionary();
	} catch (...) {
		close();
		throw;
	}
}

Manager::~Manager()
{
	close();
}

void Manager::close()
{
	openContainers_.releaseRegisteredContainers();
	
	if (dictionary_)
		delete dictionary_;
	if(dbEnvAdopted_)
		dbEnv_->close(dbEnv_, 0);
	if (tempDbEnv_)
		tempDbEnv_->close(tempDbEnv_, 0);
	terminate();
	delete resolvers_;
	delete defaultQPR_;
	delete defaultCompression_;
}

void Manager::initialize(DB_ENV *env)
{
	// Initialize the global variables
	Globals::initialize(env);
}

void Manager::initTempDbEnv(DB_ENV *env)
{
	u_int32_t flags = DB_CREATE|DB_THREAD|DB_PRIVATE|DB_INIT_MPOOL;

	// Initialize environment for temporary
	// databases based on env's information.
	u_int32_t gbytes, bytes;
	int ncache;

	int ret = db_env_create(&tempDbEnv_, 0);
	if (ret)
		throw XmlException(ret);
	
	env->get_cachesize(env, &gbytes, &bytes, &ncache);

	// use 1/2 cache size
	u_int32_t nb = (bytes + (1024*1024*1024*gbytes)) / 2;
	tempDbEnv_->set_cachesize(tempDbEnv_, 0, nb, 1);

	// home, temp directories. Don't worry about data dirs, nothing
	// goes to disk for them
	const char *tdir, *home;
	env->get_home(env, &home);
	env->get_tmp_dir(env, &tdir);
	tempDbEnv_->set_tmp_dir(tempDbEnv_, tdir);
#ifdef DB_SET_ERRCALL
	tempDbEnv_->set_errcall(tempDbEnv_, errcall);
#endif

	u_int32_t mutexes;
	env->mutex_get_max(env, &mutexes);
	tempDbEnv_->mutex_set_max(tempDbEnv_, mutexes);

	// open it...
	ret = tempDbEnv_->open(tempDbEnv_, home, flags, 0);
	if (ret)
		throw XmlException(ret);

	ostringstream oss;
	oss << "Temporary database environment opened with " << nb << " bytes of cache";
	log(Log::C_MANAGER, Log::L_INFO, oss);
}

void Manager::terminate()
{
	// terminate the global variables
	Globals::terminate();
}

 void Manager::setDefaultContainerConfig(const ContainerConfig &config)
{
 	lock();
 	defaultContainerConfig_ = config;
 	defaultContainerConfig_.setManager(this);
 	unlock();
}

ContainerConfig &Manager::getDefaultContainerConfig()
{
 	return defaultContainerConfig_;
}

XmlContainer Manager::openContainer(
	const std::string &name, Transaction *txn, 
	const ContainerConfig &config, 
	bool doVersionCheck)
{
	if(config.getDbOpenFlags() & ~(DB_CREATE|DB_READ_UNCOMMITTED|
		   DB_EXCL|DB_NOMMAP|DB_RDONLY|DB_THREAD|DB_MULTIVERSION)) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Invalid flags to method XmlManager::openContainer");
	}
	if(config.getXmlFlags() & ~(DBXML_CHKSUM|
		   DBXML_ENCRYPT|DBXML_INDEX_NODES|DBXML_ALLOW_VALIDATION|
		   DBXML_STATISTICS|DBXML_NO_STATISTICS|DBXML_NO_INDEX_NODES|
		   DBXML_TRANSACTIONAL)) {
		throw XmlException(
			XmlException::INVALID_VALUE,
			"Invalid flags to method XmlManager::openContainer");
	}
	XmlContainer cont = openContainers_.findContainer(
		*this, name, txn, config,
		doVersionCheck);
	return cont;
}

Document *Manager::createDocument()
{
	return new Document(*this);
}

Transaction *Manager::createTransaction(DB_TXN *toUse)
{
	if (!toUse)
		throw XmlException(XmlException::INVALID_VALUE,
				   "XmlManager::createTransaction(DbTxn*) requires a valid DB_TXN object");

	if (isTransactedEnv() || isCDBEnv())
		return new Transaction(*this, toUse);
	
	throw XmlException(XmlException::INVALID_VALUE,
			   "Cannot call XmlManager::createTransaction when transactions are not initialized");
}

Transaction *Manager::createTransaction(u_int32_t flags)
{
	checkFlags(Log::misc_flag_info, "createTransaction()", flags,
		   DB_READ_UNCOMMITTED|DB_TXN_NOSYNC|DB_TXN_NOWAIT|
		   DB_TXN_SYNC|DB_READ_COMMITTED|DB_TXN_SNAPSHOT);
	if (isTransactedEnv())
		return new Transaction(*this, flags);
	throw XmlException(XmlException::INVALID_VALUE,
			   "Cannot call XmlManager::createTransaction when transactions are not initialized");
}

XmlContainer Manager::getOpenContainer(const std::string &name)
{
	XmlContainer cont = openContainers_.findOpenContainer(name);
	return cont;
}

ContainerBase *Manager::getContainerFromID(int id, bool acquire) const
{
	return openContainers_.getContainerFromID(id, acquire);
}

void Manager::removeContainer(Transaction *txn, const std::string &name)
{
	int err = dbEnv_->dbremove(dbEnv_, Transaction::toDB_TXN(txn), name.c_str(), 0, 0);
	if (err) {
		if (err == ENOENT)
			throwContainerNotFound(name);
		else
			throw XmlException(err);
	} else {
		ostringstream oss;
		oss << "Container '" << name << "' removed.";
		log(C_CONTAINER, L_DEBUG, oss);
	}
}

void Manager::renameContainer(Transaction *txn, const std::string &oldName,
			      const std::string &newName)
{
	int err = dbEnv_->dbrename(dbEnv_, Transaction::toDB_TXN(txn),
				       oldName.c_str(), 0, newName.c_str(), 0);
	if (err) {
		if (err == ENOENT)
			throwContainerNotFound(oldName);
		else
			throw XmlException(err);
	} else {
		ostringstream oss;
		oss << "Container '" << oldName <<
			"' renamed to '" << newName << "'.";
		log(C_CONTAINER, L_DEBUG, oss);
	}
}

void Manager::checkFlags(const FlagInfo *flag_info, const char *function,
			 u_int32_t flags, u_int32_t mask) const
{
	Log::checkFlags(function, flag_info, flags, mask);
}

void Manager::log(ImplLogCategory c, ImplLogLevel l,
		   const ostringstream &s) const
{
	Log::log(getDB_ENV(), c, l, s.str().c_str());
}

void Manager::log(ImplLogCategory c, ImplLogLevel l, const string &s) const
{
	Log::log(getDB_ENV(), c, l, s.c_str());
}

XmlInputStream *Manager::createLocalFileInputStream(const std::string &filename) const {
	XmlInputStream *newStream = 0;
	try {
		newStream = new LocalFileInputStream(filename);
	}
	catch (...) {
		ostringstream s;
		s << "Failed to create FileInputStream for file: ";
		s << filename;
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str());
	}
	return newStream;
}

XmlInputStream *Manager::createMemBufInputStream(
	const char *srcDocBytes,
	const unsigned int byteCount,
	const char *const bufId,
	bool adoptBuffer) const {
	XmlInputStream *newStream = 0;
	try {
		newStream = new MemBufInputStream(srcDocBytes,
						  byteCount, bufId,
						  adoptBuffer);
	}
	catch (...) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Failed to create MemBufInputStream");
	}
	return newStream;
}

// always adopts buffer
XmlInputStream *Manager::createMemBufInputStream(
	const char *srcDocBytes,
	const unsigned int byteCount,
	bool copyBuffer) const {
	XmlInputStream *newStream = 0;
	try {
		const char *buf = srcDocBytes;
		if (copyBuffer) {
			char *newBytes = new char[byteCount];
			if (!newBytes) {
				throw XmlException(
					XmlException::NO_MEMORY_ERROR,
					"Failed to allocate memory");
			}
			memcpy(newBytes, srcDocBytes, byteCount);
			buf = (const char *)newBytes;
		}
		newStream = new MemBufInputStream(buf,
						  byteCount, "",
						  (buf != srcDocBytes));
	}
	catch (...) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Failed to create MemBufInputStream");
	}
	return newStream;
}

XmlInputStream *Manager::createURLInputStream(const std::string &baseId, 
					      const std::string &systemId, 
					      const std::string &publicId) const
{
	XmlInputStream *newStream = 0;
	try {
		newStream = new URLInputStream(baseId, systemId, publicId);
	}
	catch (...) {
		ostringstream s;
		s << "Failed to create URLInputStream, baseId: ";
		s << baseId;
		s << ", systemId: ";
		s << systemId;
		s << ", publicId: ";
		s << publicId;
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str());
	}
	return newStream;
}

XmlInputStream *Manager::createURLInputStream(const std::string &baseId, 
					      const std::string &systemId) const
{
	XmlInputStream *newStream = 0;
	try {
		newStream = new URLInputStream(baseId, systemId);
	}
	catch (...) {
		ostringstream s;
		s << "Failed to create URLInputStream, baseId: ";
		s << baseId;
		s << ", systemId: ";
		s << systemId;
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str());
	}
	return newStream;
}

//
// Container store has a map of string name to id, where the
// integer id is an index into a vector of container objects
// This allows both identification of containers by id and
// detecting closed containers.
// Ids are never reused during the lifetime of an XmlManager (for now)
//
Manager::ContainerStore::ContainerStore()
	: mutex_(MutexLock::createMutex())
{
}

Manager::ContainerStore::~ContainerStore()
{
	MutexLock::destroyMutex(mutex_);
	// delete ContainerBase created in initialize()
	if (containers_.size() != 0) {
		ContainerBase *cb = containers_[0];
		if (cb)
			cb->release();
	}
}

void Manager::ContainerStore::initialize(Manager &mgr)
{
	// id 0 is the "transient" container for constructed docs
	ContainerBase *cb = new ContainerBase(mgr);
	cb->acquire();
	int id = insertNewContainer(cb);
	DBXML_ASSERT(id == 0);
}

bool Manager::ContainerStore::closeContainer(ContainerBase *container,
					     u_int32_t flags)
{
	bool didClose = false;
	MutexLock lock(mutex_);
	// remove name
	int id = container->getContainerID();
	container->lock(); // protect call to isReferenced()
	if (id > 0 && !container->isReferenced()) {
		container->unlock();
		didClose = true;
		// remove all references to the container (all aliases)
		ContainerIDMap::iterator i = idmap_.begin();
		while (i != idmap_.end()) {
			if (id == i->second) {
				idmap_.erase(i);
				i = idmap_.begin();
			} else
				*i++;
		}
		DBXML_ASSERT(containers_[id] == 0 ||
			     containers_[id]->getContainerID() == id);
		containers_[id] = 0; // "unuse" id
	}
	return didClose;
}

bool Manager::ContainerStore::addAlias(const std::string &alias,
				       ContainerBase *container)
{
	MutexLock lock(mutex_);

	ContainerIDMap::iterator i = idmap_.find(alias);
	if(i == idmap_.end()) {
		idmap_[alias] = container->getContainerID();
		return true;
	}
	return false;
}

// only remove if the name matches the container
bool Manager::ContainerStore::removeAlias(const std::string &alias,
					  ContainerBase *container)
{
	MutexLock lock(mutex_);

	int id = container->getContainerID();
	ContainerIDMap::iterator i = idmap_.find(alias);
	if(i != idmap_.end() && id == i->second) {
		idmap_.erase(i);
		return true;
	}
	return false;
}

int Manager::ContainerStore::insertNewContainer(ContainerBase *container)
{
	// Mutex is locked by caller
	int id = 0;

#ifdef MANAGER_REUSE_IDS
	for (int i = 1; i < (int) containers_.size(); i++) {
		if (containers_[i] == 0) {
			id = i;
			containers_[i] = container;
			break;
		}
	}
#endif
	if (id == 0) {
		containers_.push_back(container);
		id = (int)containers_.size() - 1;
	}

	idmap_[container->getName()] = id;
	return id;
}

XmlContainer Manager::ContainerStore::findContainer(
	Manager &mgr, const std::string &name, Transaction *txn,
	const ContainerConfig &config, bool doVersionCheck)
{
	MutexLock lock(mutex_);
	
	int id = 0;
	ContainerIDMap::iterator i = idmap_.find(name);
	if(i != idmap_.end()) {
		
		// Found one already open
		id = i->second;
		ContainerBase *result = containers_[id];
		DBXML_ASSERT(result && result->getContainer());
		XmlContainer ret(result->getContainer());
		return ret;
	} else {
		// Have to open one ourselves...
		Container *cresult = new Container(
			mgr, name, txn, config, doVersionCheck);
		int newId = insertNewContainer(cresult);
		cresult->setContainerID(newId);
		
		if (cresult && mgr.isCDBEnv())
			cresult->setUseCDB();
		XmlContainer ret(cresult);
		return ret;
	}
	DBXML_ASSERT(false);
	// Never happens
	return XmlContainer();
}

ContainerBase *Manager::ContainerStore::getContainerFromID(
	int id,
	bool acquire) const
{
	if (id < 0)
		return 0;
	DBXML_ASSERT(id < (int)containers_.size());
	MutexLock lock(mutex_);
	ContainerBase *ret = 0;
	ret = containers_[id];
	if (ret && acquire)
		ret->acquire();
	return ret;
}

XmlContainer Manager::ContainerStore::findOpenContainer(
	const std::string &name)
{
	XmlContainer result;
	MutexLock lock(mutex_);

	ContainerIDMap::iterator i = idmap_.find(name);
	if(i != idmap_.end()) {
		// Found one already open
		result = (containers_[i->second])->getContainer();
		DBXML_ASSERT(!result.isNull());
	}
	return result;
}

void Manager::ContainerStore::releaseRegisteredContainers()
{
	MutexLock lock(mutex_);
	idmap_.clear();
}

// The Manager's Dictionary uses a mutex to protect its state
// because:
//  1. it's not transactional
//  2. it can be used during read-only operations, such as queries, to
//  define new names for transient/constructed XML
// Dictionary will be thread-safe if tempDbEnv_ is (which it is)
//
DictionaryDatabase *Manager::getDictionary()
{
	if (!dictionary_) {
		ContainerConfig config;
		config.setAllowCreate(true);
		dictionary_ = new DictionaryDatabase(
			tempDbEnv_,
			0, // no txn
			"", // no name
			config,
			true // useMutex to protect access
			);
	}
	return dictionary_;
}

//
// temporary/cache database management implementation
//

RecordDatabase *Manager::createRecordDatabase()
{
	return new RecordDatabase(tempDbEnv_);
}

DocDatabase *Manager::createDocDatabase(CacheDatabaseCompareFcn compare)
{
	return new DocDatabase(tempDbEnv_, compare);
}

void Manager::registerCompression(const char *name, XmlCompression *compression)
{
 	MutexLock lock(mutex_);
 	std::string compName(name);
 	std::map<std::string, XmlCompression*>::iterator itr = compressionStore_.find(compName);
 	if(itr != compressionStore_.end())
 		throw XmlException(XmlException::INVALID_VALUE,
 		"Cannot register compression named: " + compName + " because the name is already registered.");
 	compressionStore_[compName] = compression;
}
 
XmlCompression *Manager::getCompression(std::string &name)
{
 	MutexLock lock(mutex_);
 	std::map<std::string, XmlCompression*>::iterator itr = compressionStore_.find(name);
 	if(itr == compressionStore_.end())
 		return NULL;
 	return itr->second;
}

int Manager::getImplicitTimezone() const
{
	return timezone_;
}

void Manager::setImplicitTimezone(int tz)
{
	if(abs(tz) > 14 * 60 * 60) {
		throw XmlException(XmlException::INVALID_VALUE,
			"Invalid timezone, valid values are between +840 minutes (+14 hours) and -840 minutes (-14 hours)");
	}
		
	timezone_ = tz;
}

docId_t Manager::allocateTempDocID()
{
	lock();
	docId_t ret = tempDocId_++;
	// don't allow doc ID of 0
	if (tempDocId_ == 0)
		tempDocId_++;
	unlock();
	return ret;
}
