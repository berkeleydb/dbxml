//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __MANAGER_HPP
#define __MANAGER_HPP

#include <string>
#include <map>

#include "ReferenceCounted.hpp"
#include "Container.hpp"
#include "Globals.hpp"
#include "Log.hpp"
#include "DocID.hpp"
#include "ContainerConfig.hpp"
#include "CacheDatabase.hpp"

namespace DbXml
{

class Container;
class ContainerBase;
class Transaction;
class ResolverStore;
class DictionaryDatabase;
class Document;
class QueryPlanRoot;
class ContainerConfig;
	
class Manager : public Log, public ReferenceCountedProtected
{
public:
	/**
	 * It's assumed that the DbEnv is already open. If adopted, the DbEnv
	 * will be both closed and deleted by the XmlDocument when it is
	 * destroyed.
	 */
	Manager(DB_ENV *dbEnv, u_int32_t flags = 0);
	Manager(u_int32_t flags);
	~Manager();


	//
	// Timezone methods
	//

	/// Timezone is measured as an offset in seconds
	int getImplicitTimezone() const;

	/// Timezone is measured as an offset in seconds
	void setImplicitTimezone(int tz);

	//
	// Resolver methods
	//

	ResolverStore &getResolverStore() {
		return *resolvers_;
	}

 	//
 	// Compression methods
 	//
 
 	void registerCompression(const char *name, XmlCompression *compression);
 	XmlCompression *getCompression(std::string &name);

	u_int32_t getFlags() const {
		return flags_;
	}
	
	//
	// Container
	//

	void setDefaultContainerConfig(const ContainerConfig &config);
	ContainerConfig &getDefaultContainerConfig();

	void setDefaultContainerFlags(const ContainerConfig &flags) {
		defaultContainerConfig_.setFlags(flags);
	}

	void setDefaultPageSize(u_int32_t pageSize) {
		defaultContainerConfig_.setPageSize(pageSize);
	}
	u_int32_t getDefaultPageSize() const { return defaultContainerConfig_.getPageSize(); }

	void setDefaultSequenceIncrement(u_int32_t incr) {
		defaultContainerConfig_.setSequenceIncrement(incr);
	}
	u_int32_t getDefaultSequenceIncrement() const { return defaultContainerConfig_.getSequenceIncrement(); }

	XmlContainer::ContainerType getDefaultContainerType() const {
		return defaultContainerConfig_.getContainerType();
	}
	void setDefaultContainerType(XmlContainer::ContainerType type) {
		defaultContainerConfig_.setContainerType(type);
	}

	void setDefaultCompression(const char *name) {
		defaultContainerConfig_.setCompressionName(name);
	}
	const char *getDefaultCompression() const {
		return defaultContainerConfig_.getCompressionName();
	}

	XmlContainer openContainer(const std::string &name,
				   Transaction *txn, 
				   const ContainerConfig &config,
				   bool doVersionCheck = true);
	XmlContainer getOpenContainer(const std::string &name);

	// factory methods for XmlManager
	Document *createDocument();
	Transaction *createTransaction(DB_TXN *toUse);
	Transaction *createTransaction(u_int32_t flags = 0);
	XmlInputStream *createLocalFileInputStream(
		const std::string &filename) const;
	XmlInputStream *createMemBufInputStream(
		const char *srcDocBytes,
		const unsigned int byteCount,
		const char *const bufId,
		bool adoptBuffer = false) const;

	XmlInputStream *createMemBufInputStream(
		const char *srcDocBytes,
		const unsigned int byteCount,
		bool copyBuffer) const;
	XmlInputStream *createURLInputStream(const std::string &baseId, 
					    const std::string &systemId, 
					    const std::string &publicId) const;
	XmlInputStream *createURLInputStream(const std::string &baseId, 
					    const std::string &systemId) const;
	

	void removeContainer(Transaction *txn, const std::string &name);
	void renameContainer(Transaction *txn, const std::string &oldName,
			     const std::string &newName);

	QueryPlanRoot *getDefaultQueryPlanRoot() const { return defaultQPR_; }
	ContainerBase *getContainerFromID(int id, bool acquire) const;
	
	DB_ENV *getDB_ENV() const { return dbEnv_; }

	const std::string &getHome() const { return dbHome_; }

	bool isTransactedEnv() const { return ((dbEnvOpenFlags_ & DB_INIT_TXN) != 0); }
	bool isLockingEnv() const { return ((dbEnvOpenFlags_ & DB_INIT_LOCK) != 0); }
	bool isCDBEnv() const { return ((dbEnvOpenFlags_ & DB_INIT_CDB) != 0); }
	bool allowAutoOpen() const { return autoOpen_; }

	//
	// Logging
	//
	virtual void log(ImplLogCategory c, ImplLogLevel l,
			 const std::ostringstream &s) const;
	virtual void log(ImplLogCategory c, ImplLogLevel l,
			 const std::string &s) const;
	virtual void checkFlags(const FlagInfo *flag_info,
				const char *function,
				u_int32_t flags, u_int32_t mask) const;

	// create/get in-memory dictionary for handling
	// transient document parsing/querying.  Once created,
	// this remains open for the lifetime of the Manager
	DictionaryDatabase *getDictionary();

	//
	// Cache Database Management (if it gets complex, it could
	// be moved to a separate class).  Objects returned are
	// owned by the caller.
	//
	RecordDatabase *createRecordDatabase();
	DocDatabase *createDocDatabase(CacheDatabaseCompareFcn compare = 0);
	docId_t allocateTempDocID();
	
	//
	// Is the current host big-endian?  DB XML's "native"
	// format is little-endian, as of release 2.0
	//
	static bool isBigendian() { return Globals::isBigendian_; }

private:
	Manager(const Manager &o);
	Manager &operator=(const Manager &o);
	void initialize(DB_ENV *env);
	void terminate();
	void initTempDbEnv(DB_ENV *env);
	void close(); // used by dtor and failure on ctor
	
	class ContainerStore {
	public:
		ContainerStore();
		~ContainerStore();
		void initialize(Manager &mgr);

		bool closeContainer(ContainerBase *container,
				    u_int32_t flags);

		// Container ID '0' is only a ContainerBase, but
		// will never be looked up via either of these methods
		XmlContainer findContainer(
			Manager &db,
			const std::string &name,
			Transaction *txn,
			const ContainerConfig &config,
			bool doVersionCheck);

		XmlContainer findOpenContainer(
			const std::string &name);

		// This method may leave open some races/holes where
		// a container could get deleted out from under an
		// object (see RawNodeValue.cpp).  Keep an eye on this.
		ContainerBase *getContainerFromID(int id,
						  bool acquire) const;

		void releaseRegisteredContainers();
		bool addAlias(const std::string &alias,
			      ContainerBase *container);
		bool removeAlias(const std::string &alias,
				 ContainerBase *container);
	private:
		int insertNewContainer(ContainerBase *container);

		
		typedef std::vector<ContainerBase *> ContainerVector;
		typedef std::map<std::string, int> ContainerIDMap;
		dbxml_mutex_t mutex_;
		ContainerIDMap idmap_;
		ContainerVector containers_;
	};

	DB_ENV *dbEnv_;
	bool dbEnvAdopted_;
	DB_ENV *tempDbEnv_;
	u_int32_t dbEnvOpenFlags_;
	u_int32_t flags_;
	bool autoOpen_;
	std::string dbHome_;

	ContainerConfig defaultContainerConfig_;
	ResolverStore *resolvers_;
	int timezone_;

	ContainerStore openContainers_;
	DictionaryDatabase *dictionary_;
	docId_t tempDocId_;

	QueryPlanRoot *defaultQPR_;

        std::map<std::string, XmlCompression*> compressionStore_;
 	XmlCompression *defaultCompression_;
	
	friend class XmlContainer;
	friend class Container;
};

}

#endif
