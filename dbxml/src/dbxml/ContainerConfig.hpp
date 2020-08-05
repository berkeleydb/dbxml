//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// 

#ifndef __CONTAINERCONFIG_HPP
#define	__CONTAINERCONFIG_HPP

#include "dbxml/XmlContainer.hpp"
#include "dbxml/XmlContainerConfig.hpp"

namespace DbXml
{

class Manager;

class DBXML_EXPORT ContainerConfig
{
public:
	ContainerConfig();
	~ContainerConfig();
	ContainerConfig(const ContainerConfig &o);
	ContainerConfig &operator=(const ContainerConfig &o);

	const char *getCompressionName() const;
	void setCompressionName(const char *name);
	
	int getMode() const;
	void setMode(int mode);

	XmlContainer::ContainerType getContainerType() const;
	void setContainerType(XmlContainer::ContainerType type);

	u_int32_t getPageSize() const;
	void setPageSize(u_int32_t pageSize);

	u_int32_t getSequenceIncrement() const;
	void setSequenceIncrement(u_int32_t incr);

	void setIndexNodes(XmlContainerConfig::ConfigState state);
	XmlContainerConfig::ConfigState getIndexNodes() const;

	void setChecksum(bool value);
	bool getChecksum() const;

	void setEncrypted(bool value);
	bool getEncrypted() const;

	void setAllowValidation(bool value);
	bool getAllowValidation() const;

	void setStatistics(XmlContainerConfig::ConfigState state);
	XmlContainerConfig::ConfigState getStatistics() const;

	void setTransactional(bool value);
	bool getTransactional() const;

	void setAllowCreate(bool value);
	bool getAllowCreate() const;

	void setExclusiveCreate(bool value);
	bool getExclusiveCreate() const;

	void setNoMMap(bool value);
	bool getNoMMap() const;

	void setReadOnly(bool value);
	bool getReadOnly() const;

	void setMultiversion(bool value);
	bool getMultiversion() const;

	void setReadUncommitted(bool value);
	bool getReadUncommitted() const;

	void setThreaded(bool value);
	bool getThreaded() const;

	void setTransactionNotDurable(bool value);
	bool getTransactionNotDurable() const;

	void setManager(Manager *mgr);
	void setXmlFlags(u_int32_t flags);
	void setDbOpenFlags(u_int32_t flags);
	void setDbSetFlags(u_int32_t flags);
	void setSeqFlags(u_int32_t flags);
	u_int32_t getXmlFlags() const;

	bool getContainerOwned() const;
	void setContainerOwned(bool owned);

	void setFlags(const ContainerConfig &flags);

	ContainerConfig(u_int32_t flags);
	ContainerConfig(XmlContainer::ContainerType type, int mode, const char *name);
	//flags to open a database
	u_int32_t getDbOpenFlags() const;
	/*Flags for db->set_flags.  getDbSetFlags gives the values as they are stored,
	which includes some DBXML_ flags, it should be called for coping the values to
	another ContainerConfig. getDbSetFlagsForSet_Flag() should to called
	to get the flags to be passed to set_flags.*/
	u_int32_t getDbSetFlags() const;
	u_int32_t getDbSetFlagsForSet_Flags() const;
	//flags to open a sequence database
	u_int32_t getSeqFlags() const;
private:
	void separateFlags(u_int32_t flags);
	inline void lock();
	inline void unlock();
	inline void owned() const;

	int mode_;
	u_int32_t dbOpenFlags_;
	u_int32_t dbSetFlags_;
	u_int32_t seqFlags_;
	u_int32_t xmlFlags_;
	XmlContainer::ContainerType type_;
	std::string compressionName_;
	Manager *mgr_;
	u_int32_t pageSize_;
	u_int32_t sequenceIncr_;
	bool containerOwned_; 
};

static const ContainerConfig DEFAULT_CONFIG((u_int32_t)0);
static const ContainerConfig CREATE_CONFIG(DB_CREATE|DB_EXCL);
}
#endif
