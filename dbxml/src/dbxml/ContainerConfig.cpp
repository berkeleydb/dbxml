//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// $Id
// 

#include "DbXmlInternal.hpp"
#include "dbxml/XmlContainerConfig.hpp"
#include "ContainerConfig.hpp"
#include "Manager.hpp"

using namespace DbXml;

ContainerConfig::ContainerConfig()
	: mode_(0), dbOpenFlags_(0), dbSetFlags_(0), seqFlags_(0), xmlFlags_(0),
	type_(XmlContainer::NodeContainer), 
	compressionName_(XmlContainerConfig::DEFAULT_COMPRESSION),
	  mgr_(0), pageSize_(0), sequenceIncr_(5), containerOwned_(false)
{}

ContainerConfig::ContainerConfig(XmlContainer::ContainerType type,
	int mode, const char *name) 
	: mode_(mode), dbOpenFlags_(0), dbSetFlags_(0), seqFlags_(0), xmlFlags_(0), type_(type),
	  compressionName_(name), mgr_(0), pageSize_(0), sequenceIncr_(5),
	  containerOwned_(false)
{}

ContainerConfig::~ContainerConfig()
{}

ContainerConfig::ContainerConfig(const ContainerConfig &o)
: mode_(o.getMode()), dbOpenFlags_(o.getDbOpenFlags()), dbSetFlags_(o.getDbSetFlags()), 
	seqFlags_(o.getSeqFlags()), xmlFlags_(o.getXmlFlags()), 
	type_(o.getContainerType()), compressionName_(o.getCompressionName()), mgr_(0),
	  pageSize_(o.getPageSize()), sequenceIncr_(o.getSequenceIncrement()),
	  containerOwned_(false)
{}

ContainerConfig::ContainerConfig(u_int32_t flags)
	: mode_(0),  dbOpenFlags_(0), dbSetFlags_(0), seqFlags_(0), xmlFlags_(0),
	type_(XmlContainer::NodeContainer), 
	compressionName_(XmlContainerConfig::DEFAULT_COMPRESSION),
	  mgr_(0), pageSize_(0), sequenceIncr_(5), containerOwned_(false)
{
	separateFlags(flags);
}

ContainerConfig &ContainerConfig::operator=(const ContainerConfig &o)
{
	if (this != &o) {
		compressionName_ = o.getCompressionName();
		type_ = o.getContainerType();
		mode_ = o.getMode();
		dbOpenFlags_ = o.getDbOpenFlags();
		dbSetFlags_ = o.getDbSetFlags();
		seqFlags_ = o.getSeqFlags();
		xmlFlags_ = o.getXmlFlags();
		mgr_ = 0;
		pageSize_ = o.getPageSize();
		sequenceIncr_ = o.getSequenceIncrement();
		containerOwned_ = false;
	}
	return *this;
}

const char *ContainerConfig::getCompressionName() const
{
	return compressionName_.c_str();
}
void ContainerConfig::setCompressionName(const char *name)
{
	if(!name)
		throw XmlException(XmlException::INVALID_VALUE, "The compression name cannot be null.");
	owned();
	lock();
	compressionName_ = name;
	unlock();
}

int ContainerConfig::getMode() const
{
	return mode_;
}

void ContainerConfig::setMode(int mode)
{
	owned();
	lock();
	mode_ = mode;
	unlock();
}

XmlContainer::ContainerType ContainerConfig::getContainerType() const
{
	return type_;
}

void ContainerConfig::setContainerType(XmlContainer::ContainerType type)
{
	owned();
	lock();
	type_ = type;
	unlock();
}

u_int32_t ContainerConfig::getPageSize() const
{
	return pageSize_;
}

void ContainerConfig::setPageSize(u_int32_t pageSize)
{
	if (pageSize != 0 && (pageSize < 512 || pageSize > 65536)) {
		throw XmlException(XmlException::INVALID_VALUE,
				   "Container expects a page size between 512 bytes and 64k");
	}
	owned();
	lock();
	pageSize_ = pageSize;
	unlock();
}

u_int32_t ContainerConfig::getSequenceIncrement() const
{
	return sequenceIncr_;
}

void ContainerConfig::setSequenceIncrement(u_int32_t incr)
{
	owned();
	lock();
	sequenceIncr_ = incr;
	unlock();
}

void ContainerConfig::setIndexNodes(XmlContainerConfig::ConfigState state)
{
	owned();
	lock();
	if(state == XmlContainerConfig::On){
		xmlFlags_ |= DBXML_INDEX_NODES;
		xmlFlags_ &= ~DBXML_NO_INDEX_NODES;
	}else if(state == XmlContainerConfig::Off){
		xmlFlags_ |= DBXML_NO_INDEX_NODES;
		xmlFlags_ &= ~DBXML_INDEX_NODES;
	}else{
		xmlFlags_ &= ~DBXML_NO_INDEX_NODES;
		xmlFlags_ &= ~DBXML_INDEX_NODES;
	}
	unlock();
}

XmlContainerConfig::ConfigState ContainerConfig::getIndexNodes() const
{
	if((xmlFlags_ & DBXML_INDEX_NODES) != 0)
		return XmlContainerConfig::On;
	else if((xmlFlags_ & DBXML_NO_INDEX_NODES) != 0)
		return XmlContainerConfig::Off;
	else
		return XmlContainerConfig::UseDefault;
}

void ContainerConfig::setChecksum(bool value)
{
	owned();
	lock();
	if(value) {
		xmlFlags_ |= DBXML_CHKSUM;
		dbSetFlags_ |= DBXML_CHKSUM;
	} else {
		xmlFlags_ &= ~DBXML_CHKSUM;
		dbSetFlags_ &= ~DBXML_CHKSUM;
	}
	unlock();
}

bool ContainerConfig::getChecksum() const
{
	return ((xmlFlags_ & DBXML_CHKSUM) != 0);
}

void ContainerConfig::setEncrypted(bool value)
{
	owned();
	lock();
	if(value) {
		xmlFlags_ |= DBXML_ENCRYPT;
		dbSetFlags_ |= DBXML_ENCRYPT;
	} else {
		xmlFlags_ &= ~DBXML_ENCRYPT;
		dbSetFlags_ &= ~DBXML_ENCRYPT;
	}
	unlock();
}
bool ContainerConfig::getEncrypted() const
{
	return ((xmlFlags_ & DBXML_ENCRYPT) != 0);
}

void ContainerConfig::setAllowValidation(bool value)
{
	owned();
	lock();
	if(value)
		xmlFlags_ |= DBXML_ALLOW_VALIDATION;
	else 
		xmlFlags_ &= ~DBXML_ALLOW_VALIDATION;
	unlock();
}
bool ContainerConfig::getAllowValidation() const
{
	return ((xmlFlags_ & DBXML_ALLOW_VALIDATION) != 0);
}

void ContainerConfig::setStatistics(XmlContainerConfig::ConfigState state)
{
	owned();
	lock();
	if(state == XmlContainerConfig::On){
		xmlFlags_ |= DBXML_STATISTICS;
		xmlFlags_ &= ~DBXML_NO_STATISTICS;
	}else if(state == XmlContainerConfig::Off){
		xmlFlags_ |= DBXML_NO_STATISTICS;
		xmlFlags_ &= ~DBXML_STATISTICS;
	}else{
		xmlFlags_ &= ~DBXML_NO_STATISTICS;
		xmlFlags_ &= ~DBXML_STATISTICS;
	}
	unlock();
}

XmlContainerConfig::ConfigState ContainerConfig::getStatistics() const
{
	if((xmlFlags_ & DBXML_STATISTICS) != 0)
		return XmlContainerConfig::On;
	else if((xmlFlags_ & DBXML_NO_STATISTICS) != 0)
		return XmlContainerConfig::Off;
	else
		return XmlContainerConfig::UseDefault;
}

void ContainerConfig::setTransactional(bool value)
{
	owned();
	lock();
	if(value)
		xmlFlags_ |= DBXML_TRANSACTIONAL;
	else 
		xmlFlags_ &= ~DBXML_TRANSACTIONAL;
	unlock();
}
bool ContainerConfig::getTransactional() const
{
	return (xmlFlags_ & DBXML_TRANSACTIONAL) != 0;
}

void ContainerConfig::setAllowCreate(bool value)
{
	owned();
	lock();
	if(value) {
		dbOpenFlags_ |= DB_CREATE;
		seqFlags_ |= DB_CREATE;
	} else {
		dbOpenFlags_ &= ~DB_CREATE;
		seqFlags_ &= ~DB_CREATE;
	}
	unlock();
}
bool ContainerConfig::getAllowCreate() const
{
	return (dbOpenFlags_ & DB_CREATE) != 0;
}

void ContainerConfig::setExclusiveCreate(bool value)
{
	owned();
	lock();
	if(value) {
		dbOpenFlags_ |= DB_EXCL;
		seqFlags_ |= DB_EXCL;
	} else {
		dbOpenFlags_ &= ~DB_EXCL;
		seqFlags_ &= ~DB_EXCL;
	}
	unlock();
}
bool ContainerConfig::getExclusiveCreate() const
{
	return (dbOpenFlags_ & DB_EXCL) != 0;
}

void ContainerConfig::setNoMMap(bool value)
{
	owned();
	lock();
	if(value)
		dbOpenFlags_ |= DB_NOMMAP;
	else 
		dbOpenFlags_ &= ~DB_NOMMAP;
	unlock();
}
bool ContainerConfig::getNoMMap() const
{
	return (dbOpenFlags_ & DB_NOMMAP) != 0;
}

void ContainerConfig::setReadOnly(bool value)
{
	owned();
	lock();
	if(value)
		dbOpenFlags_ |= DB_RDONLY;
	else 
		dbOpenFlags_ &= ~DB_RDONLY;
	unlock();
}
bool ContainerConfig::getReadOnly() const
{
	return (dbOpenFlags_ & DB_RDONLY) != 0;
}

void ContainerConfig::setMultiversion(bool value)
{
	owned();
	lock();
	if(value)
		dbOpenFlags_ |= DB_MULTIVERSION;
	else 
		dbOpenFlags_ &= ~DB_MULTIVERSION;
	unlock();
}
bool ContainerConfig::getMultiversion() const
{
	return (dbOpenFlags_ & DB_MULTIVERSION) != 0;
}

void ContainerConfig::setReadUncommitted(bool value)
{
	owned();
	lock();
	if(value)
		dbOpenFlags_ |= DB_READ_UNCOMMITTED;
	else 
		dbOpenFlags_ &= ~DB_READ_UNCOMMITTED;
	unlock();
}
bool ContainerConfig::getReadUncommitted() const
{
	return (dbOpenFlags_ & DB_READ_UNCOMMITTED) != 0;
}

void ContainerConfig::setThreaded(bool value)
{
	owned();
	lock();
	if(value) {
		dbOpenFlags_ |= DB_THREAD;
		seqFlags_ |= DB_THREAD;
	} else {
		dbOpenFlags_ &= ~DB_THREAD;
		seqFlags_ &= ~DB_THREAD;
	}
	unlock();
}
bool ContainerConfig::getThreaded() const
{
	return (dbOpenFlags_ & DB_THREAD) != 0;
}

void ContainerConfig::setTransactionNotDurable(bool value)
{
	owned();
	lock();
	if(value)
		dbSetFlags_ |= DB_TXN_NOT_DURABLE;
	else 
		dbSetFlags_ &= ~DB_TXN_NOT_DURABLE;
	unlock();
}
bool ContainerConfig::getTransactionNotDurable() const
{
	return (dbSetFlags_ & DB_TXN_NOT_DURABLE) != 0;
}

void ContainerConfig::setManager(Manager *mgr)
{
	mgr_ = mgr;
}

void ContainerConfig::setDbOpenFlags(u_int32_t flags)
{
	owned();
	lock();
	dbOpenFlags_ = flags;
	unlock();
}

void ContainerConfig::setDbSetFlags(u_int32_t flags)
{
	owned();
	lock();
	dbSetFlags_ = flags;
	unlock();
}

void ContainerConfig::setSeqFlags(u_int32_t flags)
{
	owned();
	lock();
	seqFlags_ = flags;
	unlock();
}

void ContainerConfig::setXmlFlags(u_int32_t flags)
{
	owned();
	lock();
	xmlFlags_ = flags;
	unlock();
}

u_int32_t ContainerConfig::getDbOpenFlags() const
{
	return dbOpenFlags_;
}

u_int32_t ContainerConfig::getDbSetFlagsForSet_Flags() const
{
	u_int32_t flags = 0;
	if (dbSetFlags_ & DBXML_CHKSUM) {
#if (DB_VERSION_MAJOR == 4 && DB_VERSION_MINOR < 2)
		flags |= DB_CHKSUM_SHA1;
#else
		flags |= DB_CHKSUM;
#endif
	}
	if (dbSetFlags_ & DBXML_ENCRYPT)
		flags |= DB_ENCRYPT;
	if (dbSetFlags_ & DB_TXN_NOT_DURABLE)
		flags |= DB_TXN_NOT_DURABLE;
	return flags;
}

u_int32_t ContainerConfig::getDbSetFlags() const
{
	return dbSetFlags_;
}

u_int32_t ContainerConfig::getSeqFlags() const
{
	return seqFlags_;
}

u_int32_t ContainerConfig::getXmlFlags() const
{
	return xmlFlags_;
}

bool ContainerConfig::getContainerOwned() const 
{
	return containerOwned_;
}

void ContainerConfig::setContainerOwned(bool owned) 
{
	lock();
	containerOwned_ = owned;
	unlock();
}

void ContainerConfig::separateFlags(u_int32_t flags)
{
	if (((flags & DB_READ_UNCOMMITTED) != 0)
		|| ((flags & DB_TXN_NOT_DURABLE) != 0)) {
			std::string message = "The flags DB_READ_UNCOMMITTED and DB_TXN_NOT_DURABLE cannot be used directly";
			message += ", you must set these flags using XmlContainerConfig.setReadUncommitted ";
			message += "and XmlContainerConfig.setTransactionNotDurable.";
		throw XmlException(XmlException::INVALID_VALUE, message);
	}
	xmlFlags_ = dbOpenFlags_ = dbSetFlags_ = seqFlags_ = 0;
	if((flags & DBXML_INDEX_NODES) != 0)
		setIndexNodes(XmlContainerConfig::On);
	else if((flags & DBXML_NO_INDEX_NODES) != 0)
		setIndexNodes(XmlContainerConfig::Off);
	if((flags & DBXML_TRANSACTIONAL) != 0)
		setTransactional(true);
	if((flags & DBXML_STATISTICS) != 0)
		setStatistics(XmlContainerConfig::On);
	else if((flags & DBXML_NO_STATISTICS) != 0)
		setStatistics(XmlContainerConfig::Off);
	if((flags & DBXML_CHKSUM) != 0)
		setChecksum(true);
	if((flags & DBXML_ALLOW_VALIDATION) != 0)
		setAllowValidation(true);
	if((flags & DBXML_ENCRYPT) != 0)
		setEncrypted(true);

	if((flags & DB_NOMMAP) != 0)
		setNoMMap(true);
	if((flags & DB_THREAD) != 0)
		setThreaded(true);
	if((flags & DB_CREATE) != 0)
		setAllowCreate(true);
	if((flags & DB_EXCL) != 0)
		setExclusiveCreate(true);
	if((flags & DB_RDONLY) != 0)
		setReadOnly(true);
	if((flags & DB_MULTIVERSION) != 0)
		setMultiversion(true);
}

void ContainerConfig::setFlags(const ContainerConfig &flags)
{
	setTransactional(flags.getTransactional());
	setIndexNodes(flags.getIndexNodes());
	setEncrypted(flags.getEncrypted());
	setStatistics(flags.getStatistics());
	setAllowValidation(flags.getAllowValidation());
	setChecksum(flags.getChecksum());
	setDbOpenFlags(flags.getDbOpenFlags());
	setDbSetFlags(flags.getDbSetFlags());
	setSeqFlags(flags.getSeqFlags());
}

void ContainerConfig::lock()
{
	if(mgr_) mgr_->lock();
}

void ContainerConfig::unlock()
{
	if(mgr_) mgr_->unlock();
}

void ContainerConfig::owned() const
{
	if(containerOwned_)
		throw XmlException(XmlException::INVALID_VALUE, "You cannot alter the state of the XmlContainerConfig owned by the container.");
}

