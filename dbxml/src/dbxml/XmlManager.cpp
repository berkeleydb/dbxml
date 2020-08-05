//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "DbXmlInternal.hpp"
#include "dbxml/XmlManager.hpp"
#include "dbxml/XmlException.hpp"
#include "dbxml/XmlResults.hpp"
#include "dbxml/XmlTransaction.hpp"
#include "dbxml/XmlCompression.hpp"
#include "dbxml/XmlContainerConfig.hpp"
#include "Container.hpp"
#include "UTF8.hpp"
#include "Manager.hpp"
#include "QueryExpression.hpp"
#include "StdInInputStream.hpp"
#include "Transaction.hpp"
#include "Document.hpp"
#include "QueryContext.hpp"
#include "IndexLookup.hpp"
#include "dbxml/XmlUpdateContext.hpp"
#include "UpdateContext.hpp"
#include "Results.hpp"
#include "dataItem/DbXmlURIResolver.hpp"

#include <xqilla/utils/XQillaPlatformUtils.hpp>
#include <xqilla/exceptions/XQillaException.hpp>

#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

using namespace DbXml;
using namespace std;

// Counters
#define INCR(ctr) Globals::incrementCounter(ctr)

#ifdef DEBUG
static void dbFlagsCollision(const char *flag)
{
	std::string msg = "Collision with DB flags: " + (std::string)flag;
//	throw XmlException(XmlException::INTERNAL_ERROR, msg);
	std::cout << msg << std::endl;
}

#endif

XmlManager::XmlManager(DB_ENV *env, u_int32_t flags)
	: impl_(new Manager(env, flags))
{
#ifdef DEBUG
	int ncollisions = 0;
	// Do sanity check on potential DB flags collisions.
	// This will only ever fail if DB flags are changed
	// in a way that collides with DBXML_ flags
	// New DBXML_ and DB_ flags should be added here as appropriate
	int dbTxnFlags = DB_READ_COMMITTED|DB_READ_UNCOMMITTED|DB_MULTIVERSION|DB_TXN_SNAPSHOT|DB_TXN_SYNC|DB_TXN_NOWAIT|DB_RMW;
	int dbDbFlags = DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|DB_CREATE|DB_EXCL|DB_RDONLY|DB_TXN_NOT_DURABLE|DB_MULTIVERSION|DB_TRUNCATE;
	int dbXmlFlags = DBXML_TRANSACTIONAL|DBXML_ALLOW_VALIDATION|DBXML_INDEX_NODES|DBXML_NO_INDEX_NODES|DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_STATISTICS|DBXML_NO_STATISTICS;
	if ((DBXML_LAZY_DOCS & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_LAZY_DOCS");
		++ncollisions;
	}
	if ((DBXML_REVERSE_ORDER & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_REVERSE_ORDER");
		++ncollisions;
	}
	if ((DBXML_INDEX_VALUES & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_INDEX_VALUES");
		++ncollisions;
	}
	if ((DBXML_NO_INDEX_NODES & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_NO_INDEX_NODES");
		++ncollisions;
	}
	if ((DBXML_CACHE_DOCUMENTS & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_CACHE_DOCUMENTS");
		++ncollisions;
	}
	if ((DBXML_WELL_FORMED_ONLY & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_WELL_FORMED_ONLY");
		++ncollisions;
	}
	if ((DBXML_DOCUMENT_PROJECTION & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_DOCUMENT_PROJECTION");
		++ncollisions;
	}
	if ((DBXML_NO_AUTO_COMMIT & dbTxnFlags) != 0) {
		dbFlagsCollision("DBXML_NO_AUTO_COMMIT");
		++ncollisions;
	}
	if ((dbDbFlags & dbXmlFlags) != 0) {
		dbFlagsCollision("MISC_FLAGS");
		++ncollisions;
	}
	if (ncollisions)
		throw XmlException(XmlException::INTERNAL_ERROR,
				   "One or more collisions with DB flags");
#endif
	impl_->acquire();
}

XmlManager::XmlManager(u_int32_t flags)
	: impl_(new Manager(flags))
{
	impl_->acquire();
}

XmlManager::XmlManager()
	: impl_(new Manager((u_int32_t)0))
{
	impl_->acquire();
}

XmlManager::~XmlManager()
{
	impl_->release();
}

XmlManager::XmlManager(Manager &mgr)
	: impl_(&mgr)
{
	impl_->acquire();
}

XmlManager::XmlManager(const XmlManager &o)
	: impl_(o.impl_)
{
	impl_->acquire();
}

XmlManager &XmlManager::operator=(const XmlManager &o)
{
	if(impl_ != o.impl_) {
		impl_->release();
		impl_ = o.impl_;
		impl_->acquire();
	}
	return *this;
}

DB_ENV *XmlManager::getDB_ENV()
{
	return impl_->getDB_ENV();
}

const std::string &XmlManager::getHome() const
{
	return impl_->getHome();
}

void XmlManager::setDefaultContainerFlags(const XmlContainerConfig &flags)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "setDefaultContainerFlags()",
		flags, DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_STATISTICS|DBXML_NO_STATISTICS|
		DBXML_CHKSUM|DBXML_ENCRYPT|DB_CREATE|DB_EXCL|
		DB_RDONLY|DBXML_ALLOW_VALIDATION|DB_TXN_NOT_DURABLE|
		DB_MULTIVERSION);
	impl_->setDefaultContainerFlags(flags);
}

XmlContainerConfig XmlManager::getDefaultContainerFlags() const
{
	return impl_->getDefaultContainerConfig();
}

XmlContainerConfig XmlManager::getDefaultContainerConfig()
{
	return impl_->getDefaultContainerConfig();
}

void XmlManager::setDefaultContainerConfig(XmlContainerConfig &config)
{
	impl_->setDefaultContainerConfig(config);
}

u_int32_t XmlManager::getFlags() const
{
	return impl_->getFlags();
}

void XmlManager::setDefaultPageSize(u_int32_t pageSize)
{
	impl_->setDefaultPageSize(pageSize);
}

u_int32_t XmlManager::getDefaultPageSize() const
{
	return impl_->getDefaultPageSize();
}

void XmlManager::setDefaultSequenceIncrement(u_int32_t incr)
{
	impl_->setDefaultSequenceIncrement(incr);
}

u_int32_t XmlManager::getDefaultSequenceIncrement() const
{
	return impl_->getDefaultSequenceIncrement();
}

void XmlManager::setDefaultContainerType(XmlContainer::ContainerType type)
{
	impl_->setDefaultContainerType(type);
}

XmlContainer::ContainerType XmlManager::getDefaultContainerType() const
{
	return impl_->getDefaultContainerType();
}

void XmlManager::setDefaultCompression(const char *name)
{
	impl_->setDefaultCompression(name);
}

const char *XmlManager::getDefaultCompression() const
{
	return impl_->getDefaultCompression();
}

void XmlManager::registerResolver(const XmlResolver &resolver)
{
	impl_->getResolverStore().registerResolver(resolver);
}

void XmlManager::registerCompression(const char *name, XmlCompression &compression)
{
	impl_->registerCompression(name, &compression);
}

int XmlManager::getImplicitTimezone() const
{
	// Convert from seconds based to minutes based
	return impl_->getImplicitTimezone() / 60;
}

void XmlManager::setImplicitTimezone(int tz)
{
	// Convert from minutes based to seconds based
	impl_->setImplicitTimezone(tz * 60);
}

int XmlManager::existsContainer(const std::string &name)
{
	return Container::checkContainer(name, impl_->getDB_ENV());
}

XmlContainer XmlManager::createContainer(const std::string &name)
{
	ContainerConfig config(impl_->getDefaultContainerConfig());
	config.setAllowCreate(true);
	config.setExclusiveCreate(true);
	return impl_->openContainer(name, 0, config);
}

XmlContainer XmlManager::createContainer(const std::string &name,
										 const XmlContainerConfig &config)
{
        ContainerConfig config2(*config);
	config2.setAllowCreate(true);
	config2.setExclusiveCreate(true);
	impl_->checkFlags(
		Log::open_container_flag_info, "createContainer()",
		config, DB_CREATE|
		DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DB_TXN_NOT_DURABLE|DB_EXCL|DB_MULTIVERSION|DBXML_NO_INDEX_NODES|
		DBXML_INDEX_NODES|DBXML_STATISTICS|DBXML_NO_STATISTICS|
		DBXML_TRANSACTIONAL|DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION);
	return impl_->openContainer(name, 0, config2);
}

XmlContainer XmlManager::createContainer(
	const std::string &name, const XmlContainerConfig &flags,
	XmlContainer::ContainerType type, int mode)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "createContainer()",
		flags, DB_CREATE|DBXML_NO_INDEX_NODES|
		DBXML_INDEX_NODES|
		DBXML_STATISTICS|DBXML_NO_STATISTICS|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_TXN_NOT_DURABLE|DB_EXCL|DB_MULTIVERSION);
	ContainerConfig config(*flags);
	config.setContainerType(type);
	config.setMode(mode);
	config.setPageSize(getDefaultPageSize());
	config.setSequenceIncrement(getDefaultSequenceIncrement());
	config.setCompressionName(getDefaultCompression());
	config.setAllowCreate(true);
	config.setExclusiveCreate(true);
	return impl_->openContainer(name, 0, config);
}

XmlContainer XmlManager::createContainer(
	XmlTransaction &txn, const std::string &name)
{
	ContainerConfig config(impl_->getDefaultContainerConfig());
	config.setAllowCreate(true);
	config.setExclusiveCreate(true);
	return impl_->openContainer(name, txn, config);
}

XmlContainer XmlManager::createContainer(
	XmlTransaction &txn, const std::string &name,
	const XmlContainerConfig &config)
{
	ContainerConfig config2(*config);  
	config2.setAllowCreate(true);
	config2.setExclusiveCreate(true);
	return impl_->openContainer(name, txn, config2);
}

XmlContainer XmlManager::createContainer(
	XmlTransaction &txn, const std::string &name,
	const XmlContainerConfig &flags, XmlContainer::ContainerType type, 
	int mode)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "createContainer()",
		flags, DB_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_STATISTICS|DBXML_NO_STATISTICS|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_TXN_NOT_DURABLE|DB_EXCL|DB_MULTIVERSION);
	ContainerConfig config(*flags);
	config.setContainerType(type);
	config.setMode(mode);
	config.setPageSize(getDefaultPageSize());
	config.setSequenceIncrement(getDefaultSequenceIncrement());
	config.setCompressionName(getDefaultCompression());
	config.setAllowCreate(true);
	config.setExclusiveCreate(true);
	return impl_->openContainer(name, txn, config);
}

XmlInputStream *XmlManager::createStdInInputStream() const {
	return new StdInInputStream();
}

XmlInputStream *XmlManager::createLocalFileInputStream(const std::string &filename) const {
	return impl_->createLocalFileInputStream(filename);
}

XmlInputStream *XmlManager::createMemBufInputStream(
	const char *srcDocBytes,
	const unsigned int byteCount,
	const char *const bufId,
	const bool adoptBuffer) const {
	return impl_->createMemBufInputStream(srcDocBytes, byteCount,
					      bufId, adoptBuffer);
}

// always adopts buffer
XmlInputStream *XmlManager::createMemBufInputStream(
	const char *srcDocBytes,
	const unsigned int byteCount,
	const bool copyBuffer) const {
	return impl_->createMemBufInputStream(srcDocBytes, byteCount,
					      copyBuffer);
}

XmlInputStream *XmlManager::createURLInputStream(const std::string &baseId, 
						 const std::string &systemId, 
						 const std::string &publicId) const
{
	return impl_->createURLInputStream(baseId, systemId, publicId);
}

XmlInputStream *XmlManager::createURLInputStream(const std::string &baseId, 
						 const std::string &systemId) const
{
	return impl_->createURLInputStream(baseId, systemId);
}

XmlContainer XmlManager::openContainer(const std::string &name)
{
	ContainerConfig config(impl_->getDefaultContainerConfig());
	return impl_->openContainer(name, 0, config);
}

XmlContainer XmlManager::openContainer(const std::string &name, const XmlContainerConfig &config)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "openContainer()",
		config, DB_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_STATISTICS|DBXML_NO_STATISTICS|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE|DB_MULTIVERSION);
	return impl_->openContainer(name, 0, config);
}

XmlContainer XmlManager::openContainer(const std::string &name,
				       const XmlContainerConfig &flags,
				       XmlContainer::ContainerType type,
				       int mode)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "openContainer()",
		flags, DB_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_STATISTICS|DBXML_NO_STATISTICS|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE|DB_MULTIVERSION);
	ContainerConfig config(*flags);
	config.setContainerType(type);
	config.setMode(mode);
	config.setPageSize(getDefaultPageSize());
	config.setSequenceIncrement(getDefaultSequenceIncrement());
	config.setCompressionName(getDefaultCompression());
	return impl_->openContainer(name, 0, config);
}

XmlContainer XmlManager::openContainer(XmlTransaction &txn,
				       const std::string &name,
				       const XmlContainerConfig &flags,
				       XmlContainer::ContainerType type,
				       int mode)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "openContainer()",
		flags, DB_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_STATISTICS|DBXML_NO_STATISTICS|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE|DB_MULTIVERSION);
	ContainerConfig config(*flags);
	config.setContainerType(type);
	config.setMode(mode);
	config.setPageSize(getDefaultPageSize());
	config.setSequenceIncrement(getDefaultSequenceIncrement());
	config.setCompressionName(getDefaultCompression());
	return impl_->openContainer(name, txn, config);
}

XmlContainer XmlManager::openContainer(XmlTransaction &txn, const std::string &name)
{
	ContainerConfig config(impl_->getDefaultContainerConfig());
	return impl_->openContainer(name, txn, config);
}

XmlContainer XmlManager::openContainer(XmlTransaction &txn, const std::string &name, const XmlContainerConfig &config)
{
	impl_->checkFlags(
		Log::open_container_flag_info, "openContainer()",
		config, DB_CREATE|DBXML_NO_INDEX_NODES|DBXML_INDEX_NODES|
		DBXML_STATISTICS|DBXML_NO_STATISTICS|
		DBXML_TRANSACTIONAL|DB_READ_UNCOMMITTED|DB_NOMMAP|DB_THREAD|
		DBXML_CHKSUM|DBXML_ENCRYPT|DBXML_ALLOW_VALIDATION|
		DB_RDONLY|DB_EXCL|DB_TXN_NOT_DURABLE|DB_MULTIVERSION);
	return impl_->openContainer(name, txn, config);
}

XmlDocument XmlManager::createDocument()
{
	return impl_->createDocument();
}

static void checkOpenContainer(Manager &impl, const std::string &name,
			       const char *method)
{
	// getOpenContainer() call has no side effects
	XmlContainer c = impl.getOpenContainer(name);
	if (!c.isNull()) {
		ostringstream s;
		s << "XmlManager::";
		s << method;
		s << "(): requires a closed container";
		throw XmlException(XmlException::INVALID_VALUE,
				   s.str());
	}
}

void XmlManager::removeContainer(const string &name)
{
	checkOpenContainer(*impl_, name, "removeContainer");
	impl_->removeContainer(0, name);
}

void XmlManager::renameContainer(const string &oldName,
				 const string &newName)
{
	checkOpenContainer(*impl_, oldName, "renameContainer");
	checkOpenContainer(*impl_, newName, "renameContainer");
	impl_->renameContainer(0, oldName, newName);
}

void XmlManager::dumpContainer(const string &name, ostream *out)
{
	checkOpenContainer(*impl_, name, "dumpContainer");
	Container::dump(*impl_, name, out);
}

void XmlManager::loadContainer(const string &name, istream *in,
			       unsigned long *lineno,
			       XmlUpdateContext &context)
{
	Container::load(*impl_, name, in, lineno, context);
}

void XmlManager::verifyContainer(const string &name, ostream *out,
				 u_int32_t flags)
{
	impl_->checkFlags(Log::misc_flag_info, "verifyContainer()", flags,
			  DB_SALVAGE|DB_AGGRESSIVE);
	checkOpenContainer(*impl_, name, "verifyContainer");
	Container::verify(*impl_, name, out, flags);
}

void XmlManager::upgradeContainer(const string &name,
				  XmlUpdateContext &context)
{
	checkOpenContainer(*impl_, name, "upgradeContainer");
	Container::upgradeContainer(name, *impl_, context);
}

void XmlManager::removeContainer(XmlTransaction &txn, const string &name)
{
	checkOpenContainer(*impl_, name, "removeContainer");
	impl_->removeContainer(txn, name);
}

void XmlManager::renameContainer(XmlTransaction &txn,
				 const string &oldName,
				 const string &newName)
{
	checkOpenContainer(*impl_, oldName, "renameContainer");
	checkOpenContainer(*impl_, newName, "renameContainer");
	impl_->renameContainer(txn, oldName, newName);
}

void XmlManager::reindexContainer(const std::string &name,
			      XmlUpdateContext &context)
{
	XmlContainerConfig config;
	reindexContainer(name, context, config);
}

void XmlManager::reindexContainer(const std::string &name,
				  XmlUpdateContext &context,
				  const XmlContainerConfig &flags)
{
	impl_->checkFlags(Log::open_container_flag_info, "reindexContainer()", flags,
		DBXML_STATISTICS|DBXML_NO_STATISTICS|DBXML_INDEX_NODES|DBXML_NO_INDEX_NODES);
	checkOpenContainer(*impl_, name, "reindexContainer");
	Container::reindexContainer(0, *impl_, name, context, flags);
}

void XmlManager::reindexContainer(XmlTransaction &txn,
				  const std::string &name,
				  XmlUpdateContext &context,
				  const XmlContainerConfig &flags)
{
	impl_->checkFlags(Log::open_container_flag_info, "reindexContainer()", flags,
		DBXML_STATISTICS|DBXML_NO_STATISTICS|DBXML_INDEX_NODES|DBXML_NO_INDEX_NODES);
	checkOpenContainer(*impl_, name, "reindexContainer");
	Container::reindexContainer(txn, *impl_, name, context, flags);
}

void XmlManager::truncateContainer(const std::string &name,
				   XmlUpdateContext &context)
{
	checkOpenContainer(*impl_, name, "truncateContainer");
	Container::truncateContainer(0, *impl_, name, context);
}

void XmlManager::truncateContainer(const std::string &name,
				   XmlUpdateContext &context,
				   const XmlContainerConfig &flags)
{
	truncateContainer(name, context);
}

void XmlManager::truncateContainer(XmlTransaction &txn,
				   const std::string &name,
				   XmlUpdateContext &context,
				   const XmlContainerConfig &flags)
{
	truncateContainer(txn, name, context);
}

void XmlManager::truncateContainer(XmlTransaction &txn,
				   const std::string &name,
				   XmlUpdateContext &context)
{
	checkOpenContainer(*impl_, name, "truncateContainer");
	Container::truncateContainer(txn, *impl_, name, context);
}

void XmlManager::compactContainer(const std::string &name,
				  XmlUpdateContext &context,
				  const XmlContainerConfig &config)
{
	compactContainer(name, context);
}

void XmlManager::compactContainer(const std::string &name,
				  XmlUpdateContext &context)
{
	// While compaction is supported as of 4.4, there's a bug
	// that causes it to fail for BDB XML containers, so require
	// 4.5 for compaction
#if DBVER > 44
	checkOpenContainer(*impl_, name, "compactContainer");
	Container::compactContainer(0, *impl_, name, context);
#else
	throw XmlException(XmlException::INVALID_VALUE,
			   "XmlManager::compactContainer is not supported in Berkeley DB versions before 4.5");
#endif
}

void XmlManager::compactContainer(XmlTransaction &txn,
				  const std::string &name,
				  XmlUpdateContext &context,
				  const XmlContainerConfig &flags)
{
	compactContainer(txn, name, context);
}

void XmlManager::compactContainer(XmlTransaction &txn,
				  const std::string &name,
				  XmlUpdateContext &context)
{
#if DBVER > 44	
	checkOpenContainer(*impl_, name, "compactContainer");
	Container::compactContainer(txn, *impl_, name, context);
#else
	throw XmlException(XmlException::INVALID_VALUE,
			   "XmlManager::compactContainer is not supported in Berkeley DB versions before 4.5");
#endif
}

XmlQueryContext XmlManager::createQueryContext(XmlQueryContext::ReturnType rt,
					       XmlQueryContext::EvaluationType et)
{
	return new QueryContext(*this, rt, et);
}

XmlUpdateContext XmlManager::createUpdateContext()
{
	return new UpdateContext(*this);
}

XmlQueryExpression XmlManager::prepare(const std::string &xquery,
				       XmlQueryContext &context)
{
	// Count prepare
	INCR(Counters::num_prepare);
	return new QueryExpression(xquery, context, 0);
}

XmlQueryExpression XmlManager::prepare(XmlTransaction &txn,
				       const std::string &xquery,
				       XmlQueryContext &context)
{
	// Count prepare
	INCR(Counters::num_prepare);
	return new QueryExpression(xquery, context, txn);
}

XmlResults XmlManager::query(const std::string &xquery,
			     XmlQueryContext &context, u_int32_t flags)
{
	// Count query
	INCR(Counters::num_query);
	// flags are checked in execute
	return prepare(xquery, context).execute(context, flags);
}

XmlResults XmlManager::query(XmlTransaction &txn, const std::string &xquery,
			     XmlQueryContext &context, u_int32_t flags)
{
	// Count query
	INCR(Counters::num_query);
	// flags are checked in execute
	return prepare(txn, xquery, context).execute(txn, context, flags);
}

XmlResults XmlManager::createResults()
{
	return new ValueResults(*this);
}

XmlTransaction XmlManager::createTransaction(DB_TXN *toUse)
{
	return impl_->createTransaction(toUse);
}

XmlTransaction XmlManager::createTransaction(u_int32_t flags)
{
	return impl_->createTransaction(flags);
}

XmlIndexLookup XmlManager::createIndexLookup(
	XmlContainer &cont, const std::string &uri, const std::string &name,
	const std::string &index, const XmlValue &value,
	XmlIndexLookup::Operation op)
{
	return XmlIndexLookup(new IndexLookup(cont, uri, name, index,
					      value, op));
}


DBXML_EXPORT const char *DbXml::dbxml_version(int *majorp, int *minorp, 
					      int *patchp)
{
	if (majorp != NULL)
		*majorp = DBXML_VERSION_MAJOR;
	if (minorp != NULL)
		*minorp = DBXML_VERSION_MINOR;
	if (patchp != NULL)
		*patchp = DBXML_VERSION_PATCH;
	return DBXML_VERSION_STRING;
}

DBXML_EXPORT const char *DbXml::dbxml_db_version(int *majorp, int *minorp, 
						 int *patchp)
{
	return db_version(majorp, minorp, patchp);
}

DBXML_EXPORT void DbXml::setLogLevel(LogLevel level, bool enabled)
{
	Log::setLogLevel((Log::ImplLogLevel)level, enabled);
}

DBXML_EXPORT void DbXml::setLogCategory(LogCategory category, bool enabled)
{
	Log::setLogCategory((Log::ImplLogCategory)category, enabled);
}
