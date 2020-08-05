//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __CONFIGURATIONDATABASE_HPP
#define	__CONFIGURATIONDATABASE_HPP

#include <string>
#include <db.h>
#include "SharedPtr.hpp"
#include "DbWrapper.hpp"
#include "dbxml/XmlContainer.hpp"

namespace DbXml
{

class Buffer;
class IndexSpecification;
class Transaction;
class DocID;

enum IndexVersion {
	DEFAULT_INDEX_VERSION   = 1, ///< No index version (before index versions)
	SUBSTRING_3CHAR_VERSION = 2, ///< Substring indexes include keys for lookups with < 3 chars
	CURRENT_INDEX_VERSION = SUBSTRING_3CHAR_VERSION
};

/**
 * Wraps the configuration database, which is part of a container
 */
class ConfigurationDatabase
{
public:
	typedef SharedPtr<ConfigurationDatabase> Ptr;

	ConfigurationDatabase(DB_ENV *env, Transaction *txn,
			      const std::string &name,
			      ContainerConfig &config,
			      bool &indexNodes,
			      bool doVersionCheck);
	~ConfigurationDatabase();
	int generateID(Transaction *txn, DocID &newId);
	int getConfigurationItem(Transaction *txn, const char *key,
				 size_t keyLength, Buffer &b, bool lock) const;
	int putConfigurationItem(Transaction *txn, const char *key,
				 const Buffer &b);

	int getVersion(Transaction *txn, unsigned int &version) const;
	int getVersions(Transaction *txn, unsigned int &current_version,
			unsigned int &save_version) const;

	int putIndexSpecification(Transaction *txn,
				  const IndexSpecification &index);
	int getIndexSpecification(Transaction *txn, IndexSpecification &index,
				  bool lock = false) const;

	void setIndexNodes(Transaction *txn, bool indexNodes);

	std::string getCompressionName(Transaction *txn) const;
	int putCompressionName(Transaction *txn, const char *name);

	int getIndexVersion(Transaction *txn, unsigned int &version) const;
	int updateIndexVersion(Transaction *txn);

	void sync() {
		database_.sync();
	}

	void run(Transaction *txn, DbWrapper::DbFunctionRunner &runner) {
		runner.run(txn, database_);
		runner.run(txn, seqDatabase_);
	}

	static XmlContainer::ContainerType readContainerType(
		DB_ENV *env, const std::string &name);

	static int dump(DB_ENV *env, const std::string &name,
			std::ostream *out);
	static int load(DB_ENV *env, const std::string &name,
			std::istream *in, unsigned long *lineno);
	static int verify(DB_ENV *env, const std::string &name,
			  std::ostream *out, u_int32_t flags);
	// upgrade
	static int putVersion(Transaction *txn, DbWrapper &db,
			      unsigned int version);
	static void upgrade(const std::string &name,
			    const std::string &tname, Manager &mgr,
			    int old_version, int current_version);

private:
	// no need for copy and assignment
	ConfigurationDatabase(const ConfigurationDatabase&);
	ConfigurationDatabase &operator=(const ConfigurationDatabase&);
	
	int checkVersion(Transaction *txn, bool rdonly);
	int checkIndexSpecification(Transaction *txn);
	bool checkIndexNodes(Transaction *txn, bool indexNodes,
			     bool isReadonly);
	static XmlContainer::ContainerType checkContainerType(
		Transaction *txn, DbWrapper &db,
		XmlContainer::ContainerType type, bool isReadonly);
	
	DB_ENV *environment_;
	std::string name_;
	mutable DbWrapper database_;
	mutable DbWrapper seqDatabase_;
	DB_SEQUENCE *seq_;
	u_int32_t seqIncr_;
};

}

#endif

