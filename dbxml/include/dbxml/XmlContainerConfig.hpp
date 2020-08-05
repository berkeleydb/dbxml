//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// 

#ifndef __XMLCONTAINERCONFIG_HPP
#define	__XMLCONTAINERCONFIG_HPP

#include "XmlPortability.hpp"
#include "XmlContainer.hpp"

namespace DbXml
{

class ContainerConfig;

class DBXML_EXPORT XmlContainerConfig
{
public:  
	///< The name for using no compression        
	static const char *NO_COMPRESSION;
	///< The name for the built in default compression algorithm
	static const char *DEFAULT_COMPRESSION;
	enum ConfigState{
		On,
		Off,
		UseDefault
	};

	XmlContainerConfig();
	~XmlContainerConfig();
	XmlContainerConfig(const XmlContainerConfig &o);
	XmlContainerConfig &operator=(const XmlContainerConfig &o);

	/** @name The name that the compression callback
	is registered with the XmlManager */
	const char *getCompressionName() const;
	void setCompressionName(const char *name);
	
	/** @name For setting the container mode, only relevant for UNIX*/
	int getMode() const;
	void setMode(int mode);

	/** @name For setting the container type, WholeDocument or NodeStorage */
	XmlContainer::ContainerType getContainerType() const;
	void setContainerType(XmlContainer::ContainerType type);

	/** @name Sets the default page size */
	u_int32_t getPageSize() const;
	void setPageSize(u_int32_t pageSize);

	u_int32_t getSequenceIncrement() const;
	void setSequenceIncrement(u_int32_t incr);

	void setIndexNodes(ConfigState state);
	ConfigState getIndexNodes() const;

	void setChecksum(bool value);
	bool getChecksum() const;

	void setEncrypted(bool value);
	bool getEncrypted() const;

	void setAllowValidation(bool value);
	bool getAllowValidation() const;

	void setStatistics(ConfigState state);
	ConfigState getStatistics() const;

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

	/** @name Private Methods (for internal use) */
	// @{
	XmlContainerConfig(u_int32_t flags);
	XmlContainerConfig(int flags);	
	XmlContainerConfig(XmlContainer::ContainerType type);
	XmlContainerConfig(XmlContainer::ContainerType type, int mode, const char *name);
	XmlContainerConfig(const char *name);
	operator u_int32_t() const;
	XmlContainerConfig(const ContainerConfig &config);  
	operator const ContainerConfig *() const
	{
		return config_;
	}
	operator const ContainerConfig &() const
	{
		return *config_;
	} 
	operator ContainerConfig *()
	{
		return config_;
	}

	//@}
private:
	ContainerConfig *config_;
};

// Flags used for container create/open
static const u_int32_t DBXML_ALLOW_VALIDATION      = 0x00100000;
static const u_int32_t DBXML_TRANSACTIONAL         = 0x00200000;
static const u_int32_t DBXML_CHKSUM                = 0x00400000;
static const u_int32_t DBXML_ENCRYPT               = 0x00800000;
static const u_int32_t DBXML_INDEX_NODES           = 0x01000000;
static const u_int32_t DBXML_NO_INDEX_NODES        = 0x00010000;
static const u_int32_t DBXML_STATISTICS            = 0x02000000;
static const u_int32_t DBXML_NO_STATISTICS         = 0x04000000;

DBXML_EXPORT XmlContainerConfig operator|(const XmlContainerConfig &a, const XmlContainerConfig &b);
DBXML_EXPORT XmlContainerConfig operator|(u_int32_t flags, const XmlContainerConfig &b);
DBXML_EXPORT XmlContainerConfig operator|(const XmlContainerConfig &a, u_int32_t flags);
DBXML_EXPORT XmlContainerConfig operator|(int flags, const XmlContainerConfig &b);
DBXML_EXPORT XmlContainerConfig operator|(const XmlContainerConfig &a, int flags);
DBXML_EXPORT XmlContainerConfig operator|(XmlContainer::ContainerType type, const XmlContainerConfig &b);
DBXML_EXPORT XmlContainerConfig operator|(const XmlContainerConfig &a, XmlContainer::ContainerType type);
DBXML_EXPORT XmlContainerConfig operator|(const char *name, const XmlContainerConfig &b);
DBXML_EXPORT XmlContainerConfig operator|(const XmlContainerConfig &a, const char *name);
}
#endif

