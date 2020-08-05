//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
// $Id:$
// 

#include "dbxml/XmlContainerConfig.hpp"
#include "DbXmlInternal.hpp"
#include "ContainerConfig.hpp"

using namespace DbXml;

const char *XmlContainerConfig::NO_COMPRESSION = "NONE";
const char *XmlContainerConfig::DEFAULT_COMPRESSION = "DEFAULT";

XmlContainerConfig::XmlContainerConfig()
	: config_(new ContainerConfig())
{}

XmlContainerConfig::XmlContainerConfig(XmlContainer::ContainerType type,
	int mode, const char *name) 
	: config_(new ContainerConfig(type, mode, name))
{}

XmlContainerConfig::~XmlContainerConfig()
{
	delete config_;
}

XmlContainerConfig::XmlContainerConfig(const XmlContainerConfig &o)
	: config_(new ContainerConfig(*o))
{}

XmlContainerConfig::XmlContainerConfig(u_int32_t flags)
	: config_(new ContainerConfig(flags))
{}

XmlContainerConfig &XmlContainerConfig::operator=(const XmlContainerConfig &o)
{
	if (this != &o) {
		delete config_;
		config_ = new ContainerConfig(*o);
	}
	return *this;
}

const char *XmlContainerConfig::getCompressionName() const
{
	return config_->getCompressionName();
}
void XmlContainerConfig::setCompressionName(const char *name)
{
	config_->setCompressionName(name);
}

int XmlContainerConfig::getMode() const
{
	return config_->getMode();
}

void XmlContainerConfig::setMode(int mode)
{
	config_->setMode(mode);
}

XmlContainer::ContainerType XmlContainerConfig::getContainerType() const
{
	return config_->getContainerType();
}

void XmlContainerConfig::setContainerType(XmlContainer::ContainerType type)
{
	config_->setContainerType(type);
}

u_int32_t XmlContainerConfig::getPageSize() const
{
	return config_->getPageSize();
}

void XmlContainerConfig::setPageSize(u_int32_t pageSize)
{
	config_->setPageSize(pageSize);
}

u_int32_t XmlContainerConfig::getSequenceIncrement() const
{
	return config_->getSequenceIncrement();
}

void XmlContainerConfig::setSequenceIncrement(u_int32_t incr)
{
	config_->setSequenceIncrement(incr);
}

void XmlContainerConfig::setIndexNodes(ConfigState state)
{
	config_->setIndexNodes(state);
}

XmlContainerConfig::ConfigState XmlContainerConfig::getIndexNodes() const
{
	return config_->getIndexNodes();
}

void XmlContainerConfig::setChecksum(bool value)
{
	config_->setChecksum(value);
}

bool XmlContainerConfig::getChecksum() const
{
	return config_->getChecksum();
}

void XmlContainerConfig::setEncrypted(bool value)
{
	config_->setEncrypted(value);
}
bool XmlContainerConfig::getEncrypted() const
{
	return config_->getEncrypted();
}

void XmlContainerConfig::setAllowValidation(bool value)
{
	config_->setAllowValidation(value);
}
bool XmlContainerConfig::getAllowValidation() const
{
	return config_->getAllowValidation();
}

void XmlContainerConfig::setStatistics(ConfigState state)
{
	config_->setStatistics(state);
}

XmlContainerConfig::ConfigState XmlContainerConfig::getStatistics() const
{
	return config_->getStatistics();
}

void XmlContainerConfig::setTransactional(bool value)
{
	config_->setTransactional(value);
}
bool XmlContainerConfig::getTransactional() const
{
	return config_->getTransactional();
}

void XmlContainerConfig::setAllowCreate(bool value)
{
	config_->setAllowCreate(value);
}
bool XmlContainerConfig::getAllowCreate() const
{
	return config_->getAllowCreate();
}

void XmlContainerConfig::setExclusiveCreate(bool value)
{
	config_->setExclusiveCreate(value);
}
bool XmlContainerConfig::getExclusiveCreate() const
{
	return config_->getExclusiveCreate();
}

void XmlContainerConfig::setNoMMap(bool value)
{
	config_->setNoMMap(value);
}
bool XmlContainerConfig::getNoMMap() const
{
	return config_->getNoMMap();
}

void XmlContainerConfig::setReadOnly(bool value)
{
	config_->setReadOnly(value);
}
bool XmlContainerConfig::getReadOnly() const
{
	return config_->getReadOnly();
}

void XmlContainerConfig::setMultiversion(bool value)
{
	config_->setMultiversion(value);
}
bool XmlContainerConfig::getMultiversion() const
{
	return config_->getMultiversion();
}

void XmlContainerConfig::setReadUncommitted(bool value)
{
	config_->setReadUncommitted(value);
}
bool XmlContainerConfig::getReadUncommitted() const
{
	return config_->getReadUncommitted();
}

void XmlContainerConfig::setThreaded(bool value)
{
	config_->setThreaded(value);
}
bool XmlContainerConfig::getThreaded() const
{
	return config_->getThreaded();
}

void XmlContainerConfig::setTransactionNotDurable(bool value)
{
	config_->setTransactionNotDurable(value);
}
bool XmlContainerConfig::getTransactionNotDurable() const
{
	return config_->getTransactionNotDurable();
}

XmlContainerConfig::XmlContainerConfig(const char *name)
	: config_(new ContainerConfig())
{
	config_->setCompressionName(name);
}

XmlContainerConfig::operator u_int32_t() const
{
	return config_->getXmlFlags()|config_->getDbOpenFlags()|config_->getDbSetFlags()|config_->getSeqFlags();
}

XmlContainerConfig::XmlContainerConfig(const ContainerConfig &config)
	: config_(new ContainerConfig(config))
{}

XmlContainerConfig::XmlContainerConfig(int flags)
	: config_(new ContainerConfig(flags))
{}
	
XmlContainerConfig::XmlContainerConfig(XmlContainer::ContainerType type)
	: config_(new ContainerConfig())
{
	config_->setContainerType(type);
}

XmlContainerConfig DbXml::operator|(const XmlContainerConfig &a, const XmlContainerConfig &b)
{
	XmlContainerConfig config(b);
	(*config).setDbOpenFlags((*a).getDbOpenFlags()|(*b).getDbOpenFlags());
	(*config).setDbSetFlags((*a).getDbSetFlags()|(*b).getDbSetFlags());
	(*config).setSeqFlags((*a).getSeqFlags()|(*b).getSeqFlags());
	(*config).setXmlFlags((*a).getXmlFlags()|(*b).getXmlFlags());
	return config;
}

XmlContainerConfig DbXml::operator|(const char *name, const XmlContainerConfig &b)
{
	XmlContainerConfig config(b);
	config.setCompressionName(name);
	return config;
  
}

XmlContainerConfig DbXml::operator|(const XmlContainerConfig &a, const char *name)
{
	XmlContainerConfig config(a);
	config.setCompressionName(name);
	return config;
}

XmlContainerConfig DbXml::operator|(XmlContainer::ContainerType type, const XmlContainerConfig &b)
{
	XmlContainerConfig config(b);
	config.setContainerType(type);
	return config;
  
}

XmlContainerConfig DbXml::operator|(const XmlContainerConfig &a, XmlContainer::ContainerType type)
{
	XmlContainerConfig config(a);
	config.setContainerType(type);
	return config;
}
  
XmlContainerConfig DbXml::operator|(u_int32_t flags, const XmlContainerConfig &b)
{
	XmlContainerConfig config(flags);
	return config|b;
  
}

XmlContainerConfig DbXml::operator|(const XmlContainerConfig &a, u_int32_t flags)
{
	XmlContainerConfig config(flags);
	return config|a;
}

XmlContainerConfig DbXml::operator|(int flags, const XmlContainerConfig &b)
{
	XmlContainerConfig config(flags);
	return config|b;
  
}

XmlContainerConfig DbXml::operator|(const XmlContainerConfig &a, int flags)
{
	XmlContainerConfig config(flags);
	return config|a;
}


		
