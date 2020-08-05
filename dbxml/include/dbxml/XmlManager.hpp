//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __XMLMANAGER_HPP
#define	__XMLMANAGER_HPP

#include <string>
#include <iostream>

#include "DbXmlFwd.hpp"
#include "XmlContainer.hpp"
#include "XmlQueryContext.hpp"
#include "XmlIndexLookup.hpp"
#include "XmlContainerConfig.hpp"
#include "XmlCompression.hpp"
#include "db.h"

namespace DbXml
{

class Manager;

class DBXML_EXPORT XmlManager
{
public:
	/** @name Public Constructors */
	/**
	 * It's assumed that the enviroment is already open. If adopted, the object
	 * will be both closed and deleted by the XmlManager when it is
	 * destroyed.  The environment is adopted if flags include DBXML_ADOPT_DBENV.
	 *
	 * Valid flags: DBXML_ADOPT_DBENV, DBXML_ALLOW_EXTERNAL_ACCESS,
	 *  DBXML_ALLOW_AUTO_OPEN
	 */
	XmlManager(DB_ENV *env, u_int32_t flags = 0);
	
	/**
	 * Valid flags: DBXML_ALLOW_EXTERNAL_ACCESS, DBXML_ALLOW_AUTO_OPEN
	 */
	XmlManager(u_int32_t flags);
	/**
	 * Default state: no external access, no auto open
	 */
	XmlManager();

	/** @name For Reference Counting */
	XmlManager(const XmlManager &o);
	XmlManager &operator=(const XmlManager &o);
	~XmlManager();

	/**
	 * get flags used to construct XmlManager
	 */
	u_int32_t getFlags() const;
	/** @name XmlManager Configuration for XmlContainer construction */
	/**
	 * Flags for containers:
	 * DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DBXML_CHKSUM, DBXML_ENCRYPT,
	 * DBXML_ALLOW_VALIDATION, DBXML_STATISTICS, DBXML_NO_STATISTICS,
	 * DB_READ_UNCOMMITTED, DB_NOMMAP, DB_THREAD, DB_CREATE, DB_EXCL,
	 * DB_RDONLY, DB_TXN_NOT_DURABLE, DB_MULTIVERSION
	 */
	void setDefaultContainerFlags(const XmlContainerConfig &flags);
	XmlContainerConfig getDefaultContainerFlags() const;
	XmlContainerConfig getDefaultContainerConfig();
	void setDefaultContainerConfig(XmlContainerConfig &config);
	// Berkeley DB page size for containers
	void setDefaultPageSize(u_int32_t pageSize);
	u_int32_t getDefaultPageSize() const;
	// Berkeley DB unique number increment
	void setDefaultSequenceIncrement(u_int32_t incr);
	u_int32_t getDefaultSequenceIncrement() const;
	// Container type -- defaults to XmlContainer::WholedocContainer
	void setDefaultContainerType(XmlContainer::ContainerType type);
	XmlContainer::ContainerType getDefaultContainerType() const;
	// Compression -- defaults to default_compression
	void setDefaultCompression(const char *name);
	const char *getDefaultCompression() const;

	/** @name For resolving external entities */
	void registerResolver(const XmlResolver &resolver);

	/** @name For using custom compression */
	void registerCompression(const char *name, XmlCompression &compression);

	/** @name For setting the implicit timezone */

	/// Implicit timezone as an offset in minutes from GMT
	int getImplicitTimezone() const;
	/// Implicit timezone as an offset in minutes from GMT
	void setImplicitTimezone(int tz);

	
	/** @name Container Methods */

	// returns 0 if not a container; otherwise returns format version
	int existsContainer(const std::string &name);
	
	/// Uses the default flags and ContainerType, implies DB_CREATE
	XmlContainer createContainer(const std::string &name);

	///  Uses XmlContainerConfig, but setAllowCreate(true) is set
	XmlContainer createContainer(const std::string &name, const XmlContainerConfig &config);
	/**
	 * Valid flags:
	 * DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DBXML_CHKSUM, DBXML_ENCRYPT,
	 * DBXML_ALLOW_VALIDATION, DBXML_STATISTICS, DBXML_NO_STATISTICS,
	 * DB_READ_UNCOMMITTED, DB_NOMMAP, DB_THREAD, DB_CREATE, DB_EXCL,
	 * DB_RDONLY, DB_TXN_NOT_DURABLE, DB_MULTIVERSION
	 */
	XmlContainer createContainer(
		const std::string &name, const XmlContainerConfig &flags,
		XmlContainer::ContainerType type,
		int mode = 0);
	/// Uses the default flags
	XmlContainer openContainer(const std::string &name);

	///  Uses XmlContainerConfig
	XmlContainer openContainer(const std::string &name, const XmlContainerConfig &config);

	/**
	 * Valid flags:
	 * DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DBXML_CHKSUM, DBXML_ENCRYPT,
	 * DBXML_ALLOW_VALIDATION, DBXML_STATISTICS, DBXML_NO_STATISTICS,
	 * DB_READ_UNCOMMITTED, DB_NOMMAP, DB_THREAD, DB_CREATE, DB_EXCL,
	 * DB_RDONLY, DB_TXN_NOT_DURABLE, DB_MULTIVERSION
	 */
	XmlContainer openContainer(const std::string &name,
				   const XmlContainerConfig &flags,
				   XmlContainer::ContainerType type,
				   int mode = 0);
	
	
	/** @name Factory methods for XmlInputStream classes */
	
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createStdInInputStream() const;
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createLocalFileInputStream(
		const std::string &filename) const;
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createMemBufInputStream(
		const char *srcDocBytes,
		const unsigned int byteCount,
		const char *const bufId,
		const bool adoptBuffer = false) const;

	/// XmlInputStream must be deleted by user.
	XmlInputStream *createMemBufInputStream(
		const char *srcDocBytes,
		const unsigned int byteCount,
		const bool copyBuffer) const;
	
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createURLInputStream(const std::string &baseId, 
					    const std::string &systemId, 
					    const std::string &publicId) const;
	
	/// XmlInputStream must be deleted by user.
	XmlInputStream *createURLInputStream(const std::string &baseId, 
					    const std::string &systemId) const;
	
	
	/** @name Container Methods (Transacted) */
	
	/// Uses the default flags, implies DB_CREATE
	XmlContainer createContainer(XmlTransaction &txn,
				     const std::string &name);

	/// Uses the default flags, implies DB_CREATE
	XmlContainer createContainer(XmlTransaction &txn,
				     const std::string &name,
				     const XmlContainerConfig &config);
	/**
	 * Valid flags:
	 * DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DBXML_CHKSUM, DBXML_ENCRYPT,
	 * DBXML_ALLOW_VALIDATION, DBXML_STATISTICS, DBXML_NO_STATISTICS,
	 * DB_READ_UNCOMMITTED, DB_NOMMAP, DB_THREAD, DB_CREATE, DB_EXCL,
	 * DB_RDONLY, DB_TXN_NOT_DURABLE, DB_MULTIVERSION
	 */
	XmlContainer createContainer(XmlTransaction &txn,
				     const std::string &name, 
				     const XmlContainerConfig &flags,
				     XmlContainer::ContainerType type,
				     int mode = 0);
	/// Uses the default flags
	XmlContainer openContainer(XmlTransaction &txn,
				   const std::string &name);

	/// Uses the default flags
	XmlContainer openContainer(XmlTransaction &txn,
				   const std::string &name,
				   const XmlContainerConfig &config);

	/**
	 * Valid flags:
	 * DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DBXML_CHKSUM, DBXML_ENCRYPT,
	 * DBXML_ALLOW_VALIDATION, DBXML_STATISTICS, DBXML_NO_STATISTICS,
	 * DB_READ_UNCOMMITTED, DB_NOMMAP, DB_THREAD, DB_CREATE, DB_EXCL,
	 * DB_RDONLY, DB_TXN_NOT_DURABLE, DB_MULTIVERSION
	 */
	XmlContainer openContainer(XmlTransaction &txn,
				   const std::string &name,
				   const XmlContainerConfig &flags,
				   XmlContainer::ContainerType type,
				   int mode = 0);
	
	/** @name Container Administration Methods */

	void removeContainer(const std::string &name);
	void renameContainer(const std::string &oldName,
			     const std::string &newName);
	void dumpContainer(const std::string &name,
			   std::ostream *out);
	void loadContainer(const std::string &name,
			   std::istream *in, unsigned long *lineno,
			   XmlUpdateContext &context);
	/**
	 * Valid flags:
	 * DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DBXML_STATISTICS, DBXML_NO_STATISTICS
	 */
	void reindexContainer(const std::string &name,
			      XmlUpdateContext &context);

	void reindexContainer(const std::string &name,
			      XmlUpdateContext &context,
			      const XmlContainerConfig &flags);

	/* flags is currently unused */
	void truncateContainer(const std::string &name,
			       XmlUpdateContext &context);

	void truncateContainer(const std::string &name,
			       XmlUpdateContext &context,
			       const XmlContainerConfig &flags);

	/* flags is currently unused */
	void compactContainer(const std::string &name,
			      XmlUpdateContext &context);

	void compactContainer(const std::string &name,
			      XmlUpdateContext &context,
			      const XmlContainerConfig &flags);

	/**
	 * Valid flags:
	 * DB_SALVAGE, DB_AGGRESSIVE
	 */
	void verifyContainer(const std::string &name,
			     std::ostream *out, u_int32_t flags = 0);
	void upgradeContainer(const std::string &name,
			      XmlUpdateContext &context);

	/** @name Container Administration Methods (Transacted) */

	void removeContainer(XmlTransaction &txn, const std::string &name);
	void renameContainer(XmlTransaction &txn, const std::string &oldName,
			     const std::string &newName);
	/**
	 * Valid flags:
	 * DBXML_INDEX_NODES, DBXML_NO_INDEX_NODES, DBXML_STATISTICS, DBXML_NO_STATISTICS
	 */
	void reindexContainer(XmlTransaction &txn,
			      const std::string &name,
			      XmlUpdateContext &context);

	void reindexContainer(XmlTransaction &txn,
			      const std::string &name,
			      XmlUpdateContext &context,
			      const XmlContainerConfig &flags);

	/* flags is currently unused */
	void truncateContainer(XmlTransaction &txn,
			       const std::string &name,
			       XmlUpdateContext &context);

	void truncateContainer(XmlTransaction &txn,
			       const std::string &name,
			       XmlUpdateContext &context,
			       const XmlContainerConfig &flags);

	/* flags is currently unused */
	void compactContainer(XmlTransaction &txn,
			      const std::string &name,
			      XmlUpdateContext &context);

	void compactContainer(XmlTransaction &txn,
			      const std::string &name,
			      XmlUpdateContext &context,
			      const XmlContainerConfig &flags);

	/** @name Document Methods */
	
	XmlDocument createDocument();
	
	/** @name Context Methods */
	
	XmlQueryContext createQueryContext(
		XmlQueryContext::ReturnType rt = XmlQueryContext::LiveValues,
		XmlQueryContext::EvaluationType et = XmlQueryContext::Eager);
	XmlUpdateContext createUpdateContext();

	/** @name Query Expression Methods */

	XmlQueryExpression prepare(const std::string &queryString,
				   XmlQueryContext &context);
	XmlQueryExpression prepare(XmlTransaction &txn,
				   const std::string &queryString,
				   XmlQueryContext &context);
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT, DBXML_DOCUMENT_PROJECTION
	 */
	XmlResults query(const std::string &queryString,
			 XmlQueryContext &context, u_int32_t flags = 0);
	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_RMW, DB_READ_COMMITTED, DBXML_LAZY_DOCS,
	 * DB_TXN_SNAPSHOT, DBXML_DOCUMENT_PROJECTION
	 */
	XmlResults query(XmlTransaction &txn, const std::string &queryString,
			 XmlQueryContext &context, u_int32_t flags = 0);

	/** @name Results Methods */
	XmlResults createResults();
	
	/** @name Transaction Methods */
	XmlTransaction createTransaction(DB_TXN *toUse);

	/**
	 * Valid flags:
	 * DB_READ_UNCOMMITTED, DB_TXN_NOSYNC, DB_TXN_NOWAIT,
	 * DB_READ_COMMITTED, DB_TXN_SYNC, DB_TXN_SNAPSHOT
	 */
	XmlTransaction createTransaction(u_int32_t flags = 0);

	/** @name Direct Index Lookup */

	XmlIndexLookup createIndexLookup(
		XmlContainer &container,
		const std::string &uri, const std::string &name,
		const std::string &index,
		const XmlValue &value = XmlValue(),
		XmlIndexLookup::Operation op = XmlIndexLookup::EQ);
	
	/** @name Berkeley DB Compatibility */
	DB_ENV *getDB_ENV();
	const std::string &getHome() const;
	
	
	/** @name Private Methods (for internal use) */
	// @{

	XmlManager(Manager &mgr);
	operator Manager &() {
		return *impl_;
	}
	operator const Manager &() const {
		return *impl_;
	}
	operator Manager *() {
		return impl_;
	}
	
	//@}

private:
	Manager *impl_;
};

}

#endif
